//
// Created by 지성현 on 2022/03/30.
//

#ifndef PLANT_GATEWAY_BASE_PACKET_H
#define PLANT_GATEWAY_BASE_PACKET_H

#include "protocol/protocol.h"
#include <vector>

#pragma pack(push, 1)
typedef struct RequestHeader {
    const uint8_t start = kStart;
    uint8_t idx{};
    uint8_t target_id{};
    uint8_t command{};
    uint8_t data_length{};
} RequestHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct ResponseHeader {
    const uint8_t start = kStart;
    uint8_t idx{};
    uint8_t target_id{};
    uint8_t command{};
    uint8_t error_code{};
    uint8_t data_length{};
} ResponseHeader;
#pragma pack(pop)

typedef struct PacketBody {
    uint8_t high_address{};
    uint8_t low_address{};
    std::vector<uint8_t> data{};
} PacketBody;

#pragma pack(push, 1)
typedef struct PacketTail {
    uint8_t checksum{};
    uint8_t end = kEnd;
} PacketTail;
#pragma pack(pop)


class BasePacket {
public:
    virtual const std::vector<uint8_t>& Packet() = 0;

    virtual uint8_t Checksum() = 0;

    virtual bool ValidChecksum() = 0;

    virtual ~BasePacket() = default;

protected:
    std::vector<uint8_t> packet_;

};

#endif //PLANT_GATEWAY_BASE_PACKET_H
