// © Y. Morinaga 2023

// R051014
// setpoint を変えたときに initdeviation するかどうかを指定できるよう変更
// R051028
// set_input の引数名 time_difference を time_difference_s に変更
// R051031
// 従来 setinput からしか得られなかった input の値を getinput から得られる。
#ifndef PID_F_H
#define PID_F_H

class PID_F {
    float gain_p;
    float gain_i;
    float gain_d;
    float deviation_p;
    float deviation_i;
    float deviation_d;
    float setpoint_previous;
    float input;
    void init_deviation() {
        deviation_p = deviation_i = deviation_d = .0f;
    }
public:
    PID_F (
        float gain_p_, float gain_i_, float gain_d_,
        float deviation_p_ = .0f, float deviation_i_ = .0f, float deviation_d_ = .0f
    ) : gain_p(gain_p_), gain_i(gain_i_), gain_d(gain_d_),
        deviation_p(.0f), deviation_i(0.f), deviation_d(.0f),
        setpoint_previous(.0f), input(.0f) {
    }

    float get_input() {return input;}

    float set_input(bool absolute_zero, bool is_init_deviation, float setpoint, float process_value, float time_defference_s) {
        if (absolute_zero) {
            init_deviation();
            return .0f;
        }

        if (is_init_deviation) {
            if (setpoint != setpoint_previous) {
                init_deviation();
            }
        }

        setpoint_previous = setpoint;
        
        float deviation_new = setpoint - process_value;

        deviation_d = (deviation_new - deviation_p) / time_defference_s;
        deviation_i += deviation_new * time_defference_s;
        deviation_p = deviation_new;

        input = gain_p * deviation_p + gain_i * deviation_i + gain_d * deviation_d;
        

        return input;
    }
};

#endif //PID_F_H
// © Y. Morinaga 2023