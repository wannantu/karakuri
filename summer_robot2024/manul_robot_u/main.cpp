#include "mbed.h"
#include "brushless9.h"
#include "pid_f4.h"
#include "omuni_3.h"
#include "esp32_3.h"

//パラメータの設定(アーム)
double p = 0.0011;
double I = 0.0002;
double d = -0.00005;
double rpm_set = 0.0;
const int GIA  = 16;
PID_F pid_arm(p,I,d); 
bool arm = false;

const int pos_down = 0;
const int pos_up = -30;
const int pos_set = -22;

//パラメータの設定(足回り)
const int SPEED_NM_PER_SECOND = 3500; //700*5  
const int ANGLE_SPEED = 1800; //180*10
const float RADIUS = 180.0;
const double gp = 0.00005;
const double gi = 0 /*0.00006*/;
const double gd = -0.0000002;

PID_F pidControllers[] = {
        PID_F(gp, gi, gd),
        PID_F(gp, gi, gd),
        PID_F(gp, gi, gd),
    };

//ピン宣言(アクチュエータ)
BrushLess B(PA_11,PA_12);//ブラシレスモータ
Serial pc(USBTX,USBRX);//シリアル通信(デバック)
ESP32 esp(PC_10, PC_11);//シリアル通信(コントローラー)

//処理、関数宣言
Ticker blTicker;//アーム（割込み）
void Tickerhandler(void);//アーム

//コンストラクト、変数定義(つかみ).global
bool is_move = false;//アームを動かすフラグ
int pos;
//float speed = 0.0;//デバック用(アームスピード)

int al = 1;//速度の倍速


float sp[3];

//超音波センサー
DigitalOut USSTriger (PA_7);   
InterruptIn USSEcho (PA_6);             
Timer ActiveTime;
Ticker TrigerTiming;
unsigned short USSDistance;         //USSDistance:超音波センサ測定距離
int dis_point = 8;


/* 関数宣言 */
void init(void);
void Output_Monitor(unsigned short Value);
// main() runs in its own thread in the OS
int main()
{
    //コンストラクト、変数定義(足回り)
    omuni_3 omu(SPEED_NM_PER_SECOND, ANGLE_SPEED, RADIUS);
    int md = 0;
    float motor_speed[3] = {0.0, 0.0, 0.0};
    bool absolute_zero = false;
    bool is_init_deviation = false;

    //コンストラクト、変数定義(コントローラー)
    esp.init(9600);
    int n = 0;//デバック

    //コンストラクト、変数定義(つかみ)
    //double speed = 0.0; //デバック用
    DigitalOut sw(PC_5);
    is_move = false;
    blTicker.attach(Tickerhandler,0.01);
    B.Init();
    init();
    bool light = 0;//つかみ機構の設定
    arm = true;

    while (true) {
        absolute_zero = false;
        is_init_deviation = false;
        n= 0;
        
        if(esp.Button[Up]){
            md = 1;
            //is_init_deviation = true;
            n = Up;
        }
        else if(esp.Button[Down]){
            md = 2;
            //is_init_deviation = true;
            n = Down;
        }
        else if(esp.Button[Right]){
            md = 3;
            //is_init_deviation = true;
            n = Right;
        }
        else if(esp.Button[Left]){
            md = 4;
            //is_init_deviation = true;
            n = Left;
        }
        else if(esp.Button[R1]){
            md = 9;
            //is_init_deviation = true;
            n = R1;
        }
        else if(esp.Button[L1]){
            md = 10;
            //is_init_deviation = true;
            n = L1;
        }
        else{
            md = 0;
            absolute_zero = true;
            is_init_deviation = true;
        }

        //加速度
        if(esp.Button[L2]){
            al = 3;
        }
        else if(esp.Button[Cross]){
            al = 2;
        }
        else if(esp.Button[R2]){
            al = (USSDistance - dis_point)/15.0;
        }
        else{
            al = 1;
        }

         omu.cal_speed(md, motor_speed);

        for(int i = 0; i < 3; i++){
            sp[i] = pidControllers[i].set_input(absolute_zero, is_init_deviation, al*motor_speed[i], B.R[i+1].rpm, 0.02);
            B.SetSpeed(i+1, sp[i]);
        }
        B.Write();
        
        /*for(int i = 0; i < 3; i++){
           printf("sp[%d] = %f B.R[%d].rpm = %d ",i, sp[i], i,B.R[i+1].rpm);
        }
        printf("\n");*/

        if(esp.Button[Start]){
            if(arm){
                light = !light;
                arm = false;
            }
            else{
                arm = false;
            }
        }
        else{
            arm = true;
            light = light;
        }
        sw.write(light);

        if(esp.Button[Circle]){
            is_move = true;
            pos = pos_set;
        }
        else if(esp.Button[Triangle]){
            is_move = true;
            pos = pos_up;
        }
        else if(esp.Button[Square]){
            is_move = true;
            pos = pos_down;
        }
        else{
            is_move = false;
            pos = pos;
        }
        //printf(" pos = %d",pos);

        //printf("dir = %d", md);
        /*for(int i = 0; i < 3; i++){
            //printf("motor_speed[%d] = %f ", i, motor_speed[i]);
            printf("set_speed[%d] = %f ", i, sp[i]);
            printf("motor[%d] = %d ", i+1, B.R[i+1].rpm);
        }*/
        //printf("led = %d ",light);
        //printf("ro = %d",B.Revolutions[4]);
        //printf("%d ",n);
        //printf("\n");
        /*for(int i = 0; i < 3; i++){
            printf("motor[%d] = %f ", i, al*motor_speed[i]);
        }*/
        //printf("md = %d ",md);
        //Output_Monitor( USSDistance );
        wait_ms(20);
    }
}

void Tickerhandler(void){
    rpm_set = 0.0;
    if(is_move){
        rpm_set = GIA*(100.0/30.0)*(pos - B.Revolutions[4]);
    }
    else {
    rpm_set = 0.0;
    }
    //speed = pid.set_input(false, false, rpm_set, B.R[0].rpm, 0.1);
    B.SetSpeed(4, pid_arm.set_input(false, false, rpm_set, B.R[4].rpm, 0.1));
    B.Write();
}

//超音波センサーの関数
void Triger (){
    //距離を読み込むとき超音波センサーに10usのhighレベルの入力が必要
    USSTriger = 1;
    wait_us(10);
    USSTriger = 0;
}


void RiseEcho(){
    //
    ActiveTime.start();
}

void FallEcho(){
    unsigned long ActiveWidth;
    ActiveTime.stop();
    ActiveWidth = ActiveTime.read_us();
    USSDistance = ActiveWidth * 0.0170;
    ActiveTime.reset();
}

void init(void){   
    //60ms秒間隔でTrigerという命令をする
    TrigerTiming.attach( Triger , 0.060 );      //USSTriger周期 60ms
    //USSEchoからhighが入力されたときにRiseEchoの命令をこなす
    USSEcho.rise( RiseEcho );                   //USSEcho立ち上がり時割り込み
     //USSEchoからlowが入力されたときにFallEchoの命令をこなす
    USSEcho.fall( FallEcho );                   //USSEcho立ち下がり時割り込み
}

/*void Output_Monitor(unsigned short Value){
    pc.printf("%d[cm]\n",Value);
}*/