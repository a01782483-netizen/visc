#ifndef __DIGITAL_POT_H__
#define __DIGITAL_POT_H__

#include "SimpleGPIO.h"
#include <esp_timer.h>


class digital_pot
{
public:
    digital_pot(gpio_num_t cs_pin,
                gpio_num_t ud_pin,
                gpio_num_t inc_pin);

    void setup();
    void update();

    bool home();
    bool set_Position(int position);

    int get_Position();
    bool isBusy();

private:
    enum State {
        IDLE,
        PREPARE,
        INC_HIGH,
        INC_LOW,
        RELEASE
    };

    gpio_num_t _cs_pin;
    gpio_num_t _ud_pin;
    gpio_num_t _inc_pin;

    SimpleGPIO _cs;
    SimpleGPIO _ud;
    SimpleGPIO _inc;

    State _state = IDLE;

    int _position = -1; 
    int _target = 0;
    int _remaining = 0;

    bool _up = false;

    int64_t _inicio = 0;
};

#endif // __DIGITAL_POT_H__