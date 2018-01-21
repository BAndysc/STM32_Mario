#include <gpio.h>
#include <stm32.h>
#include "buttons.h"
#include "interrupts.h"
#include <stdlib.h>
#include "leds.h"

Button Buttons[BUTTONS_END];

// BUTTONS
#define BUTTON_FIRE_GPIO  GPIOB
#define BUTTON_UP_GPIO  GPIOB
#define BUTTON_DOWN_GPIO  GPIOB
#define BUTTON_LEFT_GPIO  GPIOB
#define BUTTON_RIGHT_GPIO  GPIOB
#define BUTTON_MODE_GPIO  GPIOA
#define BUTTON_USER_GPIO  GPIOC

#define BUTTON_FIRE_PIN 10
#define BUTTON_UP_PIN 5
#define BUTTON_DOWN_PIN 6
#define BUTTON_LEFT_PIN 3
#define BUTTON_RIGHT_PIN 4
#define BUTTON_MODE_PIN 0
#define BUTTON_USER_PIN 13

// "private: "

static bool IsButtonPressed(Button_Type button)
{
	uint32_t value = Buttons[button].gpio->IDR & (1 << Buttons[button].pin);

	if (Buttons[button].mode == HIGH)
		return value == 0;

	return value != 0;
}

static void Triggered(Button_Type button)
{
	bool state = IsButtonPressed(button);
	if (Buttons[button].state != state)
	{
		Buttons[button].state = state;
		if (Buttons[button].handler)
			Buttons[button].handler(state ? BUTTON_EVENT_PRESSED : BUTTON_EVENT_RELEASED, Buttons[button].handleData);	
	}
}

static void HandleButtonInterrupt(void* data)
{
	Button_Type* type = (Button_Type*)data;
	Triggered(*type);
}

static void _InitButton(int button, GPIO_TypeDef* gpio, int pin, Button_Mode mode)
{
	Buttons[button].button = button;
	Buttons[button].pin = pin;
	Buttons[button].gpio = gpio;
	Buttons[button].mode = mode;
	Buttons[button].state = 2; // other than false (0) (released) and true (1) (pressed)
	Buttons[button].handler = NULL;

	GPIOinConfigure(gpio, pin, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);

	AddInterruptHandlerManualBit(INTERRUPT_EXTI, 1 << pin, 1 << pin, INTERRUPT_PRIORITY_HIGH, &HandleButtonInterrupt, &Buttons[button].button);
}

static void TurnClockOn()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN  | RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}

// public:

void BindOnButtonEvent(Button_Type button, void (*handler)(Button_Event event, void* data), void* data)
{
	Buttons[button].handler = handler;
	Buttons[button].handleData = data;
}

void InitButtons()
{
	TurnClockOn();

	_InitButton(BUTTON_FIRE, BUTTON_FIRE_GPIO, BUTTON_FIRE_PIN, HIGH);
	//_InitButton(BUTTON_UP, BUTTON_UP_GPIO, BUTTON_UP_PIN, HIGH);
	//_InitButton(BUTTON_DOWN, BUTTON_DOWN_GPIO, BUTTON_DOWN_PIN, HIGH);
	//_InitButton(BUTTON_LEFT, BUTTON_LEFT_GPIO, BUTTON_LEFT_PIN, HIGH);
	//_InitButton(BUTTON_RIGHT, BUTTON_RIGHT_GPIO, BUTTON_RIGHT_PIN, HIGH);
	//_InitButton(BUTTON_MODE, BUTTON_MODE_GPIO, BUTTON_MODE_PIN, LOW);
	_InitButton(BUTTON_USER, BUTTON_USER_GPIO, BUTTON_USER_PIN, HIGH);
}