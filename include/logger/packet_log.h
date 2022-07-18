//
// Created by 지성현 on 2022/07/18.
//

#ifndef HANIUM_IOT_GATEWAY_PACKET_LOG_H
#define HANIUM_IOT_GATEWAY_PACKET_LOG_H
#include <string>
#include <array>
typedef struct PacketLog{
    int id{};
    char packet_type{};
    std::string packet{};
    std::string event_name{};
    std::string created_at{};

} PacketLog;

#endif //HANIUM_IOT_GATEWAY_PACKET_LOG_H
