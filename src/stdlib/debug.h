#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include "uart.h"

void Debug(char* string);
void DebugInt(int32_t val);
void DebugSetUsart(USARTt* usart);

void Abort(char* msg);

#endif