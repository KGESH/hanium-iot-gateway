//
// Created by 지성현 on 2022/03/30.
//

#include <cmath>
#include <chrono>
#include <thread>
#include "gateway/gateway_manager.h"
#include "mqtt/mqtt_manager.h"
#include "mqtt/mqtt_topic.h"
#include "packet/response_packet.h"
#include "util/util.h"
#include "logger/logger.h"


GatewayManager::GatewayManager(const std::string& serial_port_name, int baudrate,
                               Packet::RAW_PACKET_Q* mqtt_packet_queue,
                               std::mutex* g_mqtt_queue_mutex,
                               std::condition_variable* g_cv)
        : master_board_(serial_port_name, baudrate),
          raw_packet_queue(mqtt_packet_queue),
          g_mqtt_queue_mutex(g_mqtt_queue_mutex), g_cv(g_cv) {}


/**
 * @return master/{MasterId}/slave/{SlaveId}/{SensorName}/state
 * */
std::string GatewayManager::GetSlaveStateTopic(const uint8_t slave_id, const std::string& sensor_name) const {
    return kMasterTopic + kMasterId + "/slave/" + std::to_string(slave_id) + "/" + sensor_name + "/state";
}

bool GatewayManager::ListeningMaster(MQTTManager& mqtt_manager) const {
    auto[packet, receive_fail] = ReceivePacket();

    if (receive_fail || !packet.ValidChecksum()) {
        if (receive_fail != EReceiveErrorCode::kFailReceiveHeader) {
            PacketLog log("MASTER_TO_GATEWAY", "RECEIVE_FAIL", "CODE: " + std::to_string(receive_fail));
            Logger::CreateLog(log);
        }

        return false;
    }

#ifdef DEBUG
    if (packet.header().error_code != kOK) {
        std::cout << "Publish Error topic" << std::endl;
        PublishError(mqtt_manager, kErrorTopic, Util::PacketToString(packet));
        PacketLog log("MASTER_TO_GATEWAY", "HEADER_CODE", Util::PacketToString(packet));
    }
#endif

    ParseCommand(packet, mqtt_manager);
    return true;
}

void GatewayManager::ParseCommand(ResponsePacket& packet, MQTTManager& mqtt_manager) const {
    uint16_t target_memory_address = 0;

    if (packet.header().data_length > 0) {
        /* Make 16bit address example
         * high 8bit address - 0x20
         * low 8bit address  - 0x0c
         * expect result     - 0x200c  */
        target_memory_address = (packet.body().high_address << 8) + packet.body().low_address;
    }

    switch (packet.header().command) {

        case kPolling:
            PublishPollingSuccess(mqtt_manager);
            return;

            /*  TODO: Refactor after change protocol  */
        case kMemoryRead:
            ParseMemoryRead(packet, mqtt_manager, target_memory_address);
            return;

        case kMemoryWrite:
            ParseMemoryWrite(packet, mqtt_manager, target_memory_address);
//            PublishTestPacket(packet, mqtt_manager);
            return;

        case kEmergency:
            ParseEmergency(packet, mqtt_manager, target_memory_address);
            return;


        default:
            /* ASSERT */
#ifdef DEBUG
            std::cout << "Parse Command Exception" << std::endl;
#endif
            PublishError(mqtt_manager, kAssertTopic + "/ParseDefault", Util::PacketToString(packet));
            return;
    }
}

void GatewayManager::ParseMemoryRead(ResponsePacket& packet, MQTTManager& mqtt_manager, uint16_t memory_address) const {
    switch (memory_address) {
        case kTemperatureStart ... kTemperatureEnd:
            PublishTemperature(packet, mqtt_manager);
            return;

        case kHumidityStart ... kHumidityEnd:
            /** Todo: Assert */
            PublishError(mqtt_manager, kAssertTopic + "/MemReadHumidity", Util::PacketToString(packet));
            PublishTestPacket(packet, mqtt_manager);
            return;

        case kMotorStart ... kMotorEnd:
#ifdef DEBUG
            /** Todo: Assert */
            PublishError(mqtt_manager, kAssertTopic + "/MemReadMotor", Util::PacketToString(packet));

#endif
            PublishMotorTopic(packet, mqtt_manager);
            return;

        case kLedStart ... kLedEnd:
#ifdef DEBUG
            /** Todo: Assert */
            PublishError(mqtt_manager, kAssertTopic + "/MemReadLed", Util::PacketToString(packet));

#endif
            PublishLedTopic(packet, mqtt_manager);
            return;

        case kFanStart ... kFanEnd:
#ifdef DEBUG
            /** Todo: Assert */
            PublishError(mqtt_manager, kAssertTopic + "/MemReadFan", Util::PacketToString(packet));
#endif
            PublishFanTopic(packet, mqtt_manager);
            return;


            /*  TODO: Add Function After Change Protocol  */
        default:
#ifdef DEBUG
            /** Todo: Assert */
            PublishError(mqtt_manager, kAssertTopic + "/MemReadDefault", Util::PacketToString(packet));

#endif
            return;

    }
}

