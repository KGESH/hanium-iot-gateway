//
// Created by 지성현 on 2022/04/04.
//

#ifndef PLANT_GATEWAY_MASTER_CONFIG_H
#define PLANT_GATEWAY_MASTER_CONFIG_H

namespace Serial{
    std::vector <std::string> PRE_SERIAL_PORT;  
    int cnt=0;    
    void findSerialPort(void);
    void changeSerialPort(void);
}

//constexpr auto SERIAL_PORT = "/dev/tty.usbserial-0001";
//constexpr auto SERIAL_PORT = "/dev/tty.usbserial-A50285BI";
//constexpr auto SERIAL_PORT = "/dev/ttyS2";
//constexpr auto SERIAL_PORT = "/dev/ttyUSB0";
std::string SERIAL_PORT;
constexpr auto BAUDRATE = 115200;

#endif //PLANT_GATEWAY_MASTER_CONFIG_H
