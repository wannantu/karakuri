#ifndef OMUNI_3_H_
#define OMUNI_3_H_

/*#define div_s 0
#define div_f 1
#define div_b 2
#define div_l 3
#define div_r 4
#define div_fl 5
#define div_fr 6
#define div_bl 7
#define div_br 8
#define div_rr 9
#define div_rl 10*/

#include "mbed.h"
class omuni_3{
    public:
    omuni_3(int speed_xy, int speed_rotate,float R);
    void cal_speed(int dir, float motor_speed[]);
    int set_speed[3];
    float speed_set[3];
    float omu_3[9];
    private:
    int set_dir[11][3] = {  { 0,  0, 0}, //停止
                            { 0,  1, 0}, //前
                            { 0, -1, 0}, //後ろ
                            {-1,  0, 0}, //左
                            { 1,  0, 0}, //右
                            {-1,  1, 0}, //左前
                            { 1,  1, 0}, //右前
                            {-1, -1, 0}, //左後ろ
                            { 1,  1, 0}, //右後ろ
                            { 1,  1, 1}, //左回転
                            {-1, -1,-1}, //右回転
                           };
    float s_speed[3];
    float speed_tmp;
};
#endif