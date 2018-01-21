#include "debug.h"

USARTt* debugUsart;

void Debug(char* string)
{
    if (debugUsart != nullptr)
        debugUsart->write(debugUsart, string);
}

void DebugInt(int32_t val)
{
    if (debugUsart != nullptr)
        debugUsart->writeInt(debugUsart, val);
}

void DebugSetUsart(USARTt* usart)
{
    debugUsart = usart;
}