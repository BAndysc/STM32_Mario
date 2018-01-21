#include <delay.h>
#include <fonts.h>
#include <gpio.h>
#include "lcd.h"
#include "interrupts.h"
#include "spi.h"
#include "game/game.h"
#include "debug.h"

/* Needed delay(s)	*/

#define Tinit	 150
#define T120ms	(MAIN_CLOCK_MHZ * 120000 / 4)

#define TDelay 800000

#define INV(A) ((((A) & 0xFF)<<8) | ((A) >> 8))

static void SendCommand(LCDt* lcd, uint8_t command)
{
	SetHigh(lcd->A0);
	lcd->spi.write8(&lcd->spi, command);
	SetLow(lcd->A0);
}

static void SendData8(LCDt* lcd, uint8_t data)
{
    // A0(1); is already set
	lcd->spi.write8(&lcd->spi, data);
}

static void SendData16(LCDt* lcd, uint16_t data)
{
    // A0(1); is already set
	lcd->spi.write16(&lcd->spi, data);
}

void Rescale()
{
/*    if (SCALE == 2)
    {
        lcd->SendCommand(lcd, 0x2A);
        lcd->SendData8(lcd, 0 >> 8);
        lcd->SendData8(lcd, 0);
        lcd->SendData8(lcd, (0 + 320 - 1) >> 8);
        lcd->SendData8(lcd, (0 + 320 - 1) & 0xFF);

    // CS(1);
        lcd->SendCommand(lcd, 0x2B);
        lcd->SendData8(lcd, 0 >> 8);
        lcd->SendData8(lcd, 0);
        lcd->SendData8(lcd, (0 + 240 - 1) >> 8);
        lcd->SendData8(lcd, 0 + 240 - 1);
    // CS(1);
        lcd->SendCommand(lcd, 0x2C);
    }
    else
    {
        lcd->SendCommand(lcd, 0x2A);
        lcd->SendData8(lcd, 80 >> 8);
        lcd->SendData8(lcd, 80);
        lcd->SendData8(lcd, (0 + 240 - 1) >> 8);
        lcd->SendData8(lcd, (0 + 240 - 1) & 0xFF);

    // CS(1);
        lcd->SendCommand(lcd, 0x2B);
        lcd->SendData8(lcd, 60 >> 8);
        lcd->SendData8(lcd, 60);
        lcd->SendData8(lcd, (0 + 180 - 1) >> 8);
        lcd->SendData8(lcd, 0 + 180 - 1);
    // CS(1);
        lcd->SendCommand(lcd, 0x2C);       
    }*/
}

extern uint32_t TICKS;
uint32_t PREVIOUS_TICKS;

#define LINES 20
void DidSent(void* data) {
	LCDt* lcd = (LCDt*)data;
	lcd->currentLine += LINES;
	if (lcd->currentLine == lcd->height)
	{
		lcd->currentLine = 0;
        AfterRender();
        ///uint32_t diff = TICKS - PREVIOUS_TICKS;
        ///UartSendInt((1000/diff));
        ///UartSend("\r\n");
        ///PREVIOUS_TICKS = TICKS;
        Debug("het\r\n");
	}
	lcd->renderer(lcd, lcd->buffer, lcd->currentLine, LINES, lcd->width);
	lcd->spi.writeAsync(&lcd->spi, (char*)lcd->buffer, lcd->width * LINES);
}

/*void LCDsetRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	lcd->SendCommand(lcd, 0x2A);
	lcd->SendData16(lcd, x1);
	lcd->SendData16(lcd, x2);

	lcd->SendCommand(lcd, 0x2B);
	lcd->SendData16(lcd, y1);
	lcd->SendData16(lcd, y2);

	lcd->SendCommand(lcd, 0x2C);
}*/


