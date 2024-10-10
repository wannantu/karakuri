#ifndef ESP32_3_H
#define ESP32_3_H
 
#include "mbed.h"

#define bytenum 3
#define startbyte 255
#define bitnum 8
#define buttinnum 16

#define Right 0
#define Down 1
#define Up 2
#define Left 3
#define Circle 4
#define Cross 5
#define Square 6
#define Triangle 7
#define L1 8
#define R1 9
#define L2 10
#define R2 11
#define select 12
#define Start 13
#define R3 14
#define L3 15

class ESP32{
public  :
    bool Button[18];
    int Stick[4];
    ESP32(PinName TX, PinName RX);
    void init(long baudrate);
    bool button(int num);
    void Bitcheck();
    uint8_t bitbox[3];
 
private :
    RawSerial esp;
    int count = 0;
    //uint8_t bitbox[3];
    //uint8_t start;
    bool start = false;
    uint8_t CheckBit[8] = {0x01, 0x02, 0x04, 0x08,
                        0x10, 0x20, 0x40, 0x80};
    int val;
    void intSerial();
 
};
 
#endif