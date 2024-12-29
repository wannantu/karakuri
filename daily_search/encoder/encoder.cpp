#include "mbed.h"
 
class encoder{
    public:
        encoder(PinName channelA, PinName channelB, double resolution);
        void count_puls();
        int counter;
        double pulsesPerRev;
        double getrpm();
        void start();
        void init();
        double rpm;
        double prevalue;
        double corvalue;

    private:
    Timer Mper;
    InterruptIn chaanleA;
    InterruptIn chaanleB;
};

encoder::encoder(PinName channelA, PinName channelB, double resolution): chaanleA(channelA), chaanleB(channelB){
    pulsesPerRev = resolution;
    counter = 0;
    chaanleA.rise(callback(this, &encoder::count_puls));
    prevalue = 0;
}



void encoder::count_puls(){
    int flag = chaanleB.read();
    if(!flag){
        counter += 1;
    }
    else{
        counter -= 1;
    }

}

double encoder::getrpm(){
    Mper.stop();
    corvalue = counter/pulsesPerRev;
    rpm = (corvalue - prevalue)/(Mper.read()/60);
    Mper.reset();
    Mper.start();
    prevalue = corvalue;  
    return rpm;
}

void encoder::start(){
    Mper.start();
}


static UnbufferedSerial serial_port(USBTX, USBRX);



int main() {
    int ppr = 512;
    encoder enc(PA_0,PA_1,ppr);
    enc.start();
    char c[256];
    double rpm;
    int count = 0;
    while (1) {
        rpm = enc.getrpm();
        int size = sprintf(c,"rpm = %f \n",rpm);//ここでいつもの書き方
        serial_port.write(&c, size);
        wait_us(500000);
 
    }
}