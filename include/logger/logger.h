//
// Created by 지성현 on 2022/07/18.
//

#ifndef HANIUM_IOT_GATEWAY_LOGGER_H
#define HANIUM_IOT_GATEWAY_LOGGER_H

#include "database/database_config.h"
#include "packet_log.h"


class Logger {
public:
    Logger() = delete;

    static void Init(const std::string& database_name);

    static void CreateLog(const PacketLog& log);

    static void ReadLog();


private:
};


#endif //HANIUM_IOT_GATEWAY_LOGGER_H
