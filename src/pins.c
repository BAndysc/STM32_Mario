#include "pins.h"
#include "device.h"

Pin NC = {.gpio = 0}; // NULL

Pin const PC_10 = {.gpio = GPIOC, .pin = 10};
Pin const PC_12 = {.gpio = GPIOC, .pin = 12};
Pin const PA_13 = {.gpio = GPIOA, .pin = 13};
Pin const PA_14 = {.gpio = GPIOA, .pin = 14};
Pin const PA_15 = {.gpio = GPIOA, .pin = 15};
Pin const PB_7 = {.gpio = GPIOB, .pin = 7};
Pin const PC_13 = {.gpio = GPIOC, .pin = 13};
Pin const PC_14 = {.gpio = GPIOC, .pin = 14};
Pin const PC_15 = {.gpio = GPIOC, .pin = 15};
Pin const PC_2 = {.gpio = GPIOC, .pin = 2};
Pin const PC_3 = {.gpio = GPIOC, .pin = 3};
Pin const PC_11 = {.gpio = GPIOC, .pin = 11};
Pin const PD_2 = {.gpio = GPIOD, .pin = 2};
Pin const PA_0 = {.gpio = GPIOA, .pin = 0};
Pin const PA_1 = {.gpio = GPIOA, .pin = 1};
Pin const PA_4 = {.gpio = GPIOA, .pin = 4};
Pin const PB_0 = {.gpio = GPIOB, .pin = 0};
Pin const PC_1 = {.gpio = GPIOC, .pin = 1};
Pin const PC_0 = {.gpio = GPIOC, .pin = 0};
Pin const PC_9 = {.gpio = GPIOC, .pin = 9};
Pin const PB_8 = {.gpio = GPIOB, .pin = 8};
Pin const PB_9 = {.gpio = GPIOB, .pin = 9};
Pin const PA_5 = {.gpio = GPIOA, .pin = 5};
Pin const PA_6 = {.gpio = GPIOA, .pin = 6};
Pin const PA_7 = {.gpio = GPIOA, .pin = 7};
Pin const PB_6 = {.gpio = GPIOB, .pin = 6};
Pin const PC_7 = {.gpio = GPIOC, .pin = 7};
Pin const PA_9 = {.gpio = GPIOA, .pin = 9};
Pin const PA_8 = {.gpio = GPIOA, .pin = 8};
Pin const PA_10 = {.gpio = GPIOA, .pin = 10};
Pin const PA_2 = {.gpio = GPIOA, .pin = 2};
Pin const PA_3 = {.gpio = GPIOA, .pin = 3};
Pin const PC_8 = {.gpio = GPIOC, .pin = 8};
Pin const PC_6 = {.gpio = GPIOC, .pin = 6};
Pin const PC_5 = {.gpio = GPIOC, .pin = 5};
Pin const PA_12 = {.gpio = GPIOA, .pin = 12};
Pin const PA_11 = {.gpio = GPIOA, .pin = 11};
Pin const PB_12 = {.gpio = GPIOB, .pin = 12};
Pin const PB_1 = {.gpio = GPIOB, .pin = 1};
Pin const PB_2 = {.gpio = GPIOB, .pin = 2};
Pin const PB_15 = {.gpio = GPIOB, .pin = 15};
Pin const PB_14 = {.gpio = GPIOB, .pin = 14};
Pin const PB_13 = {.gpio = GPIOB, .pin = 13};
Pin const PC_4 = {.gpio = GPIOC, .pin = 4};

void SetHigh(Pin pin)
{
	pin.gpio->BSRRH = (1 << pin.pin);
}

void SetLow(Pin pin)
{
	pin.gpio->BSRRL = (1 << pin.pin);
}

void SetPin(Pin pin, uint32_t low)
{
	if (low)
		SetLow(pin);
	else
		SetHigh(pin);
}

uint8_t PinsEq(Pin p1, Pin p2)
{
	return p1.gpio == p2.gpio && p1.pin == p2.pin;
}

void PinConfigureIn(Pin pin,
                     GPIOPuPd_TypeDef pull, EXTIMode_TypeDef mode,
                     EXTITrigger_TypeDef trigger)
{
	if (pin.gpio != 0)
	{
		GPIOClockEnable(pin);
		GPIOinConfigure(pin.gpio, pin.pin, pull, mode, trigger);
	}
}

void PinConfigureOut(Pin pin,
                      GPIOOType_TypeDef otype, GPIOSpeed_TypeDef speed,
                      GPIOPuPd_TypeDef pull)
{
	if (pin.gpio != 0)
	{
		GPIOClockEnable(pin);
		GPIOoutConfigure(pin.gpio, pin.pin, otype, speed, pull);
	}
}

void PinConfigureAlternativeFunc(Pin pin,
                     GPIOOType_TypeDef otype, GPIOSpeed_TypeDef speed,
                     GPIOPuPd_TypeDef pull, uint32_t fun)
{
	if (pin.gpio != 0)
	{
		GPIOClockEnable(pin);
		GPIOafConfigure(pin.gpio, pin.pin, otype, speed, pull, fun);
	}
}

void PinConfigureAnalogIn(Pin pin)
{
	if (pin.gpio != 0)
	{
		GPIOClockEnable(pin);
		GPIOainConfigure(pin.gpio, pin.pin);
	}
}