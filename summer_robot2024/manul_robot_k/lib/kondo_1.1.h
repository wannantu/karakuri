// kondo_1.h と比べて複数同時書き込みを追加

#ifndef KONDO_H
#define KONDO_H
#include "mbed.h"
#include <cstdint>

// 以下の定数は書き方を改める必要あり
constexpr int three = 3;
// 終わり

enum class BAUDRATE {
    _115200bps = 115200,
    _625000bps = 625000,
    _1250000bps = 1250000,
};

enum class ARG_RANGE {
    _other = 0,
    _270 = 270,
    _360 = 360,
};
 
enum class COMMAND{
    POSITION = 0b10000000,
    READ = 0b10100000,
    WRITE = 11000000,
    ID = 11100000,
};


class Kondo {
    RawSerial SERIAL;
    DigitalOut EN_IN;

    void turn_off_EN_IN(int value) { // callback の関係上 void(int) 型
        EN_IN.write(0);
        return;
    }

    bool checkID(uint8_t id) {
        if (id > 31) { return false; }
        else         { return true; }
    }

    bool makeCMD(uint8_t *CMD, COMMAND command, uint8_t id) {
        if (!checkID(id)) { return false; }
        *CMD = (int)command | id;
        return true;
    }

    bool checkPOS(uint16_t pos, ARG_RANGE arg_range) {
        switch ((int)arg_range) {
        case (int)ARG_RANGE::_270:
            if ((pos < 3500) || (11500 < pos)) { return false; };
            break;
        case (int)ARG_RANGE::_360:
            if (pos > 16383) { return false; }
            break;
        default:
            return false;
        }

        return true;
    }

    bool makePOS(uint8_t *POS, uint16_t pos, ARG_RANGE arg_range) {
        if (!checkPOS(pos, arg_range)) { return false; }

        POS[1] = uint8_t(pos >> 7);
        POS[2] = uint8_t(pos & 0b1111111);
        return true;
        
    }
    
public:
    uint8_t bufrom[6];
    Kondo(PinName tx, PinName rx, PinName en_in, BAUDRATE baudrate)
    : SERIAL(tx, rx, (int)baudrate), EN_IN(en_in, 0), debug(USBTX, USBRX) {
        SERIAL.format(8, SerialBase::Even, 1);
        //SERIAL.attach(Callback<void()>(this, &Kondo::read));
        
    }

    
    void send(uint8_t *buf, uint8_t length) {
        // 位置も自分だけ待って勝手に切れる
        /*EN_IN.write(1);
        for (int i = 0; i < length; i++) {
            SERIAL.putc(buf[i]);
        }
        wait_us(300);
        EN_IN.write(0);*/
        // はじめに EN_IN が素早く入り切りして、失敗
        /*for (int i = 0; i < length; i++) {
            EN_IN.write(1);
            SERIAL.putc(buf[i]);
            EN_IN.write(0);
        }*/
        EN_IN.write(1);
        //SERIAL.write(buf, length, Callback<void(int)>(this,(&Kondo::turn_off_EN_IN)));
        for (int i = 0; i < length; i++) {
            SERIAL.putc(buf[i]);
        }
    }

    int setid(uint8_t id) {
        uint8_t buf[4] = {0b11100000, 0, 0, 0};
        if (id > 31) {
            return -1;
        }
        buf[0] += id;
        send(buf, 4);
        return 0;
    }

    int setposition270(uint8_t id, uint16_t position) {
        if (id > 31) {
            return -1; // overrange
        };
        if ((position < 3500) || (11500 < position)) {
            return -1; // overrange
        }
        uint8_t buf[3] = {
            uint8_t(0b10000000 | id),
            uint8_t(position >> 7),
            uint8_t(position & 0b1111111)
        };
        send(buf, 3);
        return 0;
    }

    int setposition270_array(int index, uint8_t id[], uint16_t position[]) {
        int onecycle = 7;
        for (int i = 0; i < index; i++) {
            if (id[i] > 32) {
                return -1;
            }
            if ((position[i] < 3500) || (11501 < position[i])) {
            return -1; // overrange
            }
        }
        uint8_t *buf = new uint8_t[index * onecycle];
        for (int i = 0; i < index; i++) {
            buf[onecycle * i + 0] = 0b10000000 | id[i];
            buf[onecycle * i + 1] = position[i] >> 7;
            buf[onecycle * i + 2] = position[i] & 0b1111111;
            for (int j = 3; j < onecycle; j++) {
                buf[onecycle * i + j] = 0;
            }
        }
        send(buf, 3 * index);
        delete[] buf;
        return 0;
    }
    
    bool set(COMMAND command, uint8_t id, uint16_t value = 0, ARG_RANGE arg_range = ARG_RANGE::_other) {
        uint8_t buffer[127];
        if (makeCMD(buffer, command, id)) { return false; }

        switch ((int)command) {
        case (int)COMMAND::POSITION:
            if (!makePOS(buffer, value, arg_range)) { return false; }
            send(buffer, three);
            break;
        case (int)COMMAND::READ:
            break;
        case (int)COMMAND::WRITE:
            break;
        case (int)COMMAND::ID:
            break;
        
        }
        return true;
    }

    void getid() {
        uint8_t buf[4] = {0xff, 0x00, 0x00, 0x00};
        send(buf, 4);
    }

    void read(uint8_t *buf, uint8_t length) {
        for (int i = 0; i < length; i++) {
            if (SERIAL.readable()) {
                bufrom[i] = SERIAL.getc();
            }
        }
        return;
    }
    RawSerial debug;
};


#endif  // KONDO_H