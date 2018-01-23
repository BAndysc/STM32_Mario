#include <fonts.h>
#include "render.h"
#include "gfx.h"
#include "gameobject.h"
#include "store.h"
#include "input.h"
#include "levels.h"
#include "../debug.h"
#include "../lcd.h"

int16_t SCALE = 2;
#define INV(A) (A); //((A >> 8) | ((A & 0xFF) << 8))

Vector offset;

static const font_t* CurrentFont = &font8x16;

static inline uint8_t ObjectInViewport(GameObject* go, int32_t lines, int16_t startLine, int16_t width)
{
    return (startLine + lines >= go->PositionInt.y - offset.y &&
           startLine <= go->PositionInt.y + go->Size.y - offset.y &&
           go->PositionInt.x + go->Size.x >= offset.x &&
           go->PositionInt.x - go->Size.x < offset.x + width);
}

static inline uint8_t PointInViewportXAxis(int32_t x, int16_t width)
{
    return x >= offset.x && x < width + offset.x;
}

static void DrawObject(GameObject* go, uint16_t buffer[], int16_t lines, int16_t startLine, int16_t width)
{
    int16_t lineOffset = startLine + offset.y - go->PositionInt.y;

    for (int j = 0; j < go->Size.x; ++j)
    {
        int16_t flipoff = go->FlipSprite ? (go->Size.x - j) : j;
        if (PointInViewportXAxis(go->PositionInt.x + j, width))
        {
            for (int i = 0; i < lines; ++i)
            {
                if (lineOffset + i >= 0 && lineOffset + i < go->Size.y)
                {
                    uint16_t col = go->Sprite[(lineOffset+i) * go->Size.x + flipoff];
                    if (col != go->Alpha) {
                        col = INV(col);
                        int off = (j + go->PositionInt.x - offset.x) * SCALE + i * 160 * SCALE * SCALE;
                        buffer[off] = col;
                        if (SCALE > 1)
                        {
                            buffer[off+1] = col;
                            buffer[off+width*2] = col;
                            buffer[off+width*2+1] = col;
                        }
                    }
                }
            }
        }
    }
}


char text[35];
//uint16_t sky[] = {61407, 61375, 59327, 59327, 59327, 57247, 57247, 57247, 55199, 55199, 55167, 53119, 53119, 53119, 51071, 51039, 51039, 48991, 48991, 48991, 46911, 46911, 46911, 44863, 44831, 44831, 44831, 42783, 42783, 42751};
uint16_t sky[] = {21620, 21620, 21620, 21620,21620,21620, 23733, 25846, 27959, 30072, 32185, 34298, 36411, 38524, 40637, 42750, 44863, 46975, 49087, 51199,51199,51199,51199,51199,51199,51199};

static void RenderText(uint16_t buffer[], int32_t offsetX, int32_t offsetY, int16_t width, char* message, int len, uint16_t color)
{
    for (int c = 0; c < len; ++c)
    {
        char letter = message[c];

        uint16_t const *p = &CurrentFont->table[(letter - FIRST_CHAR) * CurrentFont->height];
        for (int i = 0; i < CurrentFont->height; ++i)
        {
            for (int j = 0, w = p[i]; j < CurrentFont->width; ++j, w >>= 1)
            {
                if (w&1)
                    buffer[(i+offsetY) * width * SCALE + (j+offsetX)] = color;
            }
        }

        offsetX += CurrentFont->width;
    }
}

static int16_t lostOffset = 50;
static int16_t lostAdd = 2;

static void RenderLost(LCDt* lcd, uint16_t buffer[], int16_t startLine, int16_t lines, int16_t width)
{
    uint32_t* buf = (uint32_t*)buffer;
    for (int j = 0; j < lines * width * SCALE * SCALE / 2; ++j)
        (*buf++) = 0x0000;

    if (startLine == 50)
    {
        RenderText(buffer, lostOffset, 0, width, "YOU LOST", 8, 0xFFFF);
        lostOffset += lostAdd;

        if (lostOffset >= width * SCALE - 8 * CurrentFont->width)
        {
            lostAdd = -SCALE;
        }
        else if (lostOffset <= 10)
        {
            lostAdd = SCALE;
        }
    }
}

