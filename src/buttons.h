#ifndef BUTTONS_H
#define BUTTONS_H

#include <gpio.h>
#include <stm32.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum 
{
	LOW,
	HIGH
} Button_Mode;

typedef enum
{
	BUTTON_EVENT_PRESSED,
	BUTTON_EVENT_RELEASED
} Button_Event;

typedef enum 
{
	BUTTON_FIRE  = 0,
	BUTTON_UP    = 1,
	BUTTON_DOWN  = 2,
	BUTTON_LEFT  = 3,
	BUTTON_RIGHT = 4,
	BUTTON_MODE  = 5,
	BUTTON_USER  = 6,
	BUTTONS_END
} Button_Type;


typedef struct
{
	Button_Type button;
	uint32_t pin;
	GPIO_TypeDef* gpio;
	Button_Mode mode;
	int state;
	void (*handler)(Button_Event event, void*);
	void* handleData;
} Button;

void InitButtons();
void BindOnButtonEvent(Button_Type button, void (*handler)(Button_Event event, void* data), void* data);

extern Button Buttons[BUTTONS_END];

#endif