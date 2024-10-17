#include "mbed.h"
 
// Read temperature from LM75BD

//SCL PB_8 SDA_PB_9
I2C i2c(PB_9 , PB_8); 
//const int addr_m = 0x48;      // 7 bit I2C address
const int addr_s = 1; // 8bit I2C address, 0x90
Serial pc(USBTX,USBRX);

int main() {
    char cmd[2] = {255, 0};
    int val;
    //i2c.frequency(100000);
    while (1) {
        cmd[0] -= 1;
        cmd[1] += 1;
        val = i2c.write(addr_s, cmd, 2);
        if(!val){
            pc.printf("message send\n");
        }
        else{
            pc.printf("msaagen not send\n");
        }
        wait_ms(500);
 
    }
}