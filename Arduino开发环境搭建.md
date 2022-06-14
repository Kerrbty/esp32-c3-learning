# Arduino ��������� {ignore}

[toc]

## ��װ Arduino 

������ַ: https://www.arduino.cc/en/software

## ���� Support Soc ���

����: �ļ� --> ��ѡ�� --> ���ӿ������������ַ ( ����ö��Ÿ���, githubʹ�� [���ٵ�ַ](https://hub.0z.gs) ,  �ֿ� [Release ���������±���ַ](https://hub.0z.gs/espressif/arduino-esp32/releases) )

```text
�ȶ��棺
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

�����棺
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
```


## ��װ��չ��� 

���:  ���� --> ������: "Arduino Uno" --> ����������� --> ����"esp32" --> �����������ѡ��汾��װ 

���ذ�װ��λ��: %arduino%/staging/packages (prefereces.txt Ŀ¼��)

��װ�õĿ�Ϊ��̬��, Դ����github��:  https://hub.0z.gs/espressif/arduino-esp32


## �����ESP32C3����֧�� Serial.print ��ӡ��־��Ҫ�޸Ŀ�Դ��: 

```C++
// /packages/esp32/hardware/esp32/2.0.3-RC1/cores/esp32/HWCDC.h 
extern HWCDC Serial;
// �޸�Ϊ 
#if CONFIG_IDF_TARGET_ESP32C3
extern HWCDC HWCDCSerial;
#else
extern HWCDC Serial;
#endif // CONFIG_IDF_TARGET_ESP32C3 

// /packages/esp32/hardware/esp32/2.0.3-RC1/cores/esp32/HWCDC.cpp
HWCDC Serial;
// �޸�Ϊ 
#if CONFIG_IDF_TARGET_ESP32C3
HWCDC HWCDCSerial;
#else
HWCDC Serial;
#endif  // CONFIG_IDF_TARGET_ESP32C3 

// /packages/esp32/hardware/esp32/2.0.3-RC1/cores/esp32/HardwareSerial.h
extern HardwareSerial Serial0;
// �������: 
#if CONFIG_IDF_TARGET_ESP32C3
extern HardwareSerial Serial;
#endif  // CONFIG_IDF_TARGET_ESP32C3 

// /packages/esp32/hardware/esp32/2.0.3-RC1/cores/esp32/HardwareSerial.cpp 
HardwareSerial Serial0(0);
// �������: 
#if CONFIG_IDF_TARGET_ESP32C3
HardwareSerial Serial(0);
#endif  // CONFIG_IDF_TARGET_ESP32C3 

// ע��: sdk �� Serial1 ʹ��RX 18, TX 19, �����LUATOS��ͬ 
```
