# Arduino ��������� {ignore}

[toc]

## ��װ Arduino 

������ַ: https://www.arduino.cc/en/software

## ���� Support Soc ���

����: �ļ� --> ��ѡ�� --> ���ӿ������������ַ (����ö��Ÿ���)

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
extern HWCDC HWCDCSerial;

// /packages/esp32/hardware/esp32/2.0.3-RC1/cores/esp32/HWCDC.cpp
HWCDC Serial;
// �޸�Ϊ 
HWCDC HWCDCSerial;

// /packages/esp32/hardware/esp32/2.0.3-RC1/cores/esp32/HardwareSerial.h
extern HardwareSerial Serial0;
// �������һ��: 
extern HardwareSerial Serial;

// /packages/esp32/hardware/esp32/2.0.3-RC1/cores/esp32/HardwareSerial.cpp 
HardwareSerial Serial0(0);
// �������һ��: 
HardwareSerial Serial(0);

// ע��: sdk �� Serial1 ʹ��RX 18, TX 19, �����LUATOS��ͬ 
```
