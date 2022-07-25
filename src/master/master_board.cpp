//
// Created by 지성현 on 2022/04/02.
//
#include "master/master_board.h"
#include "packet/request_packet.h"

int MasterBoard::polling_count_ = 0;

MasterBoard::MasterBoard(const std::string& port_name, int baudrate)
        : serial_port_(std::make_unique<serial::Serial>(port_name, baudrate, serial::Timeout::simpleTimeout(20000))),
          master_id_(0) {}

serial::Serial& MasterBoard::serial_port() const {
    return *serial_port_;
}

void MasterBoard::ResetPollingCount() {
    polling_count_ = 0;
}

void MasterBoard::IncreasePollingCount() {
    polling_count_++;
}

int MasterBoard::polling_count() {
    return polling_count_;
}

int MasterBoard::master_id() const {
    return master_id_;
}

void MasterBoard::SetMasterId(int master_id) {
    master_id_ = master_id;
}

//void MasterBoard::SetupMasterId() const {
//    RequestHeader header{0x23, 0x27, 0xff, 0xc1, 2};
//    PacketBody body{0x1f, 0xa5};
//    RequestPacket master_id_request_packet(header, body);
//
//    serial_port_->write(master_id_request_packet.Packet());
//}
