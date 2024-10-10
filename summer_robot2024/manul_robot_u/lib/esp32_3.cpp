#include "esp32_3.h"
#include "mbed.h"
 
//
/*long ESP32::map(long x, long in_min, long in_max, long out_min, long out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}*/
 
ESP32::ESP32(PinName TX, PinName RX) : esp(TX,RX) {
    esp.format(8,Serial::None,1);
}


void ESP32::init(long baudrate){
    esp.baud(baudrate);
    esp.attach(callback(this, &ESP32::intSerial), Serial::RxIrq);
}
 
 
bool ESP32::button(int num){
    return Button[num];
}
 
/*int ESP32::stick(int num){
    return Stick[num - buttonnumber];
}*/

void ESP32::Bitcheck(){
    if(bitbox[0] == startbyte){
        for(int i = 0; i < bitnum;i++){
        if(bitbox[1] == CheckBit[i]){
            Button[i] = true;
        }
        else{
            Button[i] = false;
        }
    }
    for(int i = 0; i < bitnum;i++){
        if(bitbox[2] == CheckBit[i]){
            Button[i + bitnum] = true;
        }
        else{
            Button[i + bitnum] = false;
        }
    }
    }
    else{
        for(int i = 0; i < buttinnum; i++){
            Button[i] = false;
        }
    }
}
 
void ESP32::intSerial(){
    /*val = esp.getc();
    if(val == 255){
       start = true;
       count = 1;
    }
    else{
        if(start){
            bitbox[count] = esp.getc();
            count++;
        }
        if(count == bytenum){
            Bitcheck();
            start = false;
            count = 0;
        }
    }*/
    if(esp.readable()){
        for(count = 0; count < bytenum; count++){
        bitbox[count] = esp.getc();
    }
    Bitcheck();
    }
}