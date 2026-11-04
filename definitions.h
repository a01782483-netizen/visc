#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "VISCOMETER.h"
#include "SimpleGPIO.h"
#include "esp_timer.h"
#include <stdio.h>

#define DAC_PIN GPIO_NUM_25
#define CS_PIN  GPIO_NUM_18
#define UD_PIN  GPIO_NUM_19
#define INC_PIN GPIO_NUM_21

#define OP_ONE  GPIO_NUM_32
#define OP_TWO  GPIO_NUM_35
#define INI_BUTTON GPIO_NUM_15

enum State {
    AIR,
    SOAP,
    READY,
    MEASURE
};




#endif