//
// Created by 지성현 on 2022/03/30.
//

#ifndef PLANT_GATEWAY_MQTT_MANAGER_H
#define PLANT_GATEWAY_MQTT_MANAGER_H

#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "mosquittopp.h"
#include "protocol/protocol.h"

#define DEFAULT_KEEP_ALIVE 10


class MQTTManager : public mosqpp::mosquittopp {
public:

    MQTTManager() = delete;

    MQTTManager(const char* id, const char* host, int port, Packet::RAW_PACKET_Q* mqtt_packet_queue,
                std::mutex* g_mqtt_queue_mutex,
                std::condition_variable* g_cv);

    MQTTManager(const MQTTManager&) = delete;

    MQTTManager& operator=(const MQTTManager&) = delete;

    ~MQTTManager() override;

    void Reconnect();

    bool IsConnected();

    void PublishTopic(const std::string& topic, const std::string& payload);

    Packet::RAW_PACKET_Q* raw_packet_queue;
    std::mutex* g_mqtt_queue_mutex;
    std::condition_variable* g_cv;

private:

    void SubscribeTopics();

    void on_connect(int rc) override;

    void on_disconnect(int rc) override;

    /* Callback when mqtt message arrived */
    void on_message(const struct mosquitto_message* message) override;

    void on_subscribe(int mid, int qos_count, const int* granted_qos) override;

    std::vector<uint8_t> MakePacket(const std::string& topic, const std::string& payload);

    std::pair<std::vector<uint8_t>, EParseJsonErrorCode>
    ParseMqttMessage(const std::string& topic, const std::string& payload);

};

#endif //PLANT_GATEWAY_MQTT_MANAGER_H
