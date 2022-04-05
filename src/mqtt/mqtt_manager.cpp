//
// Created by 지성현 on 2022/03/30.
//

#include <iostream>
#include <vector>
#include "mqtt/mqtt_config.h"
#include "mqtt/mqtt_manager.h"
#include "mqtt/mqtt_topic.h"
#include "rapidjson/document.h"
#include "packet/request_packet.h"
#include "gateway/gateway_manager.h"

MQTTManager::MQTTManager(const char* id, const char* host, int port) : mosquittopp(id) {
    std::cout << "Call Constructor" << std::endl;
    mosqpp::lib_init();
    int keepalive = DEFAULT_KEEP_ALIVE;
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

void MQTTManager::on_subscribe(int mid, int qos_count, const int* granted_qos) {
    std::cout << "Subscription succeeded." << std::endl;

}

void MQTTManager::on_message(const struct mosquitto_message* message) {
    std::string topic(message->topic);
    std::string payload(static_cast<char*>(message->payload));

#ifdef DEBUG
    std::cout << "Receive Topic: " << topic << std::endl;
    std::cout << "Payload: " << payload << std::endl;
#endif
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
    /*  Input Sensor  */
    GetInstance().subscribe(nullptr, kTemperatureTopic.c_str());


    /*  Output Sensor */
    GetInstance().subscribe(nullptr, kLedTopic.c_str());
    GetInstance().subscribe(nullptr, kWaterPumpTopic.c_str());


    /*  IO Sensor */
    /*  TODO: Create After..  */


    /*  Master Only */
    GetInstance().subscribe(nullptr, kReadMasterMemoryTopic.c_str());
    GetInstance().subscribe(nullptr, kWriteMasterMemoryTopic.c_str());


    /*  Message Test  */
    GetInstance().subscribe(nullptr, kTestRawPacket);
    GetInstance().subscribe(nullptr, "test/topic");
}

void MQTTManager::PublishTopic(const std::string& topic, const std::string& payload) {
    publish(nullptr, topic.c_str(), payload.length(), payload.data());
}

std::vector<uint8_t> MQTTManager::MakePacket(const std::string& topic, const std::string& payload) {
    auto[packet, parse_fail] = ParseMqttMessage(topic, payload);
    if (parse_fail) {
        /*  TODO: After log mqtt message to database
         *  std::cout << "Parse Fail. Code: " << parse_fail << std::endl;
         * */
        return {};
    }

    return packet;
}

std::pair<std::vector<uint8_t>, EParseJsonErrorCode>
MQTTManager::ParseMqttMessage(const std::string& topic, const std::string& payload) {

    /*  TODO: Fix data_list memory crash & Cleanup Code */
    std::cout << "Try Parse Topic: " << topic << std::endl;
    std::cout << "Try Parse Payload: " << payload << std::endl;

    rapidjson::Document document;
    if (document.Parse(payload.c_str()).HasParseError()) {
        std::cout << "Payload Parse Error" << std::endl;

    }

    rapidjson::Value& parsedPayload = document["data"];
    rapidjson::Document document2;
    if (document2.Parse(parsedPayload.GetString()).HasParseError()) {
        std::cout << "Parse data Error" << std::endl;
    }

    std::cout << "data type: " << document2.GetType() << std::endl;
    if (document2.IsObject()) {
        std::cout << "Parsed doc2 Is Object!" << std::endl;
    } else {
        std::cout << "Parsed doc2 Is Not Object!" << std::endl;
    }
//
//    if (document.Parse(parsedPayload.GetString()).HasParseError()) {
//
//        return {{}, kFailParsePayload};
//    }
//
//    if (!document.IsObject()) {
//        return {{}, kFailIsNotObject};
//    }

    uint8_t start = document2["start"].GetUint();
    std::cout << "Start : " << start << std::endl;

    uint8_t index = document2["index"].GetUint();
    uint8_t target_id = document2["target_id"].GetUint();
    uint8_t command = document2["command"].GetUint();
    uint8_t data_length = document2["data_length"].GetUint();
    uint8_t address_high = document2["address_high"].GetUint();
    uint8_t address_low = document2["address_low"].GetUint();
    RequestHeader header{start, index, target_id, command, data_length};
//    if (!document2["data_list"].GetArray().Empty()) {
//        auto data_list = document2["data_list"].GetArray();
//        if (data_length > 2) {
//            std::vector<uint8_t> body_data;
//            for (const auto& datum: data_list) {
//                body_data.emplace_back(datum.GetUint());
//            }
//
//            PacketBody body{address_high, address_low, body_data};
//            return {RequestPacket{header, body}.Packet(), kSuccessParse};
//        }
//    }


    return {RequestPacket{header}.Packet(), kSuccessParse};
}

