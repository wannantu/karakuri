#include "omuni_3.h"

omuni_3::omuni_3(int speed_xy, int speed_rotate, float R){
    omu_3[0] = 1;
    omu_3[1] = 0;
    omu_3[2] = R;
    omu_3[3] = -0.5;
    omu_3[4] = 0.8660254;
    omu_3[5] = R;
    omu_3[6] = -0.5;
    omu_3[7] = -0.8660254;
    omu_3[8] = R;

    set_speed[0] = speed_xy;
    set_speed[1] = speed_xy;
    set_speed[2] = speed_rotate;
}

void omuni_3::cal_speed(int dir, float motor_speed[]){


    
    if(dir == 9 || dir == 10){
        for(int i = 0; i < 3; i++){
            motor_speed[i] = set_speed[2] * set_dir[dir][i];
        }
    }
    else{
        //Vx,Vy,Vθを決める
    for(int i = 0; i < 3; i++){
        s_speed[i] = set_speed[i] * set_dir[dir][i];
        //printf("s_speed[%d] = %f ", i, s_speed[i]);
    }
    //printf("\n");

    for(int i = 0; i < 3; i++){
        speed_tmp = 0.0;
        for(int j = 0; j < 3; j++){
        speed_tmp += s_speed[j] * omu_3[3*i + j];
        }
        //printf("%d = %f ",i, speed_tmp);
        motor_speed[i] = speed_tmp;
    }
    //printf("\n");
    }
}