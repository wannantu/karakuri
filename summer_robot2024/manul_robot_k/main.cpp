#include "mbed.h"
#include "kondo_1.1.h"
#include "brushless9.h"
#include "mekanamu_4.h"
#include "pid_f4.h"
#include "esp32_3.h"

//パラメータの設定(足回り)
const int SPEED_XY = 2000;
const int SPEED_ROTATE = 180;
const int gia = 19;
const int LX = 1;
const int LY = 3;

const double gp = 0.00005;
const double gi = 0.00006;
const double gd = -0.0000002;

//パラメータの設定(アーム)
const uint8_t LEFTHAND = 2;
const uint8_t RIGHTHAND = 3;
const uint16_t OAC = 100; //open and close
const int pos_arm_f = -35; //上下の目標ポイント
const int pos_arm_b = 0; //前後の目標ポイント
const int pos_arm_u = -80;
const int pos_arm_d = 0;

double arm_p = 0.0001;
double arm_i = 0.001;
double arm_d = -0.002;

//デバック
//ピン宣言(アクチュエータ)
BrushLess B(PA_11,PA_12);//ブラシレスモータ
RawSerial pc(USBTX,USBRX);

// main() runs in its own thread in the OS

//データの定義（アーム）
    PID_F arm_pid[] = {
        PID_F(arm_p, arm_i, arm_d),
        PID_F(arm_p, arm_i, arm_d)
    };


//パラメータの設定(アームの動き)
float rpm_set = 0;
bool is_move_f = false;
bool is_move_u = false;
int pos = 0;
int g = 20;
float speed = 0;
float rpm_set_d = 0;

Ticker utcicker;//アーム（割込み）
Ticker fticker;
void Tickerhandler_up(void);//アーム
void Tickerhandler_down(void);//アーム
void hand(void);


int main()
{
    //データの定義（足回り）
    mekanamu_4 namu4(SPEED_XY, SPEED_ROTATE, LX, LY);
    float motor_speed[4] = {0.0, 0.0, 0.0, 0.0};
    float sp[4] = {0.0, 0.0, 0.0, 0.0};
    int div = 0;
    bool absolute_zero = false;
    bool is_init_deviation = false;
    PID_F pidControllers[] = {
        PID_F(gp, gi, gd),
        PID_F(gp, gi, gd),
        PID_F(gp, gi, gd),
        PID_F(gp, gi, gd)
    };

    B.Init();/

    utcicker.attach(hand,0.01);
    //fticker.attach(Tickerhandler_down,0.07);

    //コンストラクト
    Kondo ks(PC_12, PD_2, PA_5, BAUDRATE::_115200bps);
    int degrre[9] = {3500, 4500, 5500, 6500, 7500, 8500, 9500, 10500, 11500};
    int deg = 0;
    int id = 2;

    //コントローラー
    ESP32 esp(PC_10, PC_11);
    esp.init(9600);
    int waittime = 20;


    while (true) {
        //足回り制御
        absolute_zero = false;
        is_init_deviation = false;
        if(esp.Button[Up]){
            div = 1;
        }
        else if(esp.Button[Down]){
            div = 2;
        }
        else if(esp.Button[Left]){
            div = 3;
        }
        else if(esp.Button[Right]){
            div = 4;
        }
        else if(esp.Button[R1]){
            div = 5;
        }
        else if(esp.Button[L1]){
            div = 6;
        }
        else{
            div = 0;
            absolute_zero = true;
            is_init_deviation = true;
        }

        namu4.cal_speed(div,motor_speed);
    
        for(int i = 0; i < 4; i++){
            sp[i] = pidControllers[i].set_input(absolute_zero, is_init_deviation, motor_speed[i], B.R[i].rpm, 0.02);
            B.SetSpeed(i, pidControllers[i].set_input(absolute_zero, is_init_deviation, motor_speed[i], B.R[i].rpm, 0.02));
        }
        B.Write();

        
        //デバック(足回り)
        pc.printf("div = %d ",div);
        pc.printf("gp = %f ",gp);
        for(int i = 0; i < 4;i++){
           pc.printf("speed[%d] = %f", i, motor_speed[i]);
        }
        //pc.printf("\n");

        //アームの前後
        if(esp.Button[Circle]){
            pos = pos_arm_f;
            is_move_f = true;
        }
        else if(esp.Button[Cross]){
            pos = pos_arm_b;
            is_move_f = true;
        }
        else if(esp.Button[Triangle]){
            pos = pos_arm_u;
            is_move_u = true;
        }
        else if(esp.Button[Square]){
            pos = pos_arm_d;
            is_move_u = true;
        }
        else{
            pos = pos;
            is_move_f = false;
            is_move_u = false;

        }
        pc.printf("pos_fb = %d ", B.Revolutions[4]);
        pc.printf("pos_ud = %d ", B.Revolutions[5]);
        //pc.printf("speed = %f arm_p = %f arm_i = %f are_d = %f move = %d arm = %d\n",speed,arm_p, arm_i, arm_d, is_move, B.Revolutions[5]);
        
        //アームの開閉
        if(esp.Button[R2]){
            id = 11;
            deg = degrre[6];
        }
        else if(esp.Button[L2]){
            id = 11;
            deg = degrre[7];
        }
        else if(esp.Button[L3]){
            id = 2;
            deg = degrre[4];
        }
        else if(esp.Button[R3]){
            id = 2;
            deg = degrre[3];
        }
        else{
            id = id;
            deg = deg;
        }
        //右　6 7
        //左　4 3

        
        ks.setposition270(id, deg);
        pc.printf("id = %d degrre = %d\n", id, deg);
        wait_ms(waittime);


    }
}

