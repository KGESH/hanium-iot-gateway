//
// Created by 지성현 on 2022/07/18.
//
#include "logger/logger.h"
#include <iostream>

Logger::Logger() {
    int rc = sqlite3_open(DEBUG_DATABASE_FILE_NAME, &db);

    if (rc != SQLITE_OK) {
        std::cout << "Database Connect Error!" << std::endl;
        sqlite3_close(db);
        throw std::exception();
    }
}

void Logger::Init() {
    constexpr auto init_sql = "CREATE TABLE IF NOT EXISTS logs(id INTEGER PRIMARY KEY AUTOINCREMENT, packet_type TEXT, packet TEXT, event_name TEXT, create_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";
    int rc = sqlite3_open(DEBUG_DATABASE_FILE_NAME, &db);

    if (rc != SQLITE_OK) {
        std::cout << "Database Connect Error!" << std::endl;
        throw std::exception();
    }

//    constexpr auto init_sql = "DROP TABLE IF EXISTS logs;"
//                              "CREATE TABLE logs(id INTEGER PRIMARY KEY AUTOINCREMENT, packet_type TEXT, packet TEXT, event_name TEXT, create_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);"
//                              "INSERT INTO logs (packet_type, packet, event_name) VALUES('R', '23 27 ff 12 23 12 13', 'Audi');"
//                              "INSERT INTO logs (packet_type, packet, event_name) VALUES('S', '23 27 ff 12 23 12 13', 'Mercedes');"
//                              "INSERT INTO logs (packet_type, packet, event_name) VALUES('R', '23 27 ff 12 23 12 13', 'Skoda');"
//                              "INSERT INTO logs (packet_type, packet, event_name) VALUES('R', '23 27 ff 12 23 12 13', 'Volvo');"
//                              "INSERT INTO logs (packet_type, packet, event_name) VALUES('S', '23 27 ff 12 23 12 13', 'Bentley');"
//                              "INSERT INTO logs (packet_type, packet, event_name) VALUES('R', '23 27 ff 12 23 12 13', 'Citroen');"
//                              "INSERT INTO logs (packet_type, packet, event_name) VALUES('S', '23 27 ff 12 23 12 13', 'Hummer');"
//                              "INSERT INTO logs (packet_type, packet, event_name) VALUES('R', '23 27 ff 12 23 12 13', 'Volkswagen');";


}

void Logger::CreateLog(const PacketLog& log) {
    std::string sql = "INSERT INTO logs (packet_type, packet, event_name) VALUES('R', '23 27 ff 12 23 12 13', 'Audi');";



}
