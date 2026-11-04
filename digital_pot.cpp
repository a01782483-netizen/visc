#include "digital_pot.h"

digital_pot::digital_pot(gpio_num_t cs_pin,
                         gpio_num_t ud_pin,
                         gpio_num_t inc_pin)
{
    _cs_pin = cs_pin;
    _ud_pin = ud_pin;
    _inc_pin = inc_pin;
}

void digital_pot::setup()
{
    _cs.setup(_cs_pin, GPIO_MODE_OUTPUT);
    _cs.set(1);

    _inc.setup(_inc_pin, GPIO_MODE_OUTPUT);
    _inc.set(1);

    _ud.setup(_ud_pin, GPIO_MODE_OUTPUT);
    _ud.set(0);

    _position = -1;
    _state = IDLE;
}

bool digital_pot::home()
{
    if (isBusy()) {
        return false;
    }

    _target = 0;
    _remaining = 99;
    _up = false;

    _inc.set(1);
    _inicio = esp_timer_get_time();
    _state = PREPARE;

    return true;
}

bool digital_pot::set_Position(int position)
{
    if (isBusy() || _position < 0 ||
        position < 0 || position > 31) {
        return false;
    }

    int diferencia = position - _position;

    if (diferencia == 0) {
        return true;
    }

    _target = position;
    _up = diferencia > 0;

    if (diferencia > 0) {
        _remaining = diferencia;
    } else {
        _remaining = -diferencia;
    }

    _inc.set(1);
    _inicio = esp_timer_get_time();
    _state = PREPARE;

    return true;
}

void digital_pot::update()
{
    switch (_state)
    {
        case IDLE:
            return;

        case PREPARE:
            // INC lleva al menos 1 us en HIGH antes de cambiar U/D.
            if (esp_timer_get_time() - _inicio < 1) {
                return;
            }

            _ud.set(_up);
            _cs.set(0);

            _inicio = esp_timer_get_time();
            _state = INC_HIGH;
            break;

        case INC_HIGH:
            // Espera de HIGH y de preparación de U/D y CS.
            if (esp_timer_get_time() - _inicio < 1000) {
                return;
            }

            _inc.set(0);  // Flanco descendente: un paso
            _remaining--;

            _inicio = esp_timer_get_time();
            _state = INC_LOW;
            break;

        case INC_LOW:
            if (esp_timer_get_time() - _inicio < 1000) {
                return;
            }

            if (_remaining > 0) {
                _inc.set(1);

                _inicio = esp_timer_get_time();
                _state = INC_HIGH;
            } else {
                _cs.set(1);  // INC en LOW: no guarda

                _inicio = esp_timer_get_time();
                _state = RELEASE;
            }
            break;

        case RELEASE:
            if (esp_timer_get_time() - _inicio < 1) {
                return;
            }

            _inc.set(1);
            _position = _target;
            _state = IDLE;
            break;
    }
}

int digital_pot::get_Position()
{
    return _position;
}

bool digital_pot::isBusy()
{
    return _state != IDLE;
}