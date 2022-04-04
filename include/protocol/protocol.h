//
// Created by 지성현 on 2022/03/30.
//

#ifndef PLANT_GATEWAY_PROTOCOL_H
#define PLANT_GATEWAY_PROTOCOL_H

/*  TODO: Extract Status Code After..  */
#include <cstdint>

enum EMasterProtocol {
    kStart = 0x23,
    kIndex = 0x21,
    kPolling = 0xa0,
    kMemoryRead = 0xc1,
    kMemoryWrite = 0xd1,
    kEmergency = 0xe0,
    kMasterAddress = 0xff,
    kMasterSlaveAddress = 0,
    kEnd = 0x0d,
};

enum EMemoryMap {
    kTemperatureStart = 2000,
    kTemperatureEnd = 2019,
    kHumidityStart = 2020,
    kHumidityEnd = 2039,

    /*  TODO: Change After Demo  */
    kLuxStart = 2509,
    kWindStart = 2763,
//    kInputMemoryEnd = 4000,

    kMotorStart = 4000,
    kMotorEnd = 4059,
    kLedStart = 4060,
    kLedEnd = 4119,

    kFanStart = 5017,
    kOutputMemoryEnd = 6000,

    kControlStart = 6001,
    kInsideLedStart = 6509, // Need Rename
    kMachineStart = 7017,    // Need Rename
    kIOMemoryEnd = 8000,

    kMasterStart = 8001,
    /*  TODO: Read Document and Code it!  */

    kMasterFreeStart = 8008,
    kMasterFreeEnd = 8009,
    kMasterBuzzerStart = 8203,
    kMasterTemperatureStart = 8204,
    kMasterHumidityStart = 8206,
};

enum EErrorStatusCode {
    kError1 = 0x01,
    kError2,
    kError3,
    kOK = 0x80,
};

enum EReceiveErrorCode {
    kSuccessReceive = 0,
    kFailReceiveHeader = 1,
    kFailOverMaxSlaveCount = 2,
    kFailReceiveBodyData = 3,
    kFailReceiveTail = 4,
};

enum EParseJsonErrorCode {
    kSuccessParse = 0,
    kFailParsePayload = 1,
    kFailIsNotObject = 2,


};


#endif //PLANT_GATEWAY_PROTOCOL_H
