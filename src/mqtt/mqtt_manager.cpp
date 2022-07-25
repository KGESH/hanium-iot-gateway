//
// Created by 지성현 on 2022/03/30.
//

#include <iostream>
#include <utility>
#include <vector>
#include "mqtt/mqtt_manager.h"
#include "mqtt/mqtt_topic.h"
#include "rapidjson/document.h"
#include "packet/request_packet.h"
#include "logger/logger.h"
#include "util/util.h"

MQTTManager::MQTTManager(const char* id, const char* host, int port, int master_id,
                         Packet::RAW_PACKET_Q* mqtt_packet_queue,
                         std::mutex* g_mqtt_queue_mutex, std::condition_variable* g_cv)
        : mosquittopp(id),
          master_id_(master_id),
          packet_queue_(mqtt_packet_queue),
          packet_queue_mutex_(g_mqtt_queue_mutex), packet_queue_cv_(g_cv) {

    std::cout << "Call Constructor" << std::endl;
    mosqpp::lib_init();
    int keepalive = DEFAULT_KEEP_ALIVE;
    /** Todo: Extract after */
    std::string mock_payload = "hello, LWT";
    will_set("lwt/1", mock_payload.length(), mock_payload.c_str(), 1);
    connect(host, port, keepalive);
}

MQTTManager::~MQTTManager() {
    std::cout << "Call Destructor" << std::endl;
    mosqpp::lib_cleanup();
}

void MQTTManager::on_connect(int rc) {
    std::cout << "connect broker!" << std::endl;
    SubscribeTopics();

}


void MQTTManager::on_disconnect(int rc) {
    std::cout << "disconnect broker! - " << rc << std::endl;
    PublishTopic("dis/1", "disconnect");
}


void MQTTManager::on_subscribe(int mid, int qos_count, const int* granted_qos) {
#ifdef DEBUG
    std::cout << "Subscription succeeded." << std::endl;
#endif
}


void MQTTManager::on_message(const struct mosquitto_message* message) {
    std::string topic(message->topic);
    std::string payload(static_cast<char*>(message->payload));

#ifdef DEBUG
    std::cout << "Receive Topic: " << topic << std::endl;
    std::cout << "Receive Payload: " << payload << std::endl;
#endif
    auto packet = MakePacket(topic, payload);

#ifdef DEBUG
    std::cout << "Packet: ";
    for (const auto& p: packet) {
        std::cout << static_cast<int>(p) << " ";
    }
    std::cout << std::endl;
#endif

    {
        std::unique_lock<std::mutex> lock(*packet_queue_mutex_);
        packet_queue_->push(packet);
    }
    packet_queue_cv_->notify_one();
    {
        PacketLog log("MQTT_TO_GATEWAY", "RECEIVE_MSG", Util::RawPacketToString(packet));
        Logger::CreateLog(log);
        Logger::ReadLog();
    }
}

void MQTTManager::Reconnect() {
    reconnect();
}

bool MQTTManager::IsConnected() {
    const auto return_code = loop();

    if (return_code == MOSQ_ERR_SUCCESS) {
        return true;
    }

    return false;
}

void MQTTManager::SubscribeTopics() {
    /*  Input Sensor  */
    subscribe(nullptr, MqttTopic::TemperatureTopic(master_id_).c_str());

    /*  Output Sensor */
    subscribe(nullptr, MqttTopic::WaterPumpTopic(master_id_).c_str());
    subscribe(nullptr, MqttTopic::LedTopic(master_id_).c_str());
    subscribe(nullptr, MqttTopic::FanTopic(master_id_).c_str());


    /*  IO Sensor */
    /*  TODO: Create After..  */


    /*  Master Only */
    subscribe(nullptr, MqttTopic::ReadMasterMemoryTopic(master_id_).c_str());
    subscribe(nullptr, MqttTopic::WriteMasterMemoryTopic(master_id_).c_str());


    /*  Message Test  */
    subscribe(nullptr, MqttTopic::kTestRawPacket);
    subscribe(nullptr, "test/topic");
}

void MQTTManager::PublishTopic(const std::string& topic, const std::string& payload) {
#ifdef DEBUG
    std::cout << "Publish Topic: " << topic << std::endl;
#endif
    PacketLog log("GATEWAY_TO_MQTT", topic, payload);
    Logger::CreateLog(log);
    publish(nullptr, topic.c_str(), payload.length(), payload.data());
}

std::vector<uint8_t> MQTTManager::MakePacket(const std::string& topic, const std::string& payload) {
    auto[packet, parse_fail] = ParseMqttMessage(topic, payload);
    if (parse_fail) {
        /*  TODO: After log mqtt message to database
         * */
#ifdef DEBUG
        std::cout << "Parse Fail. Code: " << parse_fail << std::endl;
#endif
        return {};
    }

    return packet;
}

std::pair<std::vector<uint8_t>, EParseJsonErrorCode>
MQTTManager::ParseMqttMessage(const std::string& topic, const std::string& payload) {
    /* Receive Payload Format Example
     * {"pattern":"master/1/temperature","data":"{\"start\":35,\"index\":33,\"target_id\":17,\"command\":193,\"data_length\":2,\"address_high\":7,\"address_low\":208,\"data_list\":[1,2]}"}
     * raw => formatted
     * {
        "pattern" : "master/1/temperature",
        "data": {
                    "start":35,
                    "index":33,
                    "target_id":17,
                    "command":193,
                    "data_length":2,
                    "address_high":7,
                    "address_low":208,
                    "data_list":[1, 2]
                }
       }
     */

    rapidjson::Document document;
    if (document.Parse(payload.c_str()).HasParseError() || !document.HasMember("data")) {
        return {{}, kFailParsePayload};
    }

    rapidjson::Value& parsed_payload = document["data"];
    if (document.Parse(parsed_payload.GetString()).HasParseError()) {
        return {{}, kFailParsePayload};
    }

    if (!document.IsObject()) {
        return {{}, kFailIsNotObject};
    }


    uint8_t start = document["start"].GetUint();
    uint8_t index = document["index"].GetUint();
    uint8_t target_id = document["target_id"].GetUint();
    uint8_t command = document["command"].GetUint();
    uint8_t data_length = document["data_length"].GetUint();
    uint8_t address_high = document["address_high"].GetUint();
    uint8_t address_low = document["address_low"].GetUint();
    RequestHeader header{start, index, target_id, command, data_length};

    if (data_length > 0) {
        std::vector<uint8_t> body_data;
        if (document.HasMember("data_list")) {
            for (const auto& datum: document["data_list"].GetArray()) {
                body_data.emplace_back(datum.GetUint());
            }
        }

        PacketBody body{address_high, address_low, body_data};
        return {RequestPacket{header, body}.Packet(), kSuccessParse};
    }

    return {RequestPacket{header}.Packet(), kSuccessParse};
}

