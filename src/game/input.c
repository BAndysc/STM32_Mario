#include "input.h"

int INPUT_UP;
int INPUT_DOWN;
int INPUT_LEFT;
int INPUT_RIGHT;
int INPUT_JUMP;
int INPUT_ANALOG = 1;
Vectorf input;

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