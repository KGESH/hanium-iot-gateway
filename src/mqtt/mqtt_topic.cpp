//
// Created by 지성현 on 2022/07/25.
//

#include "mqtt/mqtt_topic.h"

namespace MqttTopic {
    std::string TemperatureTopic(int master_id) {
        return kMasterTopic + std::to_string(master_id) + "/temperature";
    }

    std::string WaterPumpTopic(int master_id) {
        return kMasterTopic + std::to_string(master_id) + "/water";
    }

    std::string LedTopic(int master_id) {
        return kMasterTopic + std::to_string(master_id) + "/led";
    }

    std::string FanTopic(int master_id) {
        return kMasterTopic + std::to_string(master_id) + "/fan";
    }

    std::string ReadMasterMemoryTopic(int master_id) {
        return kMasterTopic + std::to_string(master_id) + "/read";
    }

    std::string WriteMasterMemoryTopic(int master_id) {
        return kMasterTopic + std::to_string(master_id) + "/write";
    }

    std::string PollingTopic(int master_id) {
        return kMasterTopic + std::to_string(master_id) + "/polling";
    }

    std::string ErrorTopic(int master_id) {
        return kMasterTopic + std::to_string(master_id) + "/error";
    }

    std::string PollingErrorTopic(int master_id) {
        return kMasterTopic + std::to_string(master_id) + "/polling/error";
    }

    std::string LastWillTopic(int master_id) {
        return "disconnect";
    }



}