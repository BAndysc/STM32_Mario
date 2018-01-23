#include "debug.h"

USARTt* debugUsart;

void Debug(char* string)
{
    if (debugUsart != nullptr)
        debugUsart->writeSync(debugUsart, string);
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

void Abort(char* msg)
{
    Debug(msg);
    __BKPT(0);
}