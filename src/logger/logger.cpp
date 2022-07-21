//
// Created by 지성현 on 2022/07/18.
//
#include "logger/logger.h"
#include "database/database.h"
#include <iostream>
#include <cstring>

void Logger::Init(const std::string& database_name) {
    Database::StartDatabaseEngine(database_name);
    constexpr auto init_sql = "CREATE TABLE IF NOT EXISTS logs(id INTEGER PRIMARY KEY AUTOINCREMENT, direction TEXT, packet TEXT, event_name TEXT, create_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";
    char* error_message;

    int rc = sqlite3_exec(Database::db, init_sql, nullptr, nullptr, &error_message);
    if (rc != SQLITE_OK) {
        std::cout << "Init SQL error!" << std::endl;
        std::cout << error_message << std::endl;
        sqlite3_free(error_message);
        sqlite3_close(Database::db);
        throw std::exception();
    }
}


void Logger::ReadLog() {
    constexpr auto sql = "SELECT * FROM 'logs';";
    char* error_message = nullptr;

    int rc = sqlite3_exec(Database::db, sql, Database::Callback, nullptr, &error_message);
    if (rc != SQLITE_OK) {
        std::cout << "Read Log SQL error!" << std::endl;
        sqlite3_close(Database::db);
        throw std::exception();
    }
}

void Logger::CreateLog(const PacketLog& log) {
    sqlite3_stmt* res;
    constexpr auto sql = "INSERT INTO 'logs' ('direction', 'event_name', 'packet') VALUES(?, ?, ?);";

    int rc = sqlite3_prepare_v3(Database::db, sql, strlen(sql), 0, &res, nullptr);
    sqlite3_bind_text(res, 1, log.direction().c_str(), log.direction().length(), nullptr);
    sqlite3_bind_text(res, 2, log.event_name().c_str(), log.event_name().length(), nullptr);
    sqlite3_bind_text(res, 3, log.packet().c_str(), log.packet().length(), nullptr);

    rc = sqlite3_step(res);
    if (rc != SQLITE_DONE) {
        std::cout << "Create Log SQL error!!" << std::endl;
        std::cout << sqlite3_errmsg(Database::db) << std::endl;
        sqlite3_close(Database::db);
        throw std::exception();
    }
    sqlite3_reset(res);
}