void GatewayManager::PublishLedTopic(ResponsePacket& packet, MQTTManager& mqtt_manager) const {
    const auto led_topic = std::move(GetSlaveStateTopic(packet.header().target_id, "led"));
    std::cout << "Publish Led Topic" << led_topic << std::endl;

    /* TODO: Change message */
    mqtt_manager.PublishTopic(led_topic, Util::PacketToString(packet));
}

void GatewayManager::PublishMotorTopic(ResponsePacket& packet, MQTTManager& mqtt_manager) const {
    auto motor_topic = std::move(GetSlaveStateTopic(packet.header().target_id, "water"));
    /* TODO: Change message */
    mqtt_manager.PublishTopic(motor_topic, Util::PacketToString(packet));
}

void GatewayManager::PublishFanTopic(ResponsePacket& packet, MQTTManager& mqtt_manager) const {
    auto fan_topic = std::move(GetSlaveStateTopic(packet.header().target_id, "fan"));
    /* TODO: Change message */
    mqtt_manager.PublishTopic(fan_topic, Util::PacketToString(packet));
}


void GatewayManager::PublishTestPacket(ResponsePacket& packet, MQTTManager& mqtt_manager) const {
    auto message(Util::PacketToString(packet));

    mqtt_manager.PublishTopic(kTestResponsePacket, message);
}


void GatewayManager::PublishPollingSuccess(MQTTManager& mqtt_manager) const {
    mqtt_manager.PublishTopic(kPollingTopic, std::to_string(kOK));
    master_board_.ResetPollingCount();
}

void GatewayManager::PublishTemperature(ResponsePacket& packet, MQTTManager& mqtt_manager) const {
    /*  온도 값 예시
     *  high low
     *  0x02 0x35 = 23.5도
     *  0xf1 0x90 = -19.0도
     *  high byte 십의 자리는 부호를 표현
     *  0이면 양수, f면 음수
     *  high byte 일의 자리는 온도의 십의 자리를 표현
     *  low byte 십의 자리는 온도의 일의 자리를 표현
     *  low byte 일의 자리는 온도의 소숫점 첫째 자리를 표현
     * */
    const auto high_data = packet.body().data[0];
    const auto low_data = packet.body().data[1];
    uint8_t ten = (high_data & 0x0f) * 0x10;   //  (num * 0x10) Equal (num << 4)
    uint8_t one = (low_data & 0xf0) / 0x10;    //  (num / 0x10) Equal (num >> 4)
    float point = static_cast<float>(high_data & 0x0f) * 0.1f;

    /*  소숫점 둘째 자리까지 반올림  */
    float temperature = std::round((static_cast<float>(ten + one) + point) * 100) / 100;

    /*  temperature 영하  */
    if (high_data > 0xf0) {
        temperature *= -1;
    }

    const auto slave_id(std::to_string(packet.header().target_id));
    std::string topic = "master/" + kMasterId + "/slave/" + slave_id + "/temperature";
    mqtt_manager.PublishTopic(topic, std::to_string(temperature));
}

void GatewayManager::Polling(MQTTManager& mqtt_manager) const {
    static constexpr auto kMaxPollingCount = 10;
    if (master_board_.polling_count() > kMaxPollingCount) {
        /*  TODO: Publish master error message after change protocol  */
#ifdef DEBUG
        std::cout << "Polling Count Over 10" << std::endl;
#endif
        PublishError(mqtt_manager, kPollingErrorTopic, "no response");
    }

    RequestPacket polling_packet(RequestHeader{0x23, 0x27, 0xff, 0xa0, 0});
    {
        std::unique_lock<std::mutex> lock(*this->g_mqtt_queue_mutex);
        this->raw_packet_queue->push(polling_packet.Packet());
    }
    g_cv->notify_one();
    master_board_.IncreasePollingCount();
}

