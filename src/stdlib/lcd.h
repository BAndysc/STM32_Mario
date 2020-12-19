#ifndef _LCD_H
#define _LCD_H 1

#include <stdint.h>
#include "pins.h"
#include "spi.h"
#include "timer.h"

struct LCDt;

#define LCD_BUFFER_SIZE (320 * 40)

typedef void (*LCDRenderLine)(struct LCDt* lcd, uint16_t* buffor, uint16_t startLine, uint16_t lines, uint16_t width);

typedef enum {
    LCD_MSG_COMMAND,
    LCD_MSG_DATA,
    LCD_MSG_DATA16,
    LCD_MSG_DELAY,
    LCD_MSG_WIDTH,
    LCD_MSG_HEIGHT,
    LCD_MSG_END
} LcdMsg;

typedef struct {
    LcdMsg type;
    uint8_t byte;
} LcdInitInstruction;

typedef struct LCDt {
    SPIt spi;
    Pin CS;
    Pin A0;
    Pin Reset;
    uint16_t width;
    uint16_t height;
    LCDRenderLine renderer;

    uint16_t currentLine;
    uint16_t buffer[LCD_BUFFER_SIZE];

    LcdInitInstruction* instructions;
    uint16_t currentInitIndex;
    bool initialized;

    Timer timer;

    void (*SendCommand)(struct LCDt* lcd, uint8_t command);
    void (*SendData8)(struct LCDt* lcd, uint8_t data);
    void (*SendData16)(struct LCDt* lcd, uint16_t data);
    void (*SendGeneratedLines)(struct LCDt* lcd);
} LCDt;


void InitLCD(LCDt* lcd, uint16_t width, uint16_t height, Pin mosi, Pin miso, Pin clock, Pin CS, Pin A0, Pin reset,
             LCDRenderLine requestLine, LcdInitInstruction* InitSequence);

#endif
