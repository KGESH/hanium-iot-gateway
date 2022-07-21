cmake_minimum_required(VERSION 3.21)
project(hanium_iot_gateway)

set(CMAKE_CXX_STANDARD 20)


include_directories(include)
include_directories(lib/serial/include)
include_directories(lib/rapidjson/include)
include_directories(lib/sqlite)
#set(SQLite3_INCLUDE_DIR lib/sqlite)



# If Debug Build
# Then Set DEBUG FLAG
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Set App Sources
# If Create New cpp file
# Then Import Here
set(APP_SOURCES
        src/mqtt/mqtt_manager.cpp src/master/master_board.cpp src/gateway/gateway_manager.cpp
        src/packet/request_packet.cpp src/packet/response_packet.cpp
        src/database/database.cpp
        src/logger/packet_log.cpp
        src/logger/logger.cpp
        src/main.cpp)

# Set Serial Library
set(SERIAL_SOURCES lib/serial/src/serial.cc)
IF (WIN32)
    list(APPEND SERIAL_SOURCES lib/serial/src/impl/win.cc)
ELSE (WIN32)
    list(APPEND SERIAL_SOURCES lib/serial/src/impl/unix.cc)
ENDIF (WIN32)
# Add default header files
set(SERIAL_HEADERS lib/serial/include/serial/serial.h)

add_library(serial ${SERIAL_SOURCES})
target_link_libraries(serial ${CMAKE_THREAD_LIBS_INIT})

set(SQLITE_SOURCES lib/sqlite/sqlite3.c)
add_library(sqlite3 STATIC ${SQLITE_SOURCES})
#add_executable(sqlite )

#set(SQLite3_LIBRARY lib/sqlite)

#add_library(SQLITE ${SQLITE_SOURCES})


# Make Executable Binary
add_executable(${PROJECT_NAME} ${APP_SOURCES} ${SERIAL_SOURCES} ${SQLITE_SOURCES})
#${SQLITE_SOURCES}
target_link_libraries(${PROJECT_NAME} sqlite3 -ldl)


# Link Mosquitto Library
find_package(PkgConfig REQUIRED)
pkg_check_modules(Mosquitto REQUIRED IMPORTED_TARGET libmosquittopp)
target_link_libraries(${PROJECT_NAME} PkgConfig::Mosquitto)
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES MosquittoInterface.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


#find_package(SQLite3)
#include_directories(${SQLite3_INCLUDE_DIR})
#target_link_libraries (${PROJECT_NAME} ${SQLite3_LIBRARY})


# Link Thread Library
#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
#target_link_libraries(${PROJECT_NAME} Threads::Threads)
