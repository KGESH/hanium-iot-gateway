//
// Created by 지성현 on 2022/04/08.
//

#ifndef HANIUM_IOT_GATEWAY_UTIL_H
#define HANIUM_IOT_GATEWAY_UTIL_H

#include <iostream>
#include <sstream>
#include "packet/response_packet.h"


class Util {
public:
    static std::string PacketToString(ResponsePacket& packet) {
        std::stringstream ss;
        for (const auto& data: packet.Packet()) {
            ss << static_cast<char>(data);
        }

        auto message = ss.str();
        return message;
    }
};

#endif //HANIUM_IOT_GATEWAY_UTIL_H
