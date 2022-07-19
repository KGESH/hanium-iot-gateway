//
// Created by 지성현 on 2022/07/18.
//

#ifndef HANIUM_IOT_GATEWAY_PACKET_LOG_H
#define HANIUM_IOT_GATEWAY_PACKET_LOG_H

#include <string>

class PacketLog {

public:
    PacketLog() = default;

    PacketLog(char packet_type, std::string event_name, std::string packet);

    const char* packet_type() const;

    const std::string& packet() const;

    const std::string& event_name() const;

private:
    int id_;
    char packet_type_;
    std::string packet_;
    std::string event_name_;
    std::string created_at_;
};

#endif //HANIUM_IOT_GATEWAY_PACKET_LOG_H
