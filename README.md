# IoT Device Gateway

---

## Description

---

## Environment

---

### Target Machine 

- [Latte Panda](https://www.lattepanda.com/products/3.html)
  - x86_64 Architecture
  - Ubuntu 20.04
  

- [ODROID N2+](https://www.hardkernel.com/ko/shop/odroid-n2-with-4gbyte-ram-2/) (Future..)
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


## Linux Dev Setup

---

### Install Build Tools
<pre>sudo apt-get update -y</pre>
<pre>sudo apt-get install -y build-essential</pre>

### Install Mosquitto Dev Library

<pre>sudo apt-get update -y</pre>
<pre>sudo apt-get install -y libmosquitto-dev</pre>

### Download from repository

<pre>git clone https://github.com/KGESH/hanium-iot-gateway.git</pre>

### Go to download path

<pre>cd hanium-iot-gateway</pre>

### CMake Build With Your IDE

### Build
