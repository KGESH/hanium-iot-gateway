//
// Created by 지성현 on 2022/03/30.
//

#ifndef PLANT_GATEWAY_MQTT_MANAGER_H
#define PLANT_GATEWAY_MQTT_MANAGER_H

#include <iostream>
#include <vector>
#include "mosquittopp.h"
#include "protocol/protocol.h"

#define DEFAULT_KEEP_ALIVE 60


class MQTTManager : public mosqpp::mosquittopp {
public:

    MQTTManager() = delete;

    MQTTManager(const MQTTManager&) = delete;

    MQTTManager& operator=(const MQTTManager&) = delete;

    static MQTTManager& GetInstance();

    ~MQTTManager() override;

    static void Reconnect();

    static bool IsConnected();

    void PublishTopic(const std::string& topic, const std::string& payload);

private:
    MQTTManager(const char* id, const char* host, int port);

    static void SubscribeTopics();

    void on_connect(int rc) override;

    /* Callback when mqtt message arrived */
    void on_message(const struct mosquitto_message* message) override;

    void on_subscribe(int mid, int qos_count, const int* granted_qos) override;

    std::vector<uint8_t> MakePacket(const std::string& topic, const std::string& payload);

    std::pair<std::vector<uint8_t>, EParseJsonErrorCode> ParseMqttMessage(const std::string& topic, const std::string& payload);
};

#endif //PLANT_GATEWAY_MQTT_MANAGER_H
