#ifndef LEVELS_H
#define LEVELS_H

#include <stdint.h>

typedef struct {
    int width;
    int height;
    int time;
    int16_t castleX;
    int16_t castleY;
    char* data;
} Level;

typedef struct {
    Level level1;
    Level level2;
} Levels_t;

extern Levels_t Levels;

#endif