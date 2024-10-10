#include "mbed.h"
#include "mekanamu_4.h"

mekanamu_4::mekanamu_4(int speed_xy, int speed_rotate, int lx, int ly){
    speed_vector[0] = speed_vector[1] = speed_xy;
    speed_vector[2] = speed_rotate;
    l_tmp = lx + ly;
    for(int i = 0; i < 4; i++){
        set_vector[i][2] = l_tmp;
    }
}

void mekanamu_4::cal_speed(int dir, float motor_speed[]){
    
    for(int i = 0; i < 3; i++){
        set_dir[i] = dir_m[dir][i] * speed_vector[i];
    }

    for(int i = 0; i < 4;i++){
        speed_tmp = 0.0;
        for(int j = 0; j < 3; j++){
            speed_tmp += set_vector[i][j]*set_dir[j];
        }
        motor_speed[i] = speed_tmp;
    }
}