std::pair<ResponsePacket, EReceiveErrorCode> GatewayManager::ReceivePacket() const {
    constexpr static uint8_t kMaxSlaveCount = 129;

    while (master_board_.serial_port().available()) {
        uint8_t packet_length = 0;
        ResponseHeader header;

        /* 1byte 읽고 패킷의 시작인지 판단
         * 패킷의 시작이면 header에 복사
         * 이 때, 복사 시작 주소는 header + 1
         * header 첫번째 멤버 start의
         * 기본 값이 0x23이기 때문
         * */
        if (static_cast<uint8_t>(master_board_.serial_port().read().c_str()[0]) == kStart &&
            master_board_.serial_port().read(reinterpret_cast<uint8_t*>(&header) + 1, sizeof(ResponseHeader) - 1)) {

            if (header.data_length > kMaxSlaveCount) {
                /* Need Memory Crash Handling  */
                return {{}, kFailOverMaxSlaveCount};
            }

            /*  Read Body
             *  If Body Packet Exist  */
            std::vector<uint8_t> body(header.data_length);
            if (header.data_length > 0) {
                packet_length = master_board_.serial_port().read(body.data(), header.data_length);

                if (packet_length <= 0) {
                    return {{}, kFailReceiveBodyData};
                }
            }

            std::string buffer;
            auto receive_tail_length = master_board_.serial_port().readline(buffer, sizeof(PacketTail),
                                                                            std::to_string(kEnd));
            PacketTail tail{static_cast<uint8_t>(buffer.c_str()[0]), static_cast<uint8_t>(buffer.c_str()[1])};

            if (receive_tail_length <= 0) {
                return {{}, kFailReceiveTail};

            } else {
                return {ResponsePacket{header, body, tail}, kSuccessReceive};
            }
        }
    }

    return {{}, kFailReceiveHeader};
}

void
GatewayManager::PublishError(MQTTManager& mqtt_manager, const std::string& topic, const std::string& message) const {
    mqtt_manager.PublishTopic(topic, message);
}


void GatewayManager::RequestTemperature() const {
    /* TODO: after iterator all slave id
     * ex) 0x11, 0x12, 0x22, 0x33 ...*/
    constexpr uint8_t kSlaveId = 0x11;
    const RequestHeader header{0x23, 0x22, kSlaveId, 0xc1, 0x02};
    const PacketBody body{0x07, 0xd0};
    RequestPacket temperature_packet(header, body);
    {
        std::unique_lock<std::mutex> lock(*this->g_mqtt_queue_mutex);
        this->raw_packet_queue->push(temperature_packet.Packet());
    }
}

void GatewayManager::ParseEmergency(ResponsePacket& packet, MQTTManager& mqtt_manager, uint16_t memory_address) const {
    switch (memory_address) {
        case kTemperatureStart ... kTemperatureEnd:
            /** Todo: Mock Emergency */
            PublishError(mqtt_manager, kAssertTopic + "/EmergencyTemperature", Util::PacketToString(packet));

            return;

        case kHumidityStart ... kHumidityEnd:
            /** Todo: Mock Emergency */
            PublishError(mqtt_manager, kAssertTopic + "/EmergencyHumidity", Util::PacketToString(packet));
            return;

        case kMotorStart ... kMotorEnd:
#ifdef DEBUG
            PublishError(mqtt_manager, kAssertTopic + "/EmergencyMotor", Util::PacketToString(packet));
#endif
            PublishSensorStateTopic(packet, mqtt_manager, "water");
            return;

        case kLedStart ... kLedEnd:
#ifdef DEBUG
            PublishError(mqtt_manager, kAssertTopic + "/EmergencyLed", Util::PacketToString(packet));
#endif
            PublishSensorStateTopic(packet, mqtt_manager, "led");
            return;

        case kFanStart ... kFanEnd:
#ifdef DEBUG
            PublishError(mqtt_manager, kAssertTopic + "/EmergencyFan", Util::PacketToString(packet));
#endif
            PublishSensorStateTopic(packet, mqtt_manager, "fan");

            /*  TODO: Add Function After Change Protocol  */
        default:
#ifdef DEBUG
            std::cout << "Call Parse Emergency Default Assert" << std::endl;
#endif
            PublishError(mqtt_manager, kAssertTopic + "/EmergencyDefault", Util::PacketToString(packet));
            return;

    }
}

