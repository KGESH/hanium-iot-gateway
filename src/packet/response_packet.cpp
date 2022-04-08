//
// Created by 지성현 on 2022/04/02.
//
#include "packet/response_packet.h"

ResponsePacket::ResponsePacket(const ResponseHeader& header, const std::vector<uint8_t>& body, const PacketTail& tail)
        : header_(header), body_(),
          tail_(tail) {

    if (!body.empty()) {
        body_.high_address = body[0];
        body_.low_address = body[1];
        body_.data.assign(body.begin() + 2, body.end());
    }
}


const std::vector<uint8_t>& ResponsePacket::Packet() {
    /* TODO: Refactor  */
    if (packet_.empty()) {
        packet_.emplace_back(header_.start);
        packet_.emplace_back(header_.idx);
        packet_.emplace_back(header_.target_id);
        packet_.emplace_back(header_.command);
        packet_.emplace_back(header_.error_code);
        packet_.emplace_back(header_.data_length);

        if (header_.data_length > 0) {
            packet_.emplace_back(body_.high_address);
            packet_.emplace_back(body_.low_address);
            for (const auto& data: body_.data) {
                packet_.emplace_back(data);
            }
        }

        packet_.emplace_back(tail_.checksum);
        packet_.emplace_back(tail_.end);
    }

    return packet_;
}

uint8_t ResponsePacket::Checksum() {
    return CalculateChecksum();
}

bool ResponsePacket::ValidChecksum() {
    if (Checksum() == this->tail_.checksum) {
        return true;
    }

    return false;
}

uint8_t ResponsePacket::CalculateChecksum() {
    /* TODO: Refactor  */
    uint8_t checksum = 0;
    checksum += header_.start;
    checksum += header_.idx;
    checksum += header_.target_id;
    checksum += header_.command;
    checksum += header_.error_code;
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

const ResponseHeader& ResponsePacket::header() {
    return header_;
}

const PacketTail& ResponsePacket::tail() {
    return tail_;
}

const PacketBody& ResponsePacket::body() {
    return body_;
}
