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

const std::array<uint8_t, kMaxSlaveCount>& MasterBoard::slave_ids() const {
    return slave_ids_;
}

void MasterBoard::SetSlaveIds(const std::array<uint8_t, kMaxSlaveCount>& slaveIds) {
    /** Todo: Check Shallow Copy */
    slave_ids_ = slaveIds;
}

void MasterBoard::SetSlaveCount(const uint8_t slave_count) {
    slave_count_ = slave_count;
}

uint8_t MasterBoard::slave_count() const {
    return slave_count_;
}

