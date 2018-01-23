#ifndef MARIO_H
#define MARIO_H

#include "gameobject.h"

typedef struct Mario {
    GameObject super;

    int Grounded;

    int Frame;
    float FrameTime;

    int Dead;
    int Won;
} Mario;

void InitMario(Mario* mario, int x, int y);

#endif