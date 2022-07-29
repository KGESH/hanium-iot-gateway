cmake_minimum_required(VERSION 3.21)
project(hanium_iot_gateway)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(lib/mosquitto/include)
include_directories(lib/mosquitto/lib/cpp)
include_directories(lib/serial/include)
include_directories(lib/rapidjson/include)
include_directories(lib/sqlite)


# If Debug Build
# Then Set DEBUG FLAG
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Set App Sources
set(APP_SOURCES
        src/main.cpp)


# Packet module
set(PACKET_SOURCES
        src/packet/request_packet.cpp
        src/packet/response_packet.cpp)
add_library(packet ${PACKET_SOURCES})


# Gateway manager module
set(GATEWAY_MANAGER_SOURCES
        src/master/master_board.cpp
        src/gateway/gateway_manager.cpp)
add_library(gateway ${GATEWAY_MANAGER_SOURCES})


# Mqtt manager module
set(MQTT_MANAGER_SOURCES
        src/mqtt/mqtt_manager.cpp
        src/mqtt/mqtt_topic.cpp)
add_library(mqtt ${MQTT_MANAGER_SOURCES})


# Packet logger module
set(LOGGER_SOURCES
        src/database/database.cpp
        src/logger/packet_log.cpp
        src/logger/logger.cpp)
add_library(logger ${LOGGER_SOURCES})


# Set Serial Library
set(SERIAL_SOURCES lib/serial/src/serial.cc)
IF (WIN32)
    list(APPEND SERIAL_SOURCES lib/serial/src/impl/win.cc)
ELSE (WIN32)
    list(APPEND SERIAL_SOURCES lib/serial/src/impl/unix.cc)
ENDIF (WIN32)
# Add default header files
#set(SERIAL_HEADERS lib/serial/include/serial/serial.h)
add_library(serial ${SERIAL_SOURCES})
target_link_libraries(serial ${CMAKE_THREAD_LIBS_INIT})

set(SQLITE_SOURCES lib/sqlite/sqlite3.c)
add_library(sqlite3 ${SQLITE_SOURCES})


# Make Executable Binary
add_executable(${PROJECT_NAME} ${APP_SOURCES})


# Link Mosquitto Library
find_package(PkgConfig REQUIRED)
pkg_check_modules(Mosquitto REQUIRED IMPORTED_TARGET libmosquittopp)
#target_link_libraries(${PROJECT_NAME} PkgConfig::Mosquitto)
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES MosquittoInterface.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


target_link_libraries(${PROJECT_NAME}
        packet
        gateway
        mqtt
        logger
        serial
        sqlite3
        PkgConfig::Mosquitto
        )
