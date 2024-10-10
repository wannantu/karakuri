#ifndef MEKANAMU_4_H_
#define MEKANAMU_4_H_

#include "mbed.h"

enum div_b : int
{
    s = 0,
    f = 1,
    b = 2,
    l = 3,
    r = 4,
    rr = 5,
    rl = 6,
};

class mekanamu_4{
    public:
    mekanamu_4(int speed_xy, int speed_rotate, int lx, int ly);
    void cal_speed(int dir, float motor_speed[]);
    private:
    int speed_vector[3];
    //３，４列目はlx+ly
    int set_vector[4][3] = {    { 1,  1, 0}, //前
                                {-1,  1, 0}, //後ろ
                                {-1, -1, 0}, //左
                                { 1, -1, 0},
                            };
    int dir_m[7][3] = {   {0,  0,  0}, //止まる
                        {1,  0,  0}, //前
                        {-1,  0, 0}, //後ろ
                        {0,  1,  0}, //右
                        {0, -1,  0}, //左
                        {0,  0,  1}, //右回転
                        {0,  0, -1} //左回転
                        };
    int set_dir[3];
    float speed_tmp;
    int l_tmp;
};
#endif