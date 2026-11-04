#include <DAC.h>

DAC::DAC()
{
    _handle = nullptr;
    _dac_pin = GPIO_NUM_NC;
    _value = 0;
    _voltage = 0.0f;
}

void DAC::setup(gpio_num_t dac_pin)
{
    _dac_pin = dac_pin;
    dac_oneshot_config_t config = {};

    if (_dac_pin == GPIO_NUM_25)
    {
        config.chan_id = DAC_CHAN_0;
    }
    else if (_dac_pin == GPIO_NUM_26)
    {
        config.chan_id = DAC_CHAN_1;
    }
    else
    {
        ESP_ERROR_CHECK(ESP_ERR_INVALID_ARG);
        return;
    }

    ESP_ERROR_CHECK(dac_oneshot_new_channel(&config, &_handle));

    setValue(0);
}

void DAC::setValue(uint8_t value)
{
    dac_oneshot_output_voltage(_handle, value);
    _value = value;
    _voltage = _value * 3.3f / 255.0f;
}

void DAC::setVoltage(float voltage)
{
    if (voltage > 3.3f)
    {
        voltage = 3.3f;
    }
    else if (voltage < 0.0f)
    {
        voltage = 0.0f;
    }
    uint8_t value = (uint8_t)(voltage * 255.0f / 3.3f + 0.5f);
    setValue(value);
}

float DAC::getVoltage()
{
    return _voltage;
}
