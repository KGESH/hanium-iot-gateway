//
// Created by 지성현 on 2022/07/19.
//
#include "logger/packet_log.h"


PacketLog::PacketLog(char packet_type, std::string event_name, std::string packet)
        : packet_type_(packet_type), event_name_(std::move(event_name)), packet_(std::move(packet)) {}

const char* PacketLog::packet_type() const {
    return &packet_type_;
}

const std::string& PacketLog::packet() const {
    return packet_;
}

const std::string& PacketLog::event_name() const {
    return event_name_;
}


