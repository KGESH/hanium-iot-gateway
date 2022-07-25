//
// Created by 지성현 on 2022/03/30.
//

#ifndef PLANT_GATEWAY_MASTER_MANAGER_H
#define PLANT_GATEWAY_MASTER_MANAGER_H

#include "serial/serial.h"
#include <array>
#include <memory>

constexpr auto kMaxSlaveCount = 127;

class MasterBoard {
public:
    MasterBoard() = delete;

    MasterBoard(const MasterBoard&) = delete;

    const MasterBoard& operator=(const MasterBoard&) = delete;

    MasterBoard(const std::string& port_name, int baudrate);

    int master_id() const;

    void SetMasterId(int master_id);

    const std::array<uint8_t, kMaxSlaveCount>& slave_ids() const;

    void SetSlaveIds(const std::array<uint8_t, kMaxSlaveCount>& slaveIds);

    serial::Serial& serial_port() const;

    static void ResetPollingCount();

    static void IncreasePollingCount();

    static int polling_count();


    void SetSlaveCount(const uint8_t slave_count);

    uint8_t slave_count() const;

private:


    std::unique_ptr<serial::Serial> serial_port_;
    static int polling_count_;
    int master_id_;
    int slave_count_;
    std::array<uint8_t, kMaxSlaveCount> slave_ids_;
};

#endif //PLANT_GATEWAY_MASTER_MANAGER_H
