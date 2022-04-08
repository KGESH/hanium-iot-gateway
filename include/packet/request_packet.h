//
// Created by 지성현 on 2022/03/30.
//

#ifndef PLANT_GATEWAY_REQUEST_PACKET_H
#define PLANT_GATEWAY_REQUEST_PACKET_H
#include "base_packet.h"

class RequestPacket : public BasePacket {

public:
    RequestPacket() = default;

    explicit RequestPacket(const RequestHeader& header);

    RequestPacket(const RequestHeader& header,const PacketBody& body);

    const std::vector<uint8_t>& Packet() override;

    uint8_t Checksum() override;

    bool ValidChecksum() override;

    RequestHeader& header();

    PacketBody& body();

    PacketTail& tail();

private:
    uint8_t CalculateChecksum();

    RequestHeader header_;

    PacketBody body_;

    PacketTail tail_;

    uint8_t CalculateChecksum(const RequestHeader& header);

    uint8_t CalculateChecksum(const RequestHeader& header, const PacketBody& body);
};


#endif //PLANT_GATEWAY_REQUEST_PACKET_H
