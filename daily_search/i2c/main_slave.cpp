#include "mbed.h"
 
// Read temperature from LM75BD

//SCL PB_8 SDA_PB_9
I2CSlave i2c(PB_9 , PB_8); 
Serial pc(USBTX,USBRX);

//const int addr_m = 0x48;      // 7 bit I2C address
const int addr_s = 1; // 8bit I2C address, 0x90

int main() {
    char cmd[2];
    int val;
    i2c.frequency(100000);
    i2c.address(1);
    while (1) {
        val = i2c.read();
            for(int i = 0; i < 2;i++){
                pc.printf("cmd[%d] = %d",i ,cmd[i]);
            }
        pc.printf("\n");
        wait_ms(500);
 
    }
}