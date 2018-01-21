#ifndef _GPIO_H
#define _GPIO_H 1

#include <stm32.h>

/** Funkcje konfigurujące pojedyncze wyprowadzenie **/

/* Typy wzięte z biblioteki STM32 */

typedef enum {
  GPIO_OType_PP = 0x00,
  GPIO_OType_OD = 0x01
} GPIOOType_TypeDef;

typedef enum {
  GPIO_Low_Speed    = 0x00,
  GPIO_Medium_Speed = 0x01,
  GPIO_Fast_Speed   = 0x02,
  GPIO_High_Speed   = 0x03
} GPIOSpeed_TypeDef;

typedef enum {
  GPIO_PuPd_NOPULL = 0x00,
  GPIO_PuPd_UP     = 0x01,
  GPIO_PuPd_DOWN   = 0x02
} GPIOPuPd_TypeDef;

typedef enum {
  EXTI_Mode_Interrupt = 0x00,
  EXTI_Mode_Event     = 0x04,
  EXTI_Mode_Disable   = 0xFF  /* Wartość, której nie ma w bibliotece STM32. */
} EXTIMode_TypeDef;

typedef enum {
  EXTI_Trigger_Irrelevant     = 0x00, /* Wartość, której nie ma w bibliotece STM32. */
  EXTI_Trigger_Rising         = 0x08,
  EXTI_Trigger_Falling        = 0x0C,
  EXTI_Trigger_Rising_Falling = 0x10
} EXTITrigger_TypeDef;

void GPIOinConfigure(GPIO_TypeDef * const gpio, uint32_t pin,
                     GPIOPuPd_TypeDef pull, EXTIMode_TypeDef mode,
                     EXTITrigger_TypeDef trigger);

void GPIOoutConfigure(GPIO_TypeDef * const gpio, uint32_t pin,
                      GPIOOType_TypeDef otype, GPIOSpeed_TypeDef speed,
                      GPIOPuPd_TypeDef pull);

void GPIOafConfigure(GPIO_TypeDef * const gpio, uint32_t pin_n,
                     GPIOOType_TypeDef otype, GPIOSpeed_TypeDef speed,
                     GPIOPuPd_TypeDef pull, uint32_t fun);

void GPIOainConfigure(GPIO_TypeDef * const gpio, uint32_t pin);

#endif
