#include <fonts.h>
#include <gpio.h>
#include "lcd.h"
#include "debug.h"

#define INV(A) ((((A) & 0xFF)<<8) | ((A) >> 8))

#define LCD_DELAY_US 100000

#define LINES_TO_DRAW_AT_ONCE 20

static void SendCommand(LCDt* lcd, uint8_t command)
{
    SetHigh(lcd->A0);
    lcd->spi.writeAsync(&lcd->spi, (char*)&command, 1);
}

static void SendData8(LCDt* lcd, uint8_t data)
{
    lcd->spi.writeAsync(&lcd->spi, (char*)&data, 1);
}

static void SendData16(LCDt* lcd, uint16_t data)
{
    lcd->spi.writeAsync(&lcd->spi, (char*)&data, 2);
}

void DidSent(void* data);

static void SendNextInitializeInstruction(LCDt* lcd)
{
    SetLow(lcd->A0);
    LcdInitInstruction* instruction = &lcd->instructions[lcd->currentInitIndex];

    lcd->currentInitIndex++;

    switch (instruction->type)
    {
        case LCD_MSG_COMMAND:
            lcd->SendCommand(lcd, instruction->byte);
            break;

        case LCD_MSG_DATA:
            lcd->SendData8(lcd, instruction->byte);
            break;

        case LCD_MSG_DATA16:
            lcd->SendData16(lcd, instruction->byte);
            break;

        case LCD_MSG_DELAY:
            lcd->timer.startOnce(&lcd->timer);
            break;

        case LCD_MSG_WIDTH:
            lcd->SendData16(lcd, INV(lcd->width - 1));
            break;

        case LCD_MSG_HEIGHT:
            lcd->SendData16(lcd, INV(lcd->height - 1));
            break;

        case LCD_MSG_END:
            lcd->initialized = true;
            SPISetHandler(&lcd->spi, &DidSent, lcd);

            SPISetTranfserSize(&lcd->spi, SPI_TRANSFER_SIZE_HALF_WORD);
            lcd->currentLine = 0;
            lcd->renderer(lcd, lcd->buffer, lcd->currentLine, LINES_TO_DRAW_AT_ONCE, lcd->width);
            break;
    }
}

static void AfterDelay(void* data)
{
    LCDt* lcd = (LCDt*)data;
    SendNextInitializeInstruction(lcd);
}

void DidSentWhileInitialization(void* data)
{
    LCDt* lcd = (LCDt*)data;

    SendNextInitializeInstruction(lcd);
}

void DidSent(void* data)
{
    LCDt* lcd = (LCDt*)data;

    lcd->currentLine += LINES_TO_DRAW_AT_ONCE;
    if (lcd->currentLine == lcd->height)
        lcd->currentLine = 0;

    lcd->renderer(lcd, lcd->buffer, lcd->currentLine, LINES_TO_DRAW_AT_ONCE, lcd->width);
}

static void InitializeLCD(LCDt* lcd)
{
    SetLow(lcd->CS);
    SetHigh(lcd->CS);

    SendNextInitializeInstruction(lcd);
}

static void SendGeneratedLines(LCDt* lcd)
{
    lcd->spi.writeAsync(&(lcd->spi), (char*)lcd->buffer, lcd->width * LINES_TO_DRAW_AT_ONCE);
}

void InitLCD(LCDt* lcd, uint16_t width, uint16_t height, Pin mosi, Pin miso, Pin clock, Pin cs, Pin a0, Pin reset, LCDRenderLine requestLine, LcdInitInstruction* instructions)
{
    lcd->width = width;
    lcd->height = height;

    lcd->CS = cs;
    lcd->A0 = a0;
    lcd->Reset = reset;

    lcd->instructions = instructions;
    lcd->currentInitIndex = 0;
    lcd->initialized = false;

    lcd->SendCommand = &SendCommand;
    lcd->SendData16 = &SendData16;
    lcd->SendData8 = &SendData8;
    lcd->SendGeneratedLines = SendGeneratedLines;

    lcd->renderer = requestLine;

    PinConfigureOut(cs, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_NOPULL);
    PinConfigureOut(a0, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_NOPULL);
    PinConfigureOut(reset, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_NOPULL);

    SPI_Configuration configuration;
    configuration.BaudRatePrescaler = SPI_BAUD_DIV2;
    configuration.BidiMode = SPI_BIDI_MODE_1_LINE_BI;
    configuration.Direction = SPI_TRANSMIT_ONLY;
    configuration.ByteOrder = SPI_MOST_SIG_BYTE_FIRST;
    configuration.CalculateCRC = false;
    configuration.ClockPhase = SPI_SECOND_CLOCK_TRANSITION_IS_FIRST_DATA_CAPTURE_EDGE;
    configuration.ClockPolarity = SPI_CLOCK_1_WHEN_IDLE;
    configuration.InternalSlaveSelect = true;
    configuration.Role = SPI_ROLE_MASTER;
    configuration.TransferSize = SPI_TRANSFER_SIZE_BYTE;
    configuration.SlaveManagment = SPI_SLAVE_MANAGMENT_SOFTWARE;

    InitSPI(&lcd->spi, clock, mosi, miso, &configuration);
    SPISetHandler(&lcd->spi, &DidSentWhileInitialization, lcd);

    InitNextTimer32(&lcd->timer, LCD_DELAY_US, TIMER_DIRECTION_UP);
    lcd->timer.bind(&lcd->timer, AfterDelay, lcd, INTERRUPT_PRIORITY_HIGH);

    InitializeLCD(lcd);
}