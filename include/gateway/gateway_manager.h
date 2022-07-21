//
// Created by 지성현 on 2022/03/30.
//

#ifndef PLANT_GATEWAY_GATEWAY_MANAGER_H
#define PLANT_GATEWAY_GATEWAY_MANAGER_H

#include <memory>
#include <queue>
#include "protocol/protocol.h"
#include "mqtt/mqtt_manager.h"
#include "packet/response_packet.h"
#include "packet/request_packet.h"
#include "master/master_board.h"


class GatewayManager {

public:

    GatewayManager() = delete;

    GatewayManager(const GatewayManager&) = delete;

    const GatewayManager& operator=(const GatewayManager&) = delete;

    GatewayManager(const std::string& serial_port_name, int baudrate,
                   Packet::RAW_PACKET_Q* packet_queue, std::mutex* packet_queue_mutex,
                   std::condition_variable* packet_queue_cv);

    bool ListeningMaster(MQTTManager& mqtt_manager) const;

    void ParseCommand(ResponsePacket& packet, MQTTManager& mqtt_manager) const;

    void Polling(MQTTManager& mqtt_manager) const;

    void RequestTemperature() const;

    /** Todo: Run Worker Thread */
    void WritePacket() const;


private:


    std::pair<ResponsePacket, EReceiveErrorCode> ReceivePacket() const;

    void PublishError(MQTTManager& mqtt_manager, const std::string& topic, const std::string& message) const;

    void PublishTemperature(ResponsePacket& packet, MQTTManager& mqtt_manager) const;

    void ParseMemoryRead(ResponsePacket& packet, MQTTManager& mqtt_manager, uint16_t memory_address) const;

    void ParseMemoryWrite(ResponsePacket& packet, MQTTManager& mqtt_manager, uint16_t memory_address) const;

    void ParseEmergency(ResponsePacket& packet, MQTTManager& mqtt_manager, uint16_t memory_address) const;

    void PublishTestPacket(ResponsePacket& packet, MQTTManager& mqtt_manager) const;

    void PublishPollingSuccess(MQTTManager& mqtt_manager) const;

    void PublishSensorStateTopic(ResponsePacket& packet, MQTTManager& mqtt_manager, std::string&& sensor_name) const;

    void PublishMotorTopic(ResponsePacket& packet, MQTTManager& mqtt_manager) const;

    void PublishLedTopic(ResponsePacket& packet, MQTTManager& mqtt_manager) const;

    void PublishFanTopic(ResponsePacket& packet, MQTTManager& mqtt_manager) const;

    std::string GetSlaveStateTopic(uint8_t slave_id, const std::string& sensor_name) const;

    Packet::RAW_PACKET_Q* packet_queue_;
    std::mutex* packet_queue_mutex_;
    std::condition_variable* packet_queue_cv_;
    MasterBoard master_board_;

};

#endif //PLANT_GATEWAY_GATEWAY_MANAGER_H
