#ifndef LEDS_H
#define LEDS_H

#include <gpio.h>
#include <stm32.h>
#include <stdbool.h>
#include <stdint.h>
#include "pins.h"

typedef enum 
{
	LED_LOW,
	LED_HIGH
} LedMode;

typedef struct
{
	Pin pin;
	int state;
	LedMode mode;
} Led;

typedef enum 
{
	LED_RED  = 0,
	LED_GREEN  = 1,
	LED_BLUE  = 2,
	LED_GREEN2  = 3
} LedType;

#define __LEDS 4

extern Led Leds[__LEDS];

void InitLeds();
void LedOn(LedType led);
void LedOff(LedType led);
void LedToggle(LedType led);

#endif