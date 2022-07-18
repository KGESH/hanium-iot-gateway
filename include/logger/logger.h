//
// Created by 지성현 on 2022/07/18.
//

#ifndef HANIUM_IOT_GATEWAY_LOGGER_H
#define HANIUM_IOT_GATEWAY_LOGGER_H
#include "sqlite3.h"
#include "database/database_config.h"
#include "packet_log.h"







class Logger {
public:
    Logger();

    void Init();

    void CreateLog(const PacketLog& log);

private:
    sqlite3* db;

};

#endif //HANIUM_IOT_GATEWAY_LOGGER_H
