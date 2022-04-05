#include <iostream>
#include <chrono>
#include <thread>
#include "mqtt/mqtt_manager.h"
#include "gateway/gateway_manager.h"


[[noreturn]] void run() {
    auto& mqtt_manager = MQTTManager::GetInstance();
    auto& gateway_manager = GatewayManager::GetInstance();
    unsigned long polling_interval = 0;

    while (true) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1000ms);

        if (mqtt_manager.IsConnected()) {
            using namespace std::chrono;
            auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

//            if (now - polling_interval > 3000) {
//                polling_interval = now;
//                gateway_manager.Polling(mqtt_manager);
//            }

            if (gateway_manager.ListeningMaster(mqtt_manager)) {
                std::cout << "Receive Success!" << std::endl;
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
