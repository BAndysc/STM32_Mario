#include "setup.h"
#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"
#include "debug.h"
#include "game/external.h"
#include "ILI9341.h"
#include "game/game.h"
#include "clock.h"

#define US_IN_1_MILISECOND 1000
#define UPDATE_TIMER_MS 30
#define LCD_WIDTH 320
#define LCD_HEIGHT 240

LCDt lcd;
Timer ticker;
USARTt serial;
Timer updateTimer;
Timer drawTimer;
ADCt adc;

uint8_t JoystickReadings[2];

int32_t MilisecondsSinceStart;

uint16_t LinesToDraw;
bool ShouldDraw;

bool ShouldUpdateWorld;

bool CanDraw;

void WorldUpdateTimer(void* data)
{
    ShouldUpdateWorld = true;
}

void DrawUpdateTimer(void* data)
{
    CanDraw = true;
}

void GlobalTimer(void* data)
{
    MilisecondsSinceStart++;
}

void SetupSerial()
{
    InitUsart(&serial, PA_2, PA_3, 9600U, UART_LENGTH_8b, UART_PARITY_NO, UART_STOP_BITS_1);
    DebugSetUsart(&serial);
    SetOut(&serial);
    SetIn(&serial);
}

void SetupMillisecondsTimer()
{
    //1000us - event every 1ms
    InitNextTimer16(&ticker, US_IN_1_MILISECOND, TIMER_DIRECTION_UP);
    ticker.bind(&ticker, &GlobalTimer, nullptr, INTERRUPT_PRIORITY_HIGHEST);
    ticker.start(&ticker);
}

void SetupDrawTimer()
{
    InitNextTimer32(&drawTimer, LCD_USECS_PER_LINE, TIMER_DIRECTION_UP);
    drawTimer.bind(&drawTimer, &DrawUpdateTimer, nullptr, INTERRUPT_PRIORITY_HIGHEST);
    drawTimer.start(&drawTimer);
}

void SetupUpdateWorldTimer()
{
    // update world every 30ms
    InitNextTimer16(&updateTimer, UPDATE_TIMER_MS * US_IN_1_MILISECOND, TIMER_DIRECTION_UP);
    updateTimer.bind(&updateTimer, &WorldUpdateTimer, nullptr, INTERRUPT_PRIORITY_HIGHEST);
    updateTimer.start(&updateTimer);
}

void SetupJoystick()
{
    InitADC(&adc);
    AddPinToADC(&adc, PA_4);
    AddPinToADC(&adc, PA_1);
    ADCStartContinous(&adc, ADC_RESOLUTION_6, JoystickReadings, INTERRUPT_PRIORITY_HIGH);

    SetJoystick(JoystickReadings);
}

static void RequestLine(LCDt* lcd, uint16_t* buffor, uint16_t startLine, uint16_t lines, uint16_t width)
{
    ShouldDraw = true;
    LinesToDraw = lines;
}

void SetupLCD()
{
    InitILI9341LCD(&lcd, LCD_WIDTH, LCD_HEIGHT, PA_7, PA_6, PA_5, PD_2, PC_11, PC_12, &RequestLine);
}

void MainLoopPass()
{
    if (ShouldUpdateWorld)
    {
        ShouldUpdateWorld = false;
        UpdateWorldLoop();
    }

    if (CanDraw && ShouldDraw)
    {
        ShouldDraw = false;
        CanDraw = false;
        RenderLine(&lcd, lcd.buffer, lcd.currentLine, LinesToDraw, lcd.width);
        lcd.SendGeneratedLines(&lcd);
    }
}