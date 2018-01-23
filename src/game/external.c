#include "external.h"
#include "input.h"

USARTt* usart;

void Puts(char* message)
{
    if (usart != nullptr)
        usart->write(usart, message);
}

void SetOut(USARTt* serial)
{
    usart = serial;
}

static void myReadHandler(void* data, char const* recv, uint8_t len)
{
    ProcessInput(recv, len);
}

void SetIn(USARTt* serial)
{
    UsartSetReadHandler(serial, 1, myReadHandler, nullptr);
}

void SetJoystick(uint8_t readings[])
{
    JoystickInput = readings;
}