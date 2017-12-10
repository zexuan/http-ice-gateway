
#include "http_handler.h"

#include <string>

#include "ice_services.h"
#include "ice_resp_handler.h"

using namespace NAMESPACE_GATEWAY::rqst;
using namespace NAMESPACE_GATEWAY::service;

static const char* err_method_resp = "{\"code\":1,\"message\":\"invalid method\"}";
static const char* err_protocol_resp = "{\"code\":1,\"message\":\"invalid protocol\"}";

void router_request_handler(evhtp_request_t* r, void* arg) {

    const evhtp_kv_t* param_method = evhtp_kvs_find_kv(r->uri->query, "method");
    if(param_method == NULL) {
        evbuffer_add_printf(r->buffer_out, "%s", err_method_resp);
        evhtp_send_reply(r, EVHTP_RES_BADREQ);
        return;
    }
    const evhtp_kv_t* param_protocol = evhtp_kvs_find_kv(r->uri->query, "protocol");
    if(param_protocol == NULL) {
        evbuffer_add_printf(r->buffer_out, "%s", err_protocol_resp);
        evhtp_send_reply(r, EVHTP_RES_BADREQ);
        return;
    }
    //const char* v = evhtp_header_find(r->headers_in, "Host");  // 忽略大小写

    size_t body_len = evbuffer_get_length(r->buffer_in);
    const char* body = (const char *)evbuffer_pullup(r->buffer_in, body_len);

    std::string method{param_method->val, param_method->vlen};
    std::string protocol{param_protocol->val, param_protocol->vlen};

    std::string rqstid = "xxxx";
    std::string srv_id = "ice-async-service-001";
    std::string method1 = "Get";
    std::string rqst = "AAAAA";
    bool async = true;
    IceAsyncHttpRespHandler::Call(rqstid, srv_id, method, rqst, async, r);
}

