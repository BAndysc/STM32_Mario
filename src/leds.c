#include <gpio.h>
#include <stm32.h>
#include "leds.h"


Led Leds[__LEDS];

static void ConfigureLed(LedType led, Pin pin, LedMode mode)
{
	PinConfigureOut(pin,
		GPIO_OType_PP,
		GPIO_Low_Speed,
		GPIO_PuPd_NOPULL);	

	Leds[led].mode = mode;
	Leds[led].pin = pin;

	LedOff(led);
}

void InitLeds()
{
	//ConfigureLed(LED_RED, RED_LED_GPIO, RED_LED_PIN, LED_HIGH);
	//ConfigureLed(LED_BLUE, BLUE_LED_GPIO, BLUE_LED_PIN, LED_HIGH);
	//ConfigureLed(LED_GREEN, GREEN_LED_GPIO, GREEN_LED_PIN, LED_HIGH);
	ConfigureLed(LED_GREEN2, PA_5, LED_LOW);
}

void LedOn(LedType led)
{
	if (Leds[led].mode == LED_HIGH)
		SetHigh(Leds[led].pin);
	else
		SetLow(Leds[led].pin);

	Leds[led].state = 1;
}

void LedOff(LedType led)
{
	if (Leds[led].mode == LED_HIGH)
		SetLow(Leds[led].pin);
	else
		SetHigh(Leds[led].pin);

	Leds[led].state = 0;
}

void LedToggle(LedType led)
{
	if (Leds[led].state)
		LedOff(led);
	else
		LedOn(led);
}