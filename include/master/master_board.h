//
// Created by 지성현 on 2022/03/30.
//

#ifndef PLANT_GATEWAY_MASTER_MANAGER_H
#define PLANT_GATEWAY_MASTER_MANAGER_H

#include "serial/serial.h"
#include <memory>


class MasterBoard {
public:
    MasterBoard() = delete;

    MasterBoard(const MasterBoard&) = delete;

    const MasterBoard& operator=(const MasterBoard&) = delete;

    MasterBoard(const std::string& port_name, int baudrate);

    serial::Serial& serial_port() const;

    static void ResetPollingCount();

    static void IncreasePollingCount();

    static int polling_count();

private:
    std::shared_ptr<serial::Serial> serial_port_;
    static int polling_count_;
};

#endif //PLANT_GATEWAY_MASTER_MANAGER_H
