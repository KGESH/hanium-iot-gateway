//
// Created by 지성현 on 2022/07/19.
//

#ifndef HANIUM_IOT_GATEWAY_DATABASE_H
#define HANIUM_IOT_GATEWAY_DATABASE_H

#include "sqlite3.h"
#include <iostream>

namespace Database {
    extern sqlite3* db;

    extern int Callback(void* NotUsed,
                 int argc,
                 char** argv,
                 char** column_name);

    extern void StartDatabaseEngine(const std::string& database_name);
}


#endif //HANIUM_IOT_GATEWAY_DATABASE_H
