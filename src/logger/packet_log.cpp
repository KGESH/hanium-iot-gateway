//
// Created by 지성현 on 2022/07/19.
//
#include "logger/packet_log.h"


PacketLog::PacketLog(std::string direction, std::string event_name, std::string packet)
        : direction_(std::move(direction)), event_name_(std::move(event_name)), packet_(std::move(packet)) {}

const std::string& PacketLog::direction() const {
    return direction_;
}

const std::string& PacketLog::packet() const {
    return packet_;
}

const std::string& PacketLog::event_name() const {
    return event_name_;
}


