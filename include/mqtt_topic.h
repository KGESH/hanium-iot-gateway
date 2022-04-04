//
// Created by 지성현 on 2022/03/30.
//

#ifndef PLANT_GATEWAY_MQTT_TOPIC_H
#define PLANT_GATEWAY_MQTT_TOPIC_H
#include <iostream>

/* TODO: Input gateway config file after change protocol
 * Ex) gatewayId/sensorName
 * */

constexpr auto kMasterTopic = "master/";
/* If Add New Gateway
 * Then Update Gateway ID  */
const std::string kMasterId = "1";

const auto kAssertTopic = kMasterTopic + kMasterId;
const auto kTemperatureTopic = kMasterTopic + kMasterId + "/temperature";
const auto kWaterPumpTopic = kMasterTopic + kMasterId + "/water";
const auto kLedTopic = kMasterTopic + kMasterId + "/led";


const auto kErrorTopic = kMasterTopic + kMasterId + "/error";
const auto kPollingTopic = kMasterTopic + kMasterId + "/polling";
const auto kPollingErrorTopic = kMasterTopic + kMasterId + "/polling/error";
const auto kReadMasterMemoryTopic = kMasterTopic + kMasterId + "/read";
const auto kWriteMasterMemoryTopic = kMasterTopic + kMasterId + "/write";
const auto kTestRawPacket = "test-raw-packet";
const auto kTestResponsePacket = "test-res-packet";


constexpr auto kTestPollingOKMessage = "Polling OK";
constexpr auto kTestPollingErrorMessage = "Polling Error";

#endif //PLANT_GATEWAY_MQTT_TOPIC_H
