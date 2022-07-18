#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include "protocol/protocol.h"
#include "mqtt/mqtt_manager.h"
#include "gateway/gateway_manager.h"
#include "mqtt/mqtt_config.h"
#include "master/master_config.h"
#include "logger/logger.h"


[[noreturn]] void run() {
    Logger logger;
    logger.Init();


    std::mutex mutex;
    std::condition_variable cv;
    Packet::RAW_PACKET_Q mqtt_receive_packets;

    /** Todo: Refactor Mutex */
    auto gateway_manager = GatewayManager(SERIAL_PORT, BAUDRATE, &mqtt_receive_packets, &mutex, &cv);
    auto mqtt_manager = MQTTManager(CLIENT_ID, HOST, MQTT_PORT, &mqtt_receive_packets, &mutex, &cv);
    unsigned long polling_interval = 0;
    unsigned long temperature_interval = 0;
    std::thread mqtt_packet_writer(&GatewayManager::WritePacket, &gateway_manager);
    mqtt_packet_writer.detach();


    while (true) {
        using namespace std::chrono_literals;
        if (mqtt_manager.IsConnected()) {
            using namespace std::chrono;
            auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

            if (now - polling_interval > 7000) {
                polling_interval = now;
                gateway_manager.Polling(mqtt_manager);
            }

            if (now - temperature_interval > 15000) {
                temperature_interval = now;
                gateway_manager.RequestTemperature();
            }

            if (gateway_manager.ListeningMaster(mqtt_manager)) {
#ifdef DEBUG
                std::cout << "Receive Success!" << std::endl;
#endif
            }

        } else {
            mqtt_manager.Reconnect();
        }

        std::this_thread::sleep_for(500ms);
    }
}

int main() {
    try {
        run();

    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
