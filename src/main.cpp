#include <iostream>
#include <chrono>
#include <unistd.h>
#include "mqtt_manager.h"
#include "gateway_manager.h"


[[noreturn]] void run() {
    auto& mqtt_manager = MQTTManager::GetInstance();
    auto& gateway_manager = GatewayManager::GetInstance();
    unsigned long polling_interval = 0;

    while (true) {
        sleep(1);

        if (mqtt_manager.IsConnected()) {
            using namespace std::chrono;
            auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

            if (now - polling_interval > 3000) {
                polling_interval = now;
                gateway_manager.Polling(mqtt_manager);
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
