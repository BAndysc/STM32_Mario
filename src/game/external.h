#ifndef MARIO_EXTERNAL_H
#define MARIO_EXTERNAL_H

#include "../uart.h"

void Puts(char* message);

void SetOut(USARTt* serial);

void SetIn(USARTt* serial);

void SetJoystick(uint8_t readings[]);

uint8_t* JoystickInput;

typedef enum {
    JOYSTICK_INPUT_HORIZ,
    JOYSTICK_INPUT_VERT
} JoystickInputAxis;

#endif //MARIO_EXTERNAL_H
