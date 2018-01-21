#include <stdbool.h>
#include <stdlib.h>
#include "queue.h"
#include "buttons.h"
#include "leds.h"
#include "uart.h"
#include "timer.h"
#include "i2c.h"
#include "lcd.h"
#include "spi.h"
#include "clock.h"
#include "game/game.h"
#include "device.h"
#include "adc.h"

extern int INPUT_JUMP;

void handler(Button_Event ev, void* data)
{
	INPUT_JUMP = (ev == BUTTON_EVENT_PRESSED) ? 1 : 0;
}


LCDt lcd;

USARTt serial;
extern int INPUT_ANALOG;

void myReadHandler(void* data, char* recv, uint8_t len)
{
	serial.write(&serial, recv);

	switch (recv[0])
	{
		case 't':
			INPUT_ANALOG = !INPUT_ANALOG;
			break;
		case 'a':
			INPUT_LEFT = 0;
			break;
		case 'd':
			INPUT_RIGHT = 0;
			break;
		case 'w':
			INPUT_JUMP = 0;
			break;
		case 'A':
			INPUT_LEFT = 1;
			break;
		case 'D':
			INPUT_RIGHT = 1;
			break;
		case 'W':
			INPUT_JUMP = 1;
			break;
	}
}

uint8_t JOYSTICK[2];

bool shouldUpdate = true;

void timeToUpdate()
{
	shouldUpdate = true;
}

int main() 
{
	Turn100MHzClock();

	//InitLeds();

	InitUsart(&serial, PA_2, PA_3, 9600U, UART_LENGTH_8b, UART_PARITY_NO, UART_STOP_BITS_1);
	UsartSetReadHandler(&serial, 1, myReadHandler, NULL);
	serial.write(&serial, "Hellw!\r\n");

	//InitUart(9600U, UART_LENGTH_8b, UART_PARITY_NO, UART_STOP_BITS_1);
	//InitButtons();


	//BindOnButtonEvent(BUTTON_FIRE, handler, NULL);

	InitGame();

	SetTimerHandler(upd);//timeToUpdate);
	InitTimer4();
	InitTimer3();

	ADCt adc;
	InitADC(&adc);
	AddPinToADC(&adc, PA_4);
	AddPinToADC(&adc, PA_1);
	ADCStartContinous(&adc, ADC_RESOLUTION_6, JOYSTICK, INTERRUPT_PRIORITY_HIGH);

	InitLCD(&lcd, 320, 240, PA_7, PA_6, PA_5, PD_2, PC_11, PC_12, &RenderLine);

	/*while (1)
	{
		serial.write(&serial, "I am alive!\r\n");
		for (int i = 0; i < 1000000; ++i);

	}*/

	while (1)
	{
		if (shouldUpdate)
		{
			shouldUpdate = false;
			upd();
		}
	}

}
