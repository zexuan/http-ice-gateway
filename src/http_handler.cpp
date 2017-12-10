
#include "http_handler.h"

#include <string>

#include "utils.h"

#include "ice_services.h"
#include "ice_resp_handler.h"
#include "conf.h"
#include "timer.h"

using namespace NAMESPACE_GATEWAY::rqst;
using namespace NAMESPACE_GATEWAY::service;

static const char* err_method_resp = "{\"code\":1,\"message\":\"invalid method\"}";
static const char* err_protocol_resp = "{\"code\":1,\"message\":\"invalid protocol\"}";

void router_request_handler(evhtp_request_t* r, void* arg) {

    TIMER(tv_start);

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

    std::string rqst{""};
    size_t body_len = evbuffer_get_length(r->buffer_in);
    const char* body = (const char *)evbuffer_pullup(r->buffer_in, body_len);
    if(body_len >= 0 && body != nullptr) {
        rqst = std::string(body, body_len);
    }

    std::string method{param_method->val, param_method->vlen};
    std::string protocol{param_protocol->val, param_protocol->vlen};

    std::string rqstid = RQSTID();

    // method -> srv_id
    const Config& config = Config::instance();

    struct method_config_t method_cfg;
    int res = config.getSrvId(method, method_cfg);
    if(res != 0 || method_cfg.srv_id.empty()) {
        evbuffer_add_printf(r->buffer_out, "%s", err_method_resp);
        evhtp_send_reply(r, EVHTP_RES_BADREQ);
        return;
    }
    if(method_cfg.srv_method.empty()) {
        method_cfg.srv_method = method;
    }

    bool async = true;
    IceAsyncHttpRespHandler::Call(rqstid, method_cfg.srv_id, method_cfg.srv_method, rqst, async, tv_start, r);
}

void ping_request_handler(evhtp_request_t* r, void* arg) {
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
    static const char* p = "PONG";
    evbuffer_add_reference(r->buffer_out,
                           (const char *)p, strlen(p), NULL, NULL);

    evhtp_send_reply(r, EVHTP_RES_OK);
}
