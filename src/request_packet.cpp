//
// Created by 지성현 on 2022/04/02.
//

#include "request_packet.h"

RequestPacket::RequestPacket(RequestHeader& header, PacketBody& body) :
        header_(header), body_(body), tail_(PacketTail{CalculateChecksum(header, body), kEnd}) {
    /*  TODO: assert checksum  */
}

uint8_t RequestPacket::CalculateChecksum(RequestHeader& header, PacketBody body) {
    /* TODO: Refactor  */
    uint8_t checksum = 0;
    checksum += header.start;
    checksum += header.idx;
    checksum += header.target_id;
    checksum += header.command;
    checksum += header.data_length;

    if (header.data_length > 0) {
        checksum += body.high_address;
        checksum += body.low_address;
        for (const auto& packet: body.data) {
            checksum += packet;
        }
    }

    return checksum;
}

uint8_t RequestPacket::CalculateChecksum(RequestHeader& header) {
    /* TODO: Refactor  */
    uint8_t checksum = 0;
    checksum += header.start;
    checksum += header.idx;
    checksum += header.target_id;
    checksum += header.command;
    checksum += header.data_length;

    return checksum;
}

uint8_t RequestPacket::Checksum() {
    return CalculateChecksum();
}

bool RequestPacket::ValidChecksum() {
    if (Checksum() != this->tail_.checksum) {
        return false;
    }

    return true;
}

const std::vector<uint8_t>& RequestPacket::Packet() {
    /* TODO: Refactor  */
    if (packet_.empty()) {
        packet_.emplace_back(header_.start);
        packet_.emplace_back(header_.idx);
        packet_.emplace_back(header_.target_id);
        packet_.emplace_back(header_.command);
        packet_.emplace_back(header_.data_length);

        if (header_.data_length > 0) {
            packet_.emplace_back(body_.high_address);
            packet_.emplace_back(body_.low_address);
            for (const auto& i: body_.data) {
                packet_.emplace_back(i);
            }
        }
        packet_.emplace_back(CalculateChecksum(header_, body_));
        packet_.emplace_back(kEnd);
    }
    return packet_;
}

RequestPacket::RequestPacket(RequestHeader header) : header_(header),
                                                     tail_(PacketTail{CalculateChecksum(header), kEnd}) {}

RequestHeader& RequestPacket::header() {
    return header_;
}

PacketBody& RequestPacket::body() {
    return body_;
}

PacketTail& RequestPacket::tail() {
    return tail_;
}

uint8_t RequestPacket::CalculateChecksum() {
    /* TODO: Refactor  */
    uint8_t checksum = 0;
    checksum += header_.start;
    checksum += header_.idx;
    checksum += header_.target_id;
    checksum += header_.command;
    checksum += header_.data_length;

    if (header_.data_length > 0) {
        checksum += this->body_.high_address;
        checksum += this->body_.low_address;
        for (const auto& packet: body_.data) {
            checksum += packet;
        }
    }

    return checksum;
}
