#include <iostream>
#include <chrono>
#include <thread>
#include "mqtt/mqtt_manager.h"
#include "gateway/gateway_manager.h"


[[noreturn]] void run() {
    auto& mqtt_manager = MQTTManager::GetInstance();
    auto& gateway_manager = GatewayManager::GetInstance();
    unsigned long polling_interval = 0;
    unsigned long temperature_interval = 0;

    while (true) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1000ms);

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
