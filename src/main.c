#include <stdbool.h>
#include <stdlib.h>
#include "queue.h"
#include "uart.h"
#include "timer.h"
#include "lcd.h"
#include "spi.h"
#include "clock.h"
#include "game/game.h"
#include "device.h"
#include "adc.h"
#include "debug.h"
#include "ILI9341.h"

LCDt lcd;
Timer ticker;
USARTt serial;
Timer updateTimer;
ADCt adc;

void myReadHandler(void* data, char const* recv, uint8_t len)
{
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
        default:
            break;
    }
}

uint8_t JOYSTICK[2];

bool shouldUpdate = true;

bool ShouldDraw;

void timeToUpdate(void* data)
{
    shouldUpdate = true;
}

int32_t TICKS = 0;

void GlobalTimer(void* data)
{
    TICKS++;
}

static void SetupSerial()
{
    InitUsart(&serial, PA_2, PA_3, 9600U, UART_LENGTH_8b, UART_PARITY_NO, UART_STOP_BITS_1);
    UsartSetReadHandler(&serial, 1, myReadHandler, NULL);
    DebugSetUsart(&serial);
    serial.write(&serial, "Welcome to Super Mario Bros\r\n");
}

static void SetupTicker()
{
    // 100us - event every 0.1ms
    InitNextTimer16(&ticker, 100, TIMER_DIRECTION_UP);
    ticker.bind(&ticker, &GlobalTimer, NULL, INTERRUPT_PRIORITY_HIGHEST);
    ticker.start(&ticker);
}

static void SetupUpdateWorldTimer()
{
    // update world every 30ms
    InitNextTimer16(&updateTimer, 30000, TIMER_DIRECTION_UP);
    updateTimer.bind(&updateTimer, &timeToUpdate, NULL, INTERRUPT_PRIORITY_HIGHEST);
    updateTimer.start(&updateTimer);
}

static void SetupJoystick()
{
    InitADC(&adc);
    AddPinToADC(&adc, PA_4);
    AddPinToADC(&adc, PA_1);
    ADCStartContinous(&adc, ADC_RESOLUTION_6, JOYSTICK, INTERRUPT_PRIORITY_HIGH);
}

static void RequestLine(LCDt* lcd, uint16_t* buffor, uint16_t startLine, uint16_t lines, uint16_t width)
{
    ShouldDraw = true;
}

static void MainLoop()
{
    int32_t lastDraw = 0;
    while (1)
    {
        if (shouldUpdate)
        {
            shouldUpdate = false;
            UpdateWorldLoop();
        }

        if (ShouldDraw && (TICKS - lastDraw) >= LCD_USECS_PER_LINE)
        {
            lastDraw = TICKS;
            ShouldDraw = false;
            RenderLine(&lcd, lcd.buffer, lcd.currentLine, 20, lcd.width);
            lcd.spi.writeAsync(&(lcd.spi), (char*)lcd.buffer, lcd.width * 20);
        }

        SleepAndWaitForInterrupts();
    }
}

int main()
{
    Turn100MHzClock();

    SetupSerial();

    SetupTicker();

    SetupUpdateWorldTimer();

    InitGame();

    SetupJoystick();

    InitILI9341LCD(&lcd, 320, 240, PA_7, PA_6, PA_5, PD_2, PC_11, PC_12, &RequestLine);

    MainLoop();
}
