
#include <string>

#include "evhtp/evhtp.h"

#include "ice_services.h"
#include "ice_resp_handler.h"
#include "http_handler.h"

using namespace NAMESPACE_GATEWAY::service;

struct app_info {
    evbase_t* evbase;
    evhtp_t* evhtp;
};

struct thread_info {
    struct app_info* parent;
    evbase_t* evbase;
};

void gateway_init_thread(evhtp_t* evhtp, evthr_t* thread, void* arg) {

    struct app_info* parent = (struct app_info*)arg;
    struct thread_info* info = (struct thread_info*)calloc(sizeof(struct thread_info), 1);

    info->parent = parent;
    info->evbase = evthr_get_base(thread);

    evthr_set_aux(thread, info);
}

// ICE服务配置
struct ice_config_service_t ice_srv_configs[] = {
    {true, "ice-async-service-001", "SrvPrx", "ice-async-server", "ice.admin.conf", 1000},
    {true, "ice-async-service-002", "SrvPrx", "ice-async-server", "ice.admin.conf", 1000},
};

int main(int argc, char ** argv) {

    int num_threads     = 2;
    const char * baddr  = "127.0.0.1";
    uint16_t bport      = 8089;
    int backlog         = 1024;
    int nodelay         = 0;
    int defer_accept    = 0;
    int reuse_port      = 0;

    struct event_base* evbase = event_base_new();
    evhtp_t* evhtp = evhtp_new(evbase, NULL);

    evhtp_set_parser_flags(evhtp, EVHTP_PARSE_QUERY_FLAG_LENIENT);
    if(nodelay)
        evhtp_enable_flag(evhtp, EVHTP_FLAG_ENABLE_NODELAY);

    if(defer_accept)
        evhtp_enable_flag(evhtp, EVHTP_FLAG_ENABLE_DEFER_ACCEPT);

    if(reuse_port)
        evhtp_enable_flag(evhtp, EVHTP_FLAG_ENABLE_REUSEPORT);

    for(request_handler_t* p=request_handlers; p && p->path; p++) {
        evhtp_set_cb(evhtp, p->path, p->handler, p->arg);
    }

    // 初始化ice服务
    struct ice_config_services_t srv_configs;
    for(auto& ice_srv_conf : ice_srv_configs) {
        std::cerr<<ice_srv_conf.srv_id<<std::endl;
        srv_configs.services[ice_srv_conf.srv_id] = ice_srv_conf;
    }
    IceAsyncHttpRespHandler::init(srv_configs);

#ifndef EVHTP_DISABLE_EVTHR
    if(num_threads > 0) {
        struct app_info* app_p = new struct app_info;
        app_p->evbase = evbase;
        app_p->evhtp = evhtp;
        evhtp_use_threads_wexit(evhtp, gateway_init_thread, NULL, num_threads, app_p);
    }
#endif
    evhtp_bind_socket(evhtp, baddr, bport, backlog);
    event_base_loop(evbase, 0);

    return 0;
} /* main */

