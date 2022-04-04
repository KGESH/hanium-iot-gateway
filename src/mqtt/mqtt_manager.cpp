//
// Created by 지성현 on 2022/03/30.
//

#include <iostream>
#include <cstring>
#include <vector>
#include "mqtt/mqtt_config.h"
#include "mqtt/mqtt_manager.h"
#include "rapidjson/document.h"
#include "packet/request_packet.h"
#include "gateway/gateway_manager.h"

MQTTManager::MQTTManager(const char* id, const char* host, int port) : mosquittopp(id) {
    mosqpp::lib_init();
    int keepalive = DEFAULT_KEEP_ALIVE;
    connect(host, port, keepalive);
}

MQTTManager::~MQTTManager() {
    mosqpp::lib_cleanup();
}

void MQTTManager::on_connect(int rc) {
    std::cout << "connect broker!" << std::endl;
    SubscribeTopics();

}

void MQTTManager::on_subscribe(int mid, int qos_count, const int* granted_qos) {
    std::cout << "Subscription succeeded." << std::endl;

}

void MQTTManager::on_message(const struct mosquitto_message* message) {
    std::string topic(message->topic);
    std::string payload(static_cast<char*>(message->payload));

    auto packet = MakePacket(topic, payload);

    /* Write master board*/
    GatewayManager::master_board().serial_port().write(packet);
}

MQTTManager& MQTTManager::GetInstance() {
    static MQTTManager mqtt_manager(CLIENT_ID, HOST, MQTT_PORT);
    return mqtt_manager;
}

void MQTTManager::Reconnect() {
    GetInstance().reconnect();
}

bool MQTTManager::IsConnected() {
    const auto return_code = GetInstance().loop();

    if (return_code == MOSQ_ERR_SUCCESS) {
        return true;
    }

    return false;
}

void MQTTManager::SubscribeTopics() {
    GetInstance().subscribe(nullptr, "test/topic");
    GetInstance().subscribe(nullptr, "master/+/led");

}

void MQTTManager::PublishTopic(const std::string& topic, const std::string& payload) {
    publish(nullptr, topic.c_str(), payload.length(), payload.data());
}

std::vector<uint8_t> MQTTManager::MakePacket(const std::string& topic, const std::string& payload) {
    return ParseMqttMessage(topic, payload);
}

std::vector<uint8_t> MQTTManager::ParseMqttMessage(const std::string& topic, const std::string& payload) {

    /* TODO: Change Return Value to std::pair< psacket, success or fail >*/
    rapidjson::Document document;
    if (document.Parse(payload.c_str()).HasParseError() ||
        document.Parse(document["data"].GetString()).HasParseError()) {

        std::cout << "Json Parse Error!" << std::endl;
        return {};
    }

    if (!document.IsObject()) {
        return {};
    }

    uint8_t start = document["start"].GetUint();
    uint8_t index = document["index"].GetUint();
    uint8_t target_id = document["target_id"].GetUint();
    uint8_t command = document["command"].GetUint();
    uint8_t data_length = document["data_length"].GetUint();
    uint8_t address_high = document["address_high"].GetUint();
    uint8_t address_low = document["address_low"].GetUint();
    auto data_list = document["data_list"].GetArray();

    RequestHeader header{start, index, target_id, command, data_length};
    if (data_length > 0) {
        std::vector<uint8_t> body_data;
        for (const auto& datum: data_list) {
            body_data.emplace_back(datum.GetUint());
        }

        PacketBody body{address_high, address_low, body_data};
        return RequestPacket{header, body}.Packet();
    }

    return RequestPacket{header}.Packet();
}

