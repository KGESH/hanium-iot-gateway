# IoT Device Gateway

---

## Description

마스터보드가 이더넷 통신을 할 수 있도록 패킷을 교환하는 게이트웨이. 
시리얼 통신 패킷과 MQTT 패킷을 변환.
---


## Environment

---

### Target Machine 

- [Latte Panda](https://www.lattepanda.com/products/3.html)
  - x86_64 Architecture
  - Ubuntu 20.04
  

- [ODROID N2+](https://www.hardkernel.com/ko/shop/odroid-n2-with-4gbyte-ram-2/) (Future ...)
  - Arm 64 Architecture
  - Ubuntu 20.04


### Build Machine

- x86_64 Architecture
- Ubuntu 20.04
- C++ 20
- CMake 3.21

### External Library

- [Rapid JSON](https://rapidjson.org/)
- [Mosquitto](https://github.com/eclipse/mosquitto)
- [Serial](https://github.com/wjwwood/serial)


## How to build

---

### Install build tools
<pre>sudo apt-get update -y</pre>
<pre>sudo apt-get install -y build-essential</pre>

### Install Mosquitto dev library

<pre>sudo apt-get update -y</pre>
<pre>sudo apt-get install -y libmosquitto-dev libmosquittopp-dev</pre>

### Download from repository

<pre>git clone https://github.com/KGESH/hanium-iot-gateway.git</pre>

### Go to download path

<pre>cd hanium-iot-gateway</pre>

### CMake build with your IDE

### Build to target machine

## How to run

---

#### 1. Check your MQTT message broker
#### 2. Connect serial cable to master board
#### 3. Execute binary sudo (serial port need sudo permission!)

<pre>sudo ./hanium_iot_gateway</pre>

