#include "input.h"

int INPUT_UP;
int INPUT_DOWN;
int INPUT_LEFT;
int INPUT_RIGHT;
int INPUT_JUMP;
int INPUT_ANALOG = 1;
Vectorf input;

int StartTicks;

void UpdateInput()
{
    if (INPUT_UP)
        input.y  = -1;
    else if (INPUT_DOWN)
        input.y = 1;
    else
        input.y = 0;

    if (INPUT_LEFT)
        input.x = -1;
    else if (INPUT_RIGHT)
        input.x = 1;
    else
        input.x = 0;
}

void ProcessInput(char const* recv, uint8_t len)
{
    switch (recv[0])
    {
        case 't':
            INPUT_ANALOG = !INPUT_ANALOG;
            break;
        case 'a':
            INPUT_LEFT = 0;
            break;
        case 'd':
            INPUT_RIGHT = 0;
            break;
        case 'w':
            INPUT_JUMP = 0;
            break;
        case 'A':
            INPUT_LEFT = 1;
            break;
        case 'D':
            INPUT_RIGHT = 1;
            break;
        case 'W':
            INPUT_JUMP = 1;
            break;
        default:
            break;
    }
}