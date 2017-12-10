
#include "conf.h"

bool Config::load(const std::string& cfg_file) {

    // TODO: read from cfg_file


    // ICE服务配置
    ice_srv_configs_ = std::vector<struct ice_config_service_t>{
        {true, "ice-async-service-001", "SrvPrx", "ice-async-server", "ice.admin.conf", 1000},
        {true, "ice-async-service-002", "SrvPrx", "ice-async-server", "ice.admin.conf", 1000},
    };

    // method -> srv_id
    method2srvid_ = std::unordered_map<std::string, std::string>{{"srv_id1", "ice-async-service-002"}};

    return true;
}

