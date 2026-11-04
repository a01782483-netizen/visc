#ifndef __DAC_H__
#define __DAC_H__

#include "driver/dac_oneshot.h"
#include <stdint.h>
#include "driver/gpio.h"


class DAC
{
    public:
    DAC();
    void setup(gpio_num_t dac_pin);
    void setValue(uint8_t value);
    void setVoltage(float voltage);
    float getVoltage();

    private:
    dac_oneshot_handle_t _handle;
    gpio_num_t _dac_pin;
    uint8_t _value;
    float _voltage;

};


#endif // __DAC_H__