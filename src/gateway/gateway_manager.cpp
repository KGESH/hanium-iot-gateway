//
// Created by 지성현 on 2022/03/30.
//

#include <cmath>
#include "master/master_config.h"
#include "gateway/gateway_manager.h"
#include "mqtt/mqtt_manager.h"
#include "mqtt/mqtt_topic.h"
#include "packet/response_packet.h"
#include "util/util.h"


GatewayManager::GatewayManager(const std::string& serial_port_name, int baudrate)
        : master_board_(serial_port_name, baudrate) {}


/**
 * @return master/{MasterId}/slave/{SlaveId}/{SensorName}/state
 * */
std::string GatewayManager::GetSlaveStateTopic(const uint8_t slave_id, const std::string& sensor_name) const {
    return kMasterTopic + kMasterId + "/slave/" + std::to_string(slave_id) + "/" + sensor_name + "/state";
}

bool GatewayManager::ListeningMaster(MQTTManager& mqtt_manager) const {
    auto[packet, receive_fail] = ReceivePacket();

    if (receive_fail || !packet.ValidChecksum()) {
#ifdef DEBUG
        switch (receive_fail) {
            case EReceiveErrorCode::kFailReceiveHeader:
                std::cout << "Wait for receive header.. " << std::endl;
                break;

            case EReceiveErrorCode::kFailReceiveBodyData:
                std::cout << "Receive packet Error: Fail to receive body! " << std::endl;
                break;

            case EReceiveErrorCode::kFailReceiveTail:
                std::cout << "Receive packet Error: Fail to receive tail! " << std::endl;
                break;

            case EReceiveErrorCode::kFailOverMaxSlaveCount:
                std::cout << "Receive packet Error: Max Slave Count Over then 127" << std::endl;
                break;


        }
#endif
        return false;
    }

    if (packet.header().error_code != kOK) {
        PublishError(mqtt_manager, kErrorTopic, Util::PacketToString(packet));
    }

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
            PublishTestPacket(packet, mqtt_manager);
            return;

        case kEmergency:
            ParseEmergency(packet, mqtt_manager, target_memory_address);


        default:
            /* ASSERT */
            PublishError(mqtt_manager, kAssertTopic, Util::PacketToString(packet));
#ifdef DEBUG
            std::cout << "Parse Command Exception" << std::endl;
#endif
            return;
    }
}

void GatewayManager::ParseMemoryRead(ResponsePacket& packet, MQTTManager& mqtt_manager, uint16_t memory_address) const {
    switch (memory_address) {
        case kTemperatureStart ... kTemperatureEnd:
            PublishTemperature(packet, mqtt_manager);
            return;

        case kHumidityStart ... kHumidityEnd:
            PublishTestPacket(packet, mqtt_manager);
            return;

        case kMotorStart ... kMotorEnd:
            PublishMotorTopic(packet, mqtt_manager);
            return;

        case kLedStart ... kLedEnd:
            PublishLedTopic(packet, mqtt_manager);
            return;

            /*  TODO: Add Function After Change Protocol  */
        default:
            PublishTestPacket(packet, mqtt_manager);
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
    master_board_.serial_port().write(polling_packet.Packet());
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

const GatewayManager& GatewayManager::GetInstance() {
    static GatewayManager instance = GatewayManager(SERIAL_PORT, BAUDRATE);

    return instance;
}

const MasterBoard& GatewayManager::master_board() {
    return GetInstance().master_board_;
}

void GatewayManager::RequestTemperature() const {
    /* TODO: after iterator all slave id
     * ex) 0x11, 0x12, 0x22, 0x33 ...*/
    constexpr uint8_t kSlaveId = 0x11;
    const RequestHeader header{0x23, 0x22, kSlaveId, 0xc1, 0x02};
    const PacketBody body{0x07, 0xd0};
    RequestPacket temperature_packet(header, body);
    master_board_.serial_port().write(temperature_packet.Packet());
}

void GatewayManager::ParseEmergency(ResponsePacket& packet, MQTTManager& mqtt_manager, uint16_t memory_address) const {
    switch (memory_address) {
        case kTemperatureStart ... kTemperatureEnd:
            /** Todo: Mock Emergency */
            return;

        case kHumidityStart ... kHumidityEnd:
            /** Todo: Mock Emergency */
            return;

        case kMotorStart ... kMotorEnd:
            PublishSensorStateTopic(packet, mqtt_manager, "water");
            return;

        case kLedStart ... kLedEnd:
            PublishSensorStateTopic(packet, mqtt_manager, "led");
            return;

            /*  TODO: Add Function After Change Protocol  */
        default:
            std::cout << "Call Parse Emergency Default Assert" << std::endl;
            PublishTestPacket(packet, mqtt_manager);
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
     * */

    const auto is_running = packet.body().data[2] + packet.body().data[3];
    const std::string motor_topic = GetSlaveStateTopic(packet.header().target_id, sensor_name);
    const std::string payload = is_running ? "on" : "off";
    mqtt_manager.PublishTopic(motor_topic, payload);
}
