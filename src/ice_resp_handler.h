
#pragma once

#include "evhtp/evhtp.h"

#include "timer.h"
#include "ice_rqst.h"
#include "ice_services.h"

using namespace NAMESPACE_GATEWAY::rqst;
using namespace NAMESPACE_GATEWAY::service;

class IceAsyncHttpRespHandler : public IceAsyncRqstBase, utils::MarsWalking {
public:
    IceAsyncHttpRespHandler(const std::string& rqstid, 
                            evhtp_request_t* req,
                            bool async = true);

    IceAsyncHttpRespHandler(const std::string& rqstid, 
                            const std::string& srv_id,
                            const std::string& method,
                            const std::string& rqst,
                            evhtp_request_t* req,
                            bool async=true);

    virtual ~IceAsyncHttpRespHandler();

    static int init(struct ice_config_services_t& srv_configs);

    static void Call(const std::string& rqstid,
                     const std::string& srv_id,
                     const std::string& method,
                     const std::string& rqst,
                     const bool async,
                     evhtp_request_t* r);

    static bool ProcessResult(evhtp_request_t* r, bool succ, const std::string& resp);
    virtual bool ProcessResult(bool succ, const std::string& resp) override;

private:
    static void httpResponseCb(evthr_t* thr, void* arg, void* shared);

private:
    evhtp_request_t* req_{nullptr};
};

