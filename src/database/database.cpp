//
// Created by 지성현 on 2022/07/19.
//

#include "database/database.h"
#include "database/database_config.h"

namespace Database {
    sqlite3* db = nullptr;

    int Callback(void* NotUsed, int argc, char** argv, char** column_name) {
        for (int i = 0; i < argc; i++) {
            std::cout << column_name[i] << " : " << (argv[i] ? argv[i] : "NULL") << std::endl;
        }

        return 0;
    }

    void StartDatabaseEngine(const std::string& database_name) {
        int rc = sqlite3_open(database_name.c_str(), &Database::db);
        if (rc != SQLITE_OK) {
            std::cout << "Database Connect Error!" << std::endl;
            sqlite3_close(Database::db);
            throw std::exception();
        }


    }


}