#ifndef VISCOMETER_H
#define VISCOMETER_H

#include "SimpleADC.h"
#include "DAC.h"
#include "digital_pot.h"

class Viscometer
{
public:
    Viscometer(SimpleADC &first, SimpleADC &second,
               DAC &dac, digital_pot &pot);

    void begin();
    void update();

    void measureAir();
    void measureSoap();
    void adjustGain();
    void adjustOffset();

    float readOutput();

private:
    SimpleADC &_first;
    SimpleADC &_second;
    DAC &_dac;
    digital_pot &_pot;

    static const float _gains[32];

    float _air = 0.0f;
    float _soap = 0.0f;
    float _gain = 20.05405f;
    float _minimum = 0.0f;

    float _readings[10] = {};
    float _sum = 0.0f;
    int _index = 0;
    int _count = 0;
};

#endif