void Tickerhandler_up(void){
    rpm_set = 0.0;
    if(is_move_f){
        rpm_set = 20*(pos - B.Revolutions[4]);
    }
    else {
        rpm_set = 0.0;
    }
    speed = arm_pid[0].set_input(false, false, rpm_set, B.R[4].rpm, 0.01);
    B.SetSpeed(4, arm_pid[0].set_input(false, false, rpm_set, B.R[4].rpm, 0.01));
    B.Write();
}

void Tickerhandler_down(void){
    int i = 0;
    rpm_set_d = 0.0;
    if(is_move_u){
        rpm_set = 20*(pos - B.Revolutions[5]);
    }
    else {
        rpm_set = 0.0;
    }
    speed = arm_pid[0].set_input(false, false, rpm_set, B.R[5].rpm, 0.01);
    B.SetSpeed(5, arm_pid[0].set_input(false, false, rpm_set, B.R[5].rpm, 0.01));
    B.Write();
}



void hand(void){
    int i = 0;
    rpm_set = 0.0;
    if(is_move_f){
        rpm_set = 20*(pos - B.Revolutions[4]);
        i = 4;
    }
    else if(is_move_u){
        rpm_set = 20*(pos - B.Revolutions[5]);
        i = 5;
    }
    else {
        rpm_set = 0.0;
    }
    speed = arm_pid[0].set_input(false, false, rpm_set, B.R[i].rpm, 0.01);
    B.SetSpeed(i, arm_pid[i-4].set_input(false, false, rpm_set, B.R[i].rpm, 0.01));
    B.Write();
}

/*void Tickerhandler_down(void){
    int i = 0;
    rpm_set_d = 0.0;
    if(is_move_u){
        rpm_set = 20*(pos - B.Revolutions[5]);
    }
    else {
        rpm_set = 0.0;
    }
    speed = arm_pid[0].set_input(false, false, rpm_set, B.R[5].rpm, 0.01);
    B.SetSpeed(5, arm_pid[0].set_input(false, false, rpm_set, B.R[5].rpm, 0.01));
    B.Write();
}*/