#include "mbed.h"
 
class encoder{
    public:
        encoder(PinName channelA, PinName channelB, int resolution);
        void count_puls();
        int counter;
        int pulsesPerRev;
        double getrpm();
        void start();
        void init();
        double rpm;
        int prevalue;
        int corvalue;

    private:
    Timer Mper;
    InterruptIn chaanleA;
    InterruptIn chaanleB;
};

encoder::encoder(PinName channelA, PinName channelB, int resolution): chaanleA(channelA), chaanleB(channelB){
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


Serial pc(USBTX,USBRX);
InterruptIn Asou(PA_0);
InterruptIn Bsou(PA_1);

int counter = 0;
int flag = 0;

int main() {
    //char cmd[2];
    //int val;
    encoder enc(PA_0,PA_1,400);
    enc.start();
    //i2c.frequency(100000);
    //i2c.address(1);
    double rpm;
    int count = 0;
    while (1) {
        rpm = enc.getrpm();
        //count = enc.counter;
        pc.printf("counter = %f\n",rpm);
        wait_ms(500);

 
    }
}