static void InitializeLCD(LCDt* lcd) {
	SetLow(lcd->CS);
	SetLow(lcd->A0);
	SetHigh(lcd->Reset);
	SetLow(lcd->Reset);

	SetHigh(lcd->CS);
	lcd->SendCommand(lcd, ILI9341_COMMAND_SOFTWARE_RESET);
	lcd->SendCommand(lcd, ILI9341_COMMAND_DISPLAY_OFF);

	/* Start Initial Sequence ----------------------------------------------------*/
	lcd->SendCommand(lcd, ILI9341_COMMAND_POWER_CONTROL_B);
	lcd->SendData8(lcd, 0x00); // always 0
	lcd->SendData8(lcd, 0x81);
	lcd->SendData8(lcd, 0x30);

	lcd->SendCommand(lcd, ILI9341_COMMAND_POWER_ON_SEQUENCE_CONTROL);
	lcd->SendData8(lcd, 0x64);
	lcd->SendData8(lcd, 0x03);
	lcd->SendData8(lcd, 0x12);
	lcd->SendData8(lcd, 0x81);

	lcd->SendCommand(lcd, ILI9341_COMMAND_DIVER_TIMING_CONTROL_A);
	lcd->SendData8(lcd, 0x85);
	lcd->SendData8(lcd, 0x01);
	lcd->SendData8(lcd, 0x79);

	lcd->SendCommand(lcd, ILI9341_COMMAND_POWER_CONTROL_A);
	lcd->SendData8(lcd, 0x39);
	lcd->SendData8(lcd, 0x2C);
	lcd->SendData8(lcd, 0x00);
	lcd->SendData8(lcd, 0x34);
	lcd->SendData8(lcd, 0x02);

	lcd->SendCommand(lcd, ILI9341_COMMAND_PUMP_RADIO_CONTROL);
	lcd->SendData8(lcd, 0x20);

	lcd->SendCommand(lcd, ILI9341_COMMAND_DIVER_TIMING_CONTROL_B);
	lcd->SendData8(lcd, 0x00);
	lcd->SendData8(lcd, 0x00);

	lcd->SendCommand(lcd, ILI9341_COMMAND_POWER_CONTROL_1);
	lcd->SendData8(lcd, 0x26);

	lcd->SendCommand(lcd, ILI9341_COMMAND_POWER_CONTROL_2);
	lcd->SendData8(lcd, 0x11);

	lcd->SendCommand(lcd, ILI9341_COMMAND_VCOM_CONTROL_1);
	lcd->SendData8(lcd, 0x35);
	lcd->SendData8(lcd, 0x3E);

	lcd->SendCommand(lcd, ILI9341_COMMAND_VCOM_CONTROL_2);
	lcd->SendData8(lcd, 0xBE);

	lcd->SendCommand(lcd, ILI9341_COMMAND_MEMORY_ACCESS_CONTROL);
	lcd->SendData8(lcd, 0xE8); // 48

	lcd->SendCommand(lcd, ILI9341_COMMAND_COLMOD_PIXEL_FORMAT_SET);
	lcd->SendData8(lcd, 0x55); // 16 bit pixel

	lcd->SendCommand(lcd, ILI9341_COMMAND_FRAME_RATE_CONTROL_NORMAL);
	lcd->SendData8(lcd, 0x00);
	lcd->SendData8(lcd, 0b11111);

	lcd->SendCommand(lcd, ILI9341_COMMAND_DIGITAL_GAMMA_CONTROL_1);
	lcd->SendData8(lcd, 0x08);

	lcd->SendCommand(lcd, ILI9341_COMMAND_GAMMA_SET);
	lcd->SendData8(lcd, 0x01); // gamma set for curve 01/2/04/08

	lcd->SendCommand(lcd, ILI9341_COMMAND_POSITIVE_GAMMA_CORRECTION);
	lcd->SendData8(lcd, 0x1F);
	lcd->SendData8(lcd, 0x1A);
	lcd->SendData8(lcd, 0x18);
	lcd->SendData8(lcd, 0x0A);
	lcd->SendData8(lcd, 0x0F);
	lcd->SendData8(lcd, 0x06);
	lcd->SendData8(lcd, 0x45);
	lcd->SendData8(lcd, 0x87);
	lcd->SendData8(lcd, 0x32);
	lcd->SendData8(lcd, 0x0A);
	lcd->SendData8(lcd, 0x07);
	lcd->SendData8(lcd, 0x02);
	lcd->SendData8(lcd, 0x07);
	lcd->SendData8(lcd, 0x05);
	lcd->SendData8(lcd, 0x00);

	lcd->SendCommand(lcd, ILI9341_COMMAND_NEGATIVE_GAMMA_CORRECTION);
	lcd->SendData8(lcd, 0x00);
	lcd->SendData8(lcd, 0x25);
	lcd->SendData8(lcd, 0x27);
	lcd->SendData8(lcd, 0x05);
	lcd->SendData8(lcd, 0x10);
	lcd->SendData8(lcd, 0x09);
	lcd->SendData8(lcd, 0x3A);
	lcd->SendData8(lcd, 0x78);
	lcd->SendData8(lcd, 0x4D);
	lcd->SendData8(lcd, 0x05);
	lcd->SendData8(lcd, 0x18);
	lcd->SendData8(lcd, 0x0D);
	lcd->SendData8(lcd, 0x38);
	lcd->SendData8(lcd, 0x3A);
	lcd->SendData8(lcd, 0x1F);

	lcd->SendCommand(lcd, ILI9341_COMMAND_TEARING_EFFECT_LINE_OFF);
	//lcd->SendCommand(lcd, ILI9341_COMMAND_TEARING_EFFECT_LINE_ON);

	lcd->SendCommand(lcd, ILI9341_COMMAND_ENTRY_MODE_SET);
	lcd->SendData8(lcd, 0x07);

	lcd->SendCommand(lcd, ILI9341_COMMAND_DISPLAY_FUNCTION_CONTROL);
	lcd->SendData8(lcd, 0x0A);
	lcd->SendData8(lcd, 0x82);
	lcd->SendData8(lcd, 0x27);
	lcd->SendData8(lcd, 0x00);

	lcd->SendCommand(lcd, ILI9341_COMMAND_SLEEP_OUT);

	Delay(TDelay);


	lcd->SendCommand(lcd, ILI9341_COMMAND_DISPLAY_ON);
	Delay(TDelay);

	lcd->SendCommand(lcd, ILI9341_COMMAND_COLUMN_ADDRESS_SET);
	lcd->SendData16(lcd, 0);
	lcd->SendData16(lcd, INV(lcd->width - 1));

	lcd->SendCommand(lcd, ILI9341_COMMAND_PAGE_ADDRESS_SET);
	lcd->SendData16(lcd, 0);
	lcd->SendData16(lcd, INV(lcd->height - 1));
	lcd->SendCommand(lcd, ILI9341_COMMAND_MEMORY_WRITE);


	SPISetTranfserSize(&lcd->spi, SPI_TRANSFER_SIZE_HALF_WORD);
	lcd->currentLine = lcd->height - LINES;
	DidSent(lcd);
}


void InitLCD(LCDt* lcd, uint16_t width, uint16_t height, Pin mosi, Pin miso, Pin clock, Pin cs, Pin a0, Pin reset, LCDRenderLine renderer)
{
	lcd->width = width;
	lcd->height = height;

	lcd->CS = cs;
	lcd->A0 = a0;
	lcd->Reset = reset;

	lcd->renderer = renderer;

	lcd->SendCommand = &SendCommand;
	lcd->SendData16 = &SendData16;
	lcd->SendData8 = &SendData8;

    PinConfigureOut(cs, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_NOPULL);
    PinConfigureOut(a0, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_NOPULL);
    PinConfigureOut(reset, GPIO_OType_PP, GPIO_High_Speed, GPIO_PuPd_NOPULL);

	SPI_Configuration configuration;
	configuration.BaudRatePrescaler = SPI_BAUD_DIV4;
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
    SPISetHandler(&lcd->spi, &DidSent, lcd);

    InitializeLCD(lcd);
}