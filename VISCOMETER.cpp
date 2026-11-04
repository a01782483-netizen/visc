#include "VISCOMETER.h"

const float Viscometer::_gains[32] = {
    37.83784f, 35.13514f, 33.78378f, 31.89189f,
    30.27027f, 28.91892f, 27.56757f, 26.29730f,
    25.35135f, 24.24324f, 23.67568f, 22.72973f,
    22.00000f, 21.64865f, 20.89189f, 20.05405f,
    19.45946f, 19.08108f, 18.59459f, 18.27027f,
    17.72973f, 17.24324f, 16.81081f, 16.35135f,
    16.08108f, 15.62162f, 15.24324f, 14.94595f,
    14.72973f, 14.43243f, 14.05405f, 13.86486f};

Viscometer::Viscometer(SimpleADC &first, SimpleADC &second,
                       DAC &dac, digital_pot &pot)
    : _first(first), _second(second), _dac(dac), _pot(pot)
{
}

void Viscometer::begin()
{
    _pot.home();

    while (_pot.isBusy())
    {
        update();
    }

    _pot.set_Position(15);

    while (_pot.isBusy())
    {
        update();
    }

    _gain = _gains[15];
}

void Viscometer::update()
{
    _pot.update();
}

void Viscometer::measureAir()
{
    _air = _first.read(ADC_READ_MV) / 1000.0f / _gains[15];
}

void Viscometer::measureSoap()
{
    _soap = _first.read(ADC_READ_MV) / 1000.0f / _gains[15];
}

void Viscometer::adjustGain()
{
    // La mezcla representa el 13.51 % del rango aire-jabón.
    const float mixture_fraction = 0.135135f;

    _minimum = _air + mixture_fraction * (_soap - _air);

    float diferencia = _soap - _minimum;

    if (diferencia <= 0.0f)
    {
        return;
    }

    // Ganancia necesaria del primer OPAMP.
    float required_gain = 3.0f / (10.0f * diferencia);

    if (required_gain < _gains[31] ||
        required_gain > _gains[0])
    {
        return;
    }

    for (int position = 0; position < 32; position++)
    {
        if (_gains[position] <= required_gain)
        {
            _pot.set_Position(position);

            while (_pot.isBusy())
            {
                update();
            }

            _gain = _gains[position];
            break;
        }
    }
}

void Viscometer::adjustOffset()
{
    // Mantener la sonda en jabón puro.
    // Esperar 2 segundos y tomar 3 muestras separadas por 100 ms.
    float sum = 0.0f;
    int samples = 0;

    int64_t last_time = esp_timer_get_time();
    int64_t interval = 2000000LL;

    while (samples < 3)
    {
        update();

        if (esp_timer_get_time() - last_time >= interval)
        {
            sum += _first.read(ADC_READ_MV) / 1000.0f;
            samples++;

            last_time = esp_timer_get_time();
            interval = 100000LL;
        }
    }

    float soap_voltage = sum / 3.0f;

    // Estimar el voltaje del primer OPAMP para la mezcla.
    // Ese será el DAC para que la mezcla produzca 0 V.
    float dac_voltage =
        soap_voltage - _gain * (_soap - _minimum);


    if (dac_voltage < 0.0f || dac_voltage > 3.3f)
    {
        return;
    }

    _dac.setVoltage(dac_voltage);
}

float Viscometer::readOutput()
{
    float voltage = _second.read(ADC_READ_MV) / 1000.0f;

    _sum -= _readings[_index];
    _readings[_index] = voltage;
    _sum += voltage;

    _index = (_index + 1) % 10;

    if (_count < 10)
    {
        _count++;
    }

    return _sum / _count;
}