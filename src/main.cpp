#include <iostream>
#include <chrono>
#include <unistd.h>
#include "mqtt_manager.h"
#include "gateway_manager.h"

unsigned long polling_interval = 0;

/* TODO: Extract Somewhere.. */

constexpr auto MQTT_TOPIC = "test";

[[noreturn]] void run() {
    auto& iot_client = MQTTManager::GetInstance();
    auto& gateway_manager = GatewayManager::GetInstance();

    while (true) {
        sleep(1);

        if (iot_client.IsConnected()) {
            using namespace std::chrono;
            auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

            if (now - polling_interval > 3000) {
                polling_interval = now;
                gateway_manager.Polling(iot_client);
            }

            if (gateway_manager.ListeningMaster(iot_client)) {
                std::cout << "Receive Success!" << std::endl;
            }

        } else {
            iot_client.Reconnect();
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
