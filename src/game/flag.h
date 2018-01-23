#ifndef MARIO_FLAG_H
#define MARIO_FLAG_H

#include "gameobject.h"

typedef struct Flag {
    GameObject super;

    int Triggered;
    int Grounded;
} Flag;

void InitFlag(Flag* mario, int x, int y);

#endif //MARIO_FLAG_H
