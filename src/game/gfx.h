#ifndef GFX_H
#define GFX_H

#include <stdint.h>

typedef struct {
    uint16_t BG_COLOR;
    uint16_t* PLAT;
    uint16_t* MARIO;
    uint16_t MARIO_ALPHA;
    uint16_t* MARIO_MOVE;
    uint16_t* MARIO_JUMP;
    uint16_t* MARIO_DEAD;
    uint16_t* MARIO_FALL;
    uint16_t* GROUND;
    uint16_t* BLOCK;
    uint16_t* TREE_S;
    uint16_t* TREE_M;
    uint16_t* TREE_E;
    uint16_t* ENEMY_1;
    uint16_t* ENEMY_1_DEAD;
    uint16_t* PORTAL_TOP;
    uint16_t* PORTAL_MID;
    uint16_t* COIN;
    uint16_t* CLOUDS;
    uint16_t* SPECIAL;
    uint16_t* FLAG_POLE;
    uint16_t* FLAG_TOP;
    uint16_t* FLAG;
    uint16_t FLAG_BG_COLOR;
    uint16_t* CASTLE;
} GFX_t;

extern GFX_t GFX;

#endif