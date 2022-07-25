//
// Created by 지성현 on 2022/03/30.
//

#ifndef PLANT_GATEWAY_MQTT_TOPIC_H
#define PLANT_GATEWAY_MQTT_TOPIC_H

#include <iostream>
#include <string>

/* TODO: Input gateway config file after change protocol
 * Ex) gatewayId/sensorName
 * */

namespace MqttTopic {
    constexpr auto kMasterTopic = "master/";
/* If Add New Gateway
 * Then Update Gateway ID  */
    const std::string kMasterId = "1";

    const auto kAssertTopic = kMasterTopic + kMasterId + "/assert";
    const auto kTemperatureTopic = kMasterTopic + kMasterId + "/temperature";
    const auto kWaterPumpTopic = kMasterTopic + kMasterId + "/water";
    const auto kLedTopic = kMasterTopic + kMasterId + "/led";
    const auto kFanTopic = kMasterTopic + kMasterId + "/fan";

    std::string TemperatureTopic(int master_id);

    std::string WaterPumpTopic(int master_id);

    std::string LedTopic(int master_id);

    std::string FanTopic(int master_id);

    std::string ReadMasterMemoryTopic(int master_id);

    std::string WriteMasterMemoryTopic(int master_id);

    std::string PollingTopic(int master_id);

    std::string ErrorTopic(int master_id);

    std::string PollingErrorTopic(int master_id);

    std::string LastWillTopic(int master_id);


    const auto kErrorTopic = kMasterTopic + kMasterId + "/error";
    const auto kPollingTopic = kMasterTopic + kMasterId + "/polling";
    const auto kPollingErrorTopic = kMasterTopic + kMasterId + "/polling/error";
    const auto kReadMasterMemoryTopic = kMasterTopic + kMasterId + "/read";
    const auto kWriteMasterMemoryTopic = kMasterTopic + kMasterId + "/write";
    const auto kTestRawPacket = "test-raw-packet";
    const auto kTestResponsePacket = "test-res-packet";

    const auto kLastWillTopic = "disconnect";

    constexpr auto kTestPollingOKMessage = "Polling OK";
    constexpr auto kTestPollingErrorMessage = "Polling Error";

}


#endif //PLANT_GATEWAY_MQTT_TOPIC_H
