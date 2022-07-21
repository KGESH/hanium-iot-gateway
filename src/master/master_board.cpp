//
// Created by 지성현 on 2022/04/02.
//
#include "master/master_board.h"

int MasterBoard::polling_count_ = 0;

MasterBoard::MasterBoard(const std::string& port_name, int baudrate)
        : serial_port_(std::make_unique<serial::Serial>(port_name, baudrate, serial::Timeout::simpleTimeout(20000))) {}

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
