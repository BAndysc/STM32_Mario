#ifndef LEVELS_H
#define LEVELS_H

typedef struct {
	int width;
	int height;
	char* data;
} Level;

typedef struct {
	Level level1;
	Level level2;
} Levels_t;

extern Levels_t Levels;

#endif