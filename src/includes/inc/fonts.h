#ifndef _FONTS_H
#define _FONTS_H 1

#include <stdint.h>

/** Bitmap fonts for ASCII characters from 32 to 126 **/

#define FIRST_CHAR 32
#define LAST_CHAR  126

typedef struct {
  const uint16_t *table;
  int            width;
  int            height;
} font_t;

extern const font_t font8x16;
extern const font_t font10x18;
extern const font_t font12x24;
extern const font_t font14x28;

#define LCD_DEFAULT_FONT  font14x28

#endif
