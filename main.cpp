#include "definitions.h"
#include "esp_task_wdt.h"

extern "C" void app_main()
{
    // Temporal para esta prueba con polling continuo.
    ESP_ERROR_CHECK(esp_task_wdt_deinit());

    DAC salida;
    salida.setup(DAC_PIN);

    digital_pot pot(CS_PIN, UD_PIN, INC_PIN);
    pot.setup();

    SimpleADC first;
    SimpleADC second;

    if (!first.setup(OP_ONE) || !second.setup(OP_TWO))
    {
        printf("Error al configurar los ADC\n");
        return;
    }

    SimpleGPIO button;
    button.setup(INI_BUTTON, GPIO_MODE_INPUT, GPIO_PULLUP_ONLY);

    Viscometer visco(first, second, salida, pot);
    visco.begin();

    State current_state = AIR, last_state = AIR;
    bool is_new_state = true;

    int64_t state_start_time = 0;
    int64_t last_print_time = 0;

    const int64_t AIR_DURATION = 60000000LL;
    const int64_t SOAP_DURATION = 60000000LL;
    const int64_t PRINT_INTERVAL = 500000LL;

    while (true)
    {
        visco.update();

        int64_t now = esp_timer_get_time();

        switch (current_state)
        {
            case AIR:
                if (is_new_state)
                {
                    state_start_time = now;

                    printf("Sonda en AIRE a 30 RPM.\n");
                    printf("Medicion en 60 segundos.\n");
                }

                if (now - state_start_time >= AIR_DURATION)
                {
                    visco.measureAir();
                    printf("Aire registrado.\n");

                    current_state = SOAP;
                }
                break;

            case SOAP:
                if (is_new_state)
                {
                    state_start_time = now;

                    printf("Coloca la sonda en JABON PURO a 30 RPM.\n");
                    printf("Medicion en 60 segundos.\n");
                }

                if (now - state_start_time >= SOAP_DURATION)
                {
                    visco.measureSoap();

                    printf("Ajustando. Manten la sonda en jabon.\n");

                    visco.adjustGain();
                    visco.adjustOffset();

                    current_state = READY;
                }
                break;

            case READY:
                if (is_new_state)
                {
                    printf("Rutina de calibracion terminada.\n");
                    printf("Pot: posicion %d\n", pot.get_Position());
                    printf("Coloca la muestra y espera estabilidad.\n");
                    printf("Presiona el boton para iniciar la prueba.\n");
                }

                if (button.get() == 0)
                {
                    current_state = MEASURE;
                }
                break;

            case MEASURE:
                if (is_new_state)
                {
                    last_print_time = now;
                    printf("Prueba iniciada.\n");
                }

                if (now - last_print_time >= PRINT_INTERVAL)
                {
                    last_print_time = now;

                    printf("Salida: %.3f V\n", visco.readOutput());
                }
                break;
        }

        if (current_state != last_state)
        {
            last_state = current_state;
            is_new_state = true;
        }
        else
        {
            is_new_state = false;
        }
    }
}