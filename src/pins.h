#ifndef PINS_H
#define PINS_H

#include "gpio.h"
#include "stm32.h"

#define nullptr 0

typedef struct {
	GPIO_TypeDef* gpio;
	char pin;
} Pin;

extern Pin NC;

// left left
extern Pin const PC_10;
extern Pin const PC_12;
extern Pin const PA_13;
extern Pin const PA_14;
extern Pin const PA_15;
extern Pin const PB_7;
extern Pin const PC_13;
extern Pin const PC_14;
extern Pin const PC_15;
extern Pin const PC_2;
extern Pin const PC_3;

// left right
extern Pin const PC_11;
extern Pin const PD_2;
extern Pin const PA_0;
extern Pin const PA_1;
extern Pin const PA_4;
extern Pin const PB_0;
extern Pin const PC_1;
extern Pin const PC_0;

// right left
extern Pin const PC_9;
extern Pin const PB_8;
extern Pin const PB_9;
extern Pin const PA_5;
extern Pin const PA_6;
extern Pin const PA_7;
extern Pin const PB_6;
extern Pin const PC_7;
extern Pin const PA_9;
extern Pin const PA_8;
extern Pin const PA_10;
extern Pin const PA_2;
extern Pin const PA_3;

// right right
extern Pin const PC_8;
extern Pin const PC_6;
extern Pin const PC_5;
extern Pin const PA_12;
extern Pin const PA_11;
extern Pin const PB_12;
extern Pin const PB_1;
extern Pin const PB_2;
extern Pin const PB_15;
extern Pin const PB_14;
extern Pin const PB_13;
extern Pin const PC_4;


void SetHigh(Pin pin);
void SetLow(Pin pin);
void SetPin(Pin pin, uint32_t low);

uint8_t PinsEq(Pin p1, Pin p2);

void PinConfigureIn(Pin pin,
                     GPIOPuPd_TypeDef pull, EXTIMode_TypeDef mode,
                     EXTITrigger_TypeDef trigger);

void PinConfigureOut(Pin pin,
                      GPIOOType_TypeDef otype, GPIOSpeed_TypeDef speed,
                      GPIOPuPd_TypeDef pull);

void PinConfigureAlternativeFunc(Pin pin,
                     GPIOOType_TypeDef otype, GPIOSpeed_TypeDef speed,
                     GPIOPuPd_TypeDef pull, uint32_t fun);

void PinConfigureAnalogIn(Pin pin);

#endif