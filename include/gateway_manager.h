//
// Created by 지성현 on 2022/03/30.
//

#ifndef PLANT_GATEWAY_GATEWAY_MANAGER_H
#define PLANT_GATEWAY_GATEWAY_MANAGER_H

#include "protocol.h"
#include "mqtt_manager.h"
#include "response_packet.h"
#include "request_packet.h"
#include "master_board.h"


class GatewayManager {

public:

    GatewayManager() = delete;

    GatewayManager(const GatewayManager&) = delete;

    const GatewayManager& operator=(const GatewayManager&) = delete;

    static const GatewayManager& GetInstance();

    static const MasterBoard& master_board();

    bool ListeningMaster(MQTTManager& mqtt_manager) const;

    void ParseCommand(ResponsePacket& packet, MQTTManager& mqtt_manager) const;

    void Polling(MQTTManager& mqtt_manager) const;


private:

    GatewayManager(const std::string& serial_port_name, int baudrate);

    std::pair<ResponsePacket, EReceiveStatusCode> ReceivePacket() const;

    void PublishError(MQTTManager& mqtt_manager, const std::string& topic, const std::string& message) const;

    void PublishTemperature(ResponsePacket& packet, MQTTManager& mqtt_manager) const;

    void ParseMemoryRead(ResponsePacket& packet, MQTTManager& mqtt_manager, uint16_t memory_address) const;

    void PublishTestPacket(ResponsePacket& packet, MQTTManager& mqtt_manager) const;

    std::string PacketToString(ResponsePacket& packet) const;

    void PublishPollingSuccess(MQTTManager& mqtt_manager) const;

    std::string GetSlaveStateTopic(const std::string& slave_id, const std::string& sensor_name) const;

    void PublishMotorTopic(ResponsePacket& packet, MQTTManager& mqtt_manager) const;

    void PublishLedTopic(ResponsePacket& packet, MQTTManager& mqtt_manager) const;

    MasterBoard master_board_;
};

#endif //PLANT_GATEWAY_GATEWAY_MANAGER_H
