# Arduino 开发环境搭建 {ignore}

[toc]

## 安装 Arduino 

官网地址: https://www.arduino.cc/en/software

## 设置 Support Soc 表格

设置: 文件 --> 首选项 --> 附加开发板管理器网址 ( 多个用逗号隔开, github使用 [加速地址](https://hub.0z.gs) ,  仓库 [Release 里面有最新表格地址](https://hub.0z.gs/espressif/arduino-esp32/releases) )

```text
稳定版：
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

开发版：
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
```


## 安装扩展插件 

入口:  工具 --> 开发板: "Arduino Uno" --> 开发板管理器 --> 搜索"esp32" --> 搜索结果里面选择版本安装 

下载安装包位于: %arduino%/staging/packages (prefereces.txt 目录下)

安装好的库为静态库, 源码在github上:  https://hub.0z.gs/espressif/arduino-esp32


## 合宙的ESP32C3板子支持 Serial.print 打印日志需要修改库源码: 

注：以下第三方库文件目录均在 preferebces.txt 同目录下 ( 文件--> 首选项 , 在Arduino安装目录下新建一个 portable 目录 即可修改到安装目录下)：

![首选项目录](./img/arduino_config.png)

```C++
// ./packages/esp32/hardware/esp32/2.0.3-RC1/cores/esp32/HWCDC.h 
extern HWCDC Serial;
// 修改为 
#if CONFIG_IDF_TARGET_ESP32C3
extern HWCDC HWCDCSerial;
#else
extern HWCDC Serial;
#endif // CONFIG_IDF_TARGET_ESP32C3 

// ./packages/esp32/hardware/esp32/2.0.3-RC1/cores/esp32/HWCDC.cpp
HWCDC Serial;
// 修改为 
#if CONFIG_IDF_TARGET_ESP32C3
HWCDC HWCDCSerial;
#else
HWCDC Serial;
#endif  // CONFIG_IDF_TARGET_ESP32C3 

// ./packages/esp32/hardware/esp32/2.0.3-RC1/cores/esp32/HardwareSerial.h
extern HardwareSerial Serial0;
// 后面加入: 
#if CONFIG_IDF_TARGET_ESP32C3
extern HardwareSerial Serial;
#endif  // CONFIG_IDF_TARGET_ESP32C3 

// ./packages/esp32/hardware/esp32/2.0.3-RC1/cores/esp32/HardwareSerial.cpp 
HardwareSerial Serial0(0);
// 后面加入: 
#if CONFIG_IDF_TARGET_ESP32C3
HardwareSerial Serial(0);
#endif  // CONFIG_IDF_TARGET_ESP32C3 

// 注意: sdk 中 Serial1 使用RX 18, TX 19, 与合宙LUATOS不同 
```