void GatewayManager::PublishSensorStateTopic(ResponsePacket& packet, MQTTManager& mqtt_manager,
                                             std::string&& sensor_name) const {
    /**
     * Todo: Check Exception
     *       패킷 실행시간 > 0 : 실행중
     *       패킷 실행시간 == 0 : 멈춤
     *                              len address  cycle runtime  sum end
     *       수신 패킷: 23 27 11 e0 01 06  0f a2   00 02  00 01    f6 0d
     *                                                high low
     * */

    /**
     * Todo: Validate Emergency Data Length */
    if (packet.header().data_length <= 2) {
#ifdef DEBUG
        std::cout << "Call PublishSensorStateTopic Assert" << std::endl;
#endif
        PublishError(mqtt_manager, kAssertTopic + "/emergencyDataLength", Util::PacketToString(packet));
        return;
    }

    const auto runtime_high_byte = packet.body().data[2] << 8;
    const auto runtime_low_byte = packet.body().data[3];
    const auto runtime_minutes = runtime_high_byte + runtime_low_byte;
    std::cout << "Runtime Min: " << runtime_minutes << std::endl;
    const std::string motor_topic = GetSlaveStateTopic(packet.header().target_id, sensor_name);
    const auto payload = std::to_string(runtime_minutes);
    mqtt_manager.PublishTopic(motor_topic, payload);
}

void
GatewayManager::ParseMemoryWrite(ResponsePacket& packet, MQTTManager& mqtt_manager, uint16_t memory_address) const {
    switch (memory_address) {
        case kTemperatureStart ... kTemperatureEnd:
            /** Todo: Assert */
            PublishError(mqtt_manager, kAssertTopic + "/MemoryWriteTemperature", Util::PacketToString(packet));
            return;

        case kHumidityStart ... kHumidityEnd:
            /** Todo: Assert */
            PublishError(mqtt_manager, kAssertTopic + "/MemoryWriteHumidity", Util::PacketToString(packet));
            return;

        case kMotorStart ... kMotorEnd:
            /**
             * Todo: Response to Server
             *       Extract to Method */
        {
            bool success = packet.header().error_code == kOK;
            auto response_topic =
                    GetSlaveStateTopic(packet.header().target_id, "water") + "/response" +
                    (success ? "" : "/fail");
            /**
             * Todo : Refactor to JSON */

            mqtt_manager.PublishTopic(response_topic, Util::PacketToString(packet));
        }
//            PublishMotorTopic(packet, mqtt_manager);
            return;

        case kLedStart ... kLedEnd:
            /**
             * Todo: Response to Server
             *       Extract to Method */
        {
            bool success = packet.header().error_code == kOK;
            auto response_topic =
                    GetSlaveStateTopic(packet.header().target_id, "led") + "/response" +
                    (success ? "" : "/fail");
            /**
             * Todo : Refactor to JSON */

            mqtt_manager.PublishTopic(response_topic, Util::PacketToString(packet));
        }

//            PublishLedTopic(packet, mqtt_manager);
            return;

        case kFanStart ... kFanEnd:
            /**
             * Todo: Response to Server
             *       Extract to Method */
        {
            bool success = packet.header().error_code == kOK;
            auto response_topic =
                    GetSlaveStateTopic(packet.header().target_id, "fan") + "/response" +
                    (success ? "" : "/fail");
            /**
             * Todo : Refactor to JSON */

            mqtt_manager.PublishTopic(response_topic, Util::PacketToString(packet));
        }
            return;

            /*  TODO: Add Function After Change Protocol  */
        default:
#ifdef DEBUG
            std::cout << "Parse Memory Write Default Assert" << std::endl;
#endif
            PublishError(mqtt_manager, kAssertTopic + "/MemoryWriteDefault", Util::PacketToString(packet));
            return;
    }
}

void GatewayManager::WritePacket() const {
    using namespace std::chrono_literals;
    while (true) {
        {
            std::unique_lock<std::mutex> lock(*g_mqtt_queue_mutex);
            g_cv->wait(lock, [&]() { return !raw_packet_queue->empty(); });
            auto packet = raw_packet_queue->front();
            raw_packet_queue->pop();
            master_board_.serial_port().write(packet);
#ifdef DEBUG
            std::cout << "Write Done: " << Util::PacketToString(packet) << std::endl;
#endif
            {
                PacketLog log("GATEWAY_TO_MASTER", "SERIAL_WRITE", Util::PacketToString(packet));
                Logger::CreateLog(log);
            }
            std::this_thread::sleep_for(500ms);
        }
    }
}

