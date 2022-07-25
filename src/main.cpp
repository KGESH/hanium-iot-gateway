#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include "protocol/protocol.h"
#include "mqtt/mqtt_manager.h"
#include "gateway/gateway_manager.h"
#include "mqtt/mqtt_config.h"
#include "master/master_config.h"
#include "database/database_config.h"
#include "logger/logger.h"

[[noreturn]] void run() {
    using namespace std::chrono_literals;
    using namespace std::chrono;

    std::mutex mutex;
    std::condition_variable cv;
    Packet::RAW_PACKET_Q mqtt_receive_packets;

    /** Todo: Refactor Mutex */
    auto gateway_manager = GatewayManager(SERIAL_PORT, BAUDRATE, &mqtt_receive_packets, &mutex, &cv);

    while (!gateway_manager.SetupMasterId()) {
        std::cout << "Retry get master id ..." << std::endl;
        std::this_thread::sleep_for(2000ms);
    }

    while (!gateway_manager.SetupSlaveIds()) {
        std::cout << "Retry get slave ids ..." << std::endl;
        std::this_thread::sleep_for(2000ms);
    }

    auto mqtt_manager = MQTTManager(CLIENT_ID, HOST, MQTT_PORT, &mqtt_receive_packets, &mutex, &cv);
    unsigned long polling_interval = 0;
    unsigned long temperature_interval = 0;
    std::thread mqtt_packet_writer(&GatewayManager::WritePacket, &gateway_manager);
    mqtt_packet_writer.detach();


    while (true) {
        if (mqtt_manager.IsConnected()) {
            auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

            if (now - polling_interval > 7000) {
                polling_interval = now;
                gateway_manager.Polling(mqtt_manager);
            }

            if (now - temperature_interval > 15000) {
                temperature_interval = now;
                gateway_manager.RequestTemperature();
            }

            gateway_manager.ListeningMaster(mqtt_manager);

        } else {
            mqtt_manager.Reconnect();
        }

        std::this_thread::sleep_for(500ms);
    }
}

int main() {
    try {
        Logger::Init(MEMORY_DATABASE);
        run();

    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
