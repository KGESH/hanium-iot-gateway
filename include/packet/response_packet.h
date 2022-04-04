//
// Created by 지성현 on 2022/03/30.
//

#ifndef PLANT_GATEWAY_RESPONSE_PACKET_H
#define PLANT_GATEWAY_RESPONSE_PACKET_H
#include "base_packet.h"

class ResponsePacket : public BasePacket {
public:
    ResponsePacket() = default;

    ResponsePacket(ResponseHeader header, std::vector<uint8_t>& body, PacketTail tail);

    const std::vector<uint8_t>& Packet() override;

    uint8_t Checksum() override;

    bool ValidChecksum() override;

    const ResponseHeader& header();

    const PacketBody& body();

    const PacketTail& tail();

private:
    uint8_t CalculateChecksum();

    ResponseHeader header_{};

    PacketBody body_{};

    PacketTail tail_{};

};

#endif //PLANT_GATEWAY_RESPONSE_PACKET_H
