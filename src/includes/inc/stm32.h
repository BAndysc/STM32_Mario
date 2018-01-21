#ifndef _STM32_H
#define _STM32_H 1

/** Ten plik przykrywa całe badziewie biblioteki CMSIS. Należy go włączyć
wszędzie, gdzie chcemy użyć tej biblioteki, zamiast włączać poszczególne
plik nagłówkowe, co pozwala na zachowanie pewnej niezależności od wersji
zastosowanego mikrokontrolera. Stałe zawierające w nazwie wielką literę X
pozostawiono dla zachowania kompatybilności ze starszymi wersjami biblioteki.
Najnowsza wersja, dostarczana z biblioteką Cube, zawiera w nazwach stałych
wyłącznie małą literę x. **/

/* Stałe, których brakuje w CMSIS. */
#define GPIO_AF_TIM1   1
#define GPIO_AF_TIM2   1
#define GPIO_AF_TIM3   2
#define GPIO_AF_TIM4   2
#define GPIO_AF_TIM5   2
#define GPIO_AF_TIM9   3
#define GPIO_AF_TIM10  3
#define GPIO_AF_TIM11  3
#define GPIO_AF_I2C1   4
#define GPIO_AF_I2C2   4
#define GPIO_AF_I2C3   4
#define GPIO_AF_SPI3   6
#define GPIO_AF_USART1 7
#define GPIO_AF_USART2 7
#define GPIO_AF_USART6 8
#define EXTI_PR_PR21   (1 << 21)
#define EXTI_IMR_MR21  (1 << 21)
#define EXTI_RTSR_MR21 (1 << 21)
#define EXTI_PR_PR22   (1 << 22)
#define EXTI_IMR_MR22  (1 << 22)
#define EXTI_RTSR_MR22 (1 << 22)

#if defined STM32F0XX   || \
    defined STM32F030x6 || defined STM32F030x8 || defined STM32F031x6 || \
    defined STM32F038xx || defined STM32F042x6 || defined STM32F048xx || \
    defined STM32F051x8 || defined STM32F058xx || defined STM32F071xB || \
    defined STM32F072xB || defined STM32F078xx || defined STM32F091xC || \
    defined STM32F098xx

  #include <stm32f0xx.h>

#elif defined STM32F10X_LD || defined STM32F10X_LD_VL || \
      defined STM32F10X_MD || defined STM32F10X_MD_VL || \
      defined STM32F10X_HD || defined STM32F10X_HD_VL || \
      defined STM32F10X_XL || defined STM32F10X_CL

  #include <stm32f10x.h>

#elif defined STM32F2XX || \
      defined STM32F205xx || defined STM32F215xx || \
      defined STM32F207xx || defined STM32F217xx

  #include <stm32f2xx.h>

#elif defined STM32F301x8 || \
      defined STM32F302x8 || defined STM32F302xC || defined STM32F302xE || \
      defined STM32F303x8 || defined STM32F303xC || defined STM32F303xE || \
      defined STM32F334x8 || defined STM32F373xC || \
      defined STM32F318xx || defined STM32F328xx || defined STM32F358xx || \
      defined STM32F378xx || defined STM32F398xx

  #include <stm32f3xx.h>

#elif defined STM32F401xC || defined STM32F401xE || defined STM32F411xE || \
      defined STM32F405xx || defined STM32F407xx || \
      defined STM32F415xx || defined STM32F417xx || \
      defined STM32F427xx || defined STM32F429xx || \
      defined STM32F437xx || defined STM32F439xx

  #include <stm32f4xx.h>

#elif defined STM32L051xx || defined STM32L052xx || defined STM32L053xx || \
      defined STM32L061xx || defined STM32L062xx || defined STM32L063xx

  #include <stm32l0xx.h>

#elif defined STM32L1XX_MD || defined STM32L1XX_MDP || defined STM32L1XX_HD || \
      defined STM32L100xB  || defined STM32L100xBA  || defined STM32L100xC  || \
      defined STM32L151xB  || defined STM32L151xBA  || defined STM32L151xC  || \
      defined STM32L151xCA || defined STM32L151xD   || defined STM32L151xE  || \
      defined STM32L152xB  || defined STM32L152xBA  || defined STM32L152xC  || \
      defined STM32L152xCA || defined STM32L152xD   || defined STM32L152xE  || \
      defined STM32L162xC  || defined STM32L162xCA  || defined STM32L162xD  || \
      defined STM32L162xE

  #include <stm32l1xx.h>

#else

  #error STM32 device type or subfamily is unknown or undefined.

#endif

#endif /* _STM32_H */
