#ifndef _LCD_BOARD_DEF_H
#define _LCD_BOARD_DEF_H 1

#include <xcat.h>

/* W konkretnym projekcie trzeba w makefile zdefiniować rzeczywistą wartość
poniższej stałej - zależą od niej opóźnienia w sterowniku LCD. Jeśli jej nie
zdefionowano, przyjmujemy wartość, która da największe opóźnienia. */

#ifndef MAIN_CLOCK_MHZ
  #define MAIN_CLOCK_MHZ 100
#endif

/* Żeby używać LCD z SPI3 trzeba następująco zdefiniować wyjścia:
PC10 SCK, PC11 A0, PC12 SDA, PD2 CS. */

#define LCD_CS_GPIO_N  D
#define LCD_CS_PIN_N   2

#define LCD_A0_GPIO_N  C
#define LCD_A0_PIN_N   11

#define LCD_SDA_GPIO_N C
#define LCD_SDA_PIN_N  12

#define LCD_SCK_GPIO_N C
#define LCD_SCK_PIN_N  10

#endif
