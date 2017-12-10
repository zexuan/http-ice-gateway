
#include "ice_resp_handler.h"

#include "hash_table.h"

// int值无意义
static utils::SeqHashTable<void*, int> request_hash_table;

static evhtp_res request_error_cb(evhtp_request_t* r, evhtp_error_flags errtype, void* arg) {
    request_hash_table.erase((void*)r);
}

static evthr_t* get_request_thr(evhtp_request_t * request) {
    evhtp_connection_t * htpconn;
    evthr_t            * thread;

    htpconn = evhtp_request_get_connection(request);
    thread  = htpconn->thread;

    return thread;
}

IceAsyncHttpRespHandler::IceAsyncHttpRespHandler(const std::string& rqstid, 
            evhtp_request_t* req,
            bool async)
        : IceAsyncRqstBase(rqstid, async), req_(req) {
}
IceAsyncHttpRespHandler::IceAsyncHttpRespHandler(const std::string& rqstid, 
        const std::string& srv_id,
        const std::string& method,
        const std::string& rqst,
        evhtp_request_t* req,
        bool async)
    : IceAsyncRqstBase(rqstid, srv_id, method, rqst, async), req_(req) {
}

IceAsyncHttpRespHandler::~IceAsyncHttpRespHandler() {
}

int IceAsyncHttpRespHandler::init(struct ice_config_services_t& srv_configs) {
    std::string err;
    IceServices* services = IceServices::instance();
    int res = services->init(srv_configs, err);
    if(res < 0)
        std::cerr<<"xxxxx "<<res<<" "<<err;
    return res;
}

void IceAsyncHttpRespHandler::Call(const std::string& rqstid,
                                    const std::string& srv_id,
                                    const std::string& method,
                                    const std::string& rqst,
                                    const bool async,
                                    evhtp_request_t* r) {
    // 记录请求状态
    request_hash_table.insert((void*)r, 1);
    evhtp_request_set_hook(r, evhtp_hook_on_error, (evhtp_hook)request_error_cb, NULL);

    IceAsyncCallbackHandlerPtr handle = new IceAsyncHttpRespHandler(rqstid, srv_id, method, rqst, r, async);
    handle->Start();
}

bool IceAsyncHttpRespHandler::ProcessResult(bool succ, const std::string& resp) {
    // 业务逻辑相关后续处理
    evbuffer_add_printf(req_->buffer_out, "%*s", resp.size(), resp.data());

    evthr_t* thread = get_request_thr(req_);
    if(thread) {
        // multi-thread
        evthr_defer(thread, IceAsyncHttpRespHandler::httpResponseCb, (void*)req_);
    } else {
        evhtp_send_reply(req_, EVHTP_RES_OK);
        //evhtp_request_resume(req_); ab没有-k时无法返回
    }
}

bool IceAsyncHttpRespHandler::ProcessResult(evhtp_request_t* r, bool succ, const std::string& resp) {

    // 业务逻辑相关后续处理
    evbuffer_add_printf(r->buffer_out, "%*s", resp.size(), resp.data());

    evthr_t* thread = get_request_thr(r);
    if(thread) {
        // multi-thread
        evthr_defer(thread, IceAsyncHttpRespHandler::httpResponseCb, (void*)r);
    } else {
        evhtp_send_reply(r, EVHTP_RES_OK);
        //evhtp_request_resume(r);
    }
}

void IceAsyncHttpRespHandler::httpResponseCb(evthr_t* thr, void* arg, void * shared) {
    evhtp_request_t* r = (evhtp_request_t*)arg;
    if(r && request_hash_table.contains((void*)r)) {
        request_hash_table.erase((void*)r);
        evhtp_send_reply(r, EVHTP_RES_OK);
        //evhtp_request_resume(r);
    }
}

