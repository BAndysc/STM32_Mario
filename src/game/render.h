#ifndef RENDER_H
#define RENDER_H

#include "vector.h"
#include <stdint.h>

extern Vector offset;

int SCALE;

typedef struct LCDt LCDt;


void RenderLine(LCDt* lcd, uint16_t buffer[], int16_t startLine, int16_t lines, int16_t width);

void AfterRender();
#endif