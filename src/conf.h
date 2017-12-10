
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "ice_services.h"

using namespace NAMESPACE_GATEWAY::service;

class Config {
public:

    static Config& instance() { 
        static Config config;
        return config;
    }
    virtual ~Config() {}

    bool load(const std::string& cfg_file);

    int getThreads() const { return http_threads_; }
    std::string getHttpAddr() const { return http_addr_; }
    uint16_t getHttpPort() const { return http_port_; }
    int getHttpBacklog() const { return http_backlog_; }
    int getHttpNodelay() const { return http_nodelay_; }
    int getHttpDeferAccept() const { return http_defer_accept_; }
    int getHttpReusePort() const { return http_reuser_port_; }

    std::string getSrvId(const std::string& method) const {
        auto iter = method2srvid_.find(method);
        if(iter != method2srvid_.end())
            return iter->second;
        return "";
    }
    const struct std::vector<ice_config_service_t>& getIceSrvConfigs() const {
        return ice_srv_configs_;
    }
private:
    Config(){}
    Config(const Config& conf);
    void operator=(const Config& conf);

private:
    int http_threads_{2};
    const char* http_addr_{"127.0.0.1"};
    uint16_t http_port_{8089};
    int http_backlog_{1024};
    int http_nodelay_{0};
    int http_defer_accept_{0};
    int http_reuser_port_{0};

    std::unordered_map<std::string, std::string> method2srvid_;

    std::vector<struct ice_config_service_t> ice_srv_configs_;

};

