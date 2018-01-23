#ifndef ILI9341_H
#define ILI9341_H

#include <stdint.h>
#include "lcd.h"

void InitILI9341LCD(LCDt* lcd, uint16_t width, uint16_t height, Pin mosi, Pin miso, Pin clock, Pin CS, Pin A0, Pin reset, LCDRenderLine requestLine);

#endif
