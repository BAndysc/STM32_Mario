#ifndef INPUT_H
#define INPUT_H

#define min(a, b) (a<b?a:b)
#define max(a, b) (a>b?a:b)

#include "vector.h"

extern int INPUT_UP;
extern int INPUT_DOWN;
extern int INPUT_LEFT;
extern int INPUT_RIGHT;
extern int INPUT_JUMP;

extern int INPUT_ANALOG;

extern Vectorf input;

void UpdateInput();

int StartTicks;

extern int32_t TICKS;
#endif