//
//jaewoo
//
#include <vector>
#include <string>
#include <experimental/filesystem>
#include "../../include/master/master_config.h"

namespace Serial{
    void findSerialPort(void){
        using std::experimental::filesystem::directory_iterator;
        std::string path = "/dev";
        std::string tmp;
        for (const auto & file : directory_iterator(path)){
            tmp = file.path();
            if(tmp.find("tty.usbserial")!=std::string::npos){   //find("감지할 포트파일 이름")
                PRE_SERIAL_PORT.push_back(tmp.substr(5,tmp.size())); //usb감지하면 예비usb배열에 넣어줌
            }
        }
    }
    void changeSerialPort(void){
        SERIAL_PORT = PRE_SERIAL_PORT[cnt];
        cnt = (cnt+1)%PRE_SERIAL_PORT.size();
    }
}