static void RenderWon(LCDt* lcd, uint16_t buffer[], int16_t startLine, int16_t lines, int16_t width)
{
    uint32_t* buf = (uint32_t*)buffer;
    for (int j = 0; j < lines * width * SCALE * SCALE / 2; ++j)
        (*buf++) = 0x0000;

    if (startLine == 50)
    {
        RenderText(buffer, lostOffset, 0, width, "YOU WON!", 8, 0xFFFF);
        lostOffset += lostAdd;

        if (lostOffset >= width * SCALE - 8 * CurrentFont->width)
        {
            lostAdd = -SCALE;
        }
        else if (lostOffset <= 10)
        {
            lostAdd = SCALE;
        }
    }
}


static void RenderGame(LCDt* lcd, uint16_t buffer[], int16_t startLine, int16_t lines, int16_t width)
{
    // fill with background
    uint32_t* buf = (uint32_t*)buffer;
    for (int j = 0; j < lines; ++j)
    {
        uint32_t bg = INV(sky[(startLine+offset.y + j) / 8]); //INV(GFX.BG_COLOR);
        bg = bg | (bg << 16);
        for (int i = 0; i < width * SCALE * SCALE / 2; ++i)
        {
            (*buf++) = bg;
        }
    }

    // check which objects are visible and draw their line
    for (int i = 0; i < ObjectsCounter; ++i)
    {
        GameObject* go = RenderObjects[i];

        if (ObjectInViewport(go, lines, startLine, width) && !go->Deactive)
            DrawObject(go, buffer, lines, startLine, width);
    }

    if (startLine == 0 || startLine == 10)
    {
        char* ptr;

        if (startLine == 0)
        {
            ptr = "MARIO      POINTS              TIME";
        }
        else
        {
            int32_t left = level->time - (MilisecondsSinceStart - StartTicks) / 1000;

            if (left <= 1)
                LIVES = 0;

            for (int i = 0; i <35; ++i)
                text[i] = ' ';
            text[34] = (left)%10 + '0';
            text[33] = ((left / 10)%10) + '0';
            text[32] = ((left / 100)%10) + '0';

            text[3] = 'x';
            text[4] = LIVES + '0';

            text[16] = (POINTS)%10 + '0';
            text[15] = ((POINTS / 10)%10) + '0';
            text[14] = ((POINTS / 100)%10) + '0';

            ptr = text;
        }

        int offset = 20;
        int offsetY = 4;

        RenderText(buffer, offset, offsetY, width, ptr, 35, 0x0000);
    }
}

void RenderLine(LCDt* lcd, uint16_t buffer[], int16_t startLine, int16_t lines, int16_t width)
{
    startLine /= SCALE;
    width /= SCALE;
    lines /= SCALE;


    if (LIVES == 0)
        RenderLost(lcd, buffer, startLine, lines, width);
    else if (WON)
        RenderWon(lcd, buffer, startLine, lines, width);
    else
        RenderGame(lcd, buffer, startLine, lines, width);

    if (startLine * SCALE  + lines * SCALE == lcd->height)
        AfterRender();
}


#define MIN(A, B) ((A)<(B)?A:B)
#define MAX(A, B) ((A)>(B)?A:B)

void AfterRender()
{
    for (int i = 0; i < ObjectsUpdateCounter; ++i)
        UpdateLoop[i]->PositionInt = Vectorf2Vector(UpdateLoop[i]->Position);

    offset.x = mario.super.PositionInt.x - 80;
    offset.y = mario.super.PositionInt.y - 60;

    offset.y = MAX(0, MIN(level->height*16-60-60, offset.y));
    offset.x = MAX(0, offset.x);
}