#include <gpio.h>

/** Funkcje konfigurujące pojedyncze wyprowadzenie
w STM32F0xx, STM32F2xx, STM32F3xx, STM32F4xx, STM32FLxx, ... **/

/* Typ wzięty z biblioteki STM32 */
typedef enum {
  GPIO_Mode_IN  = 0x00,
  GPIO_Mode_OUT = 0x01,
  GPIO_Mode_AF  = 0x02,
  GPIO_Mode_AN  = 0x03
} GPIOMode_TypeDef;

/* Ten kod nie kompiluje się na STM32F10x. Funkcje z tego modułu
zastępują dostarczane przez bibliotekę STM32 funkcje GPIO_Init,
GPIO_PinAFConfig, SYSCFG_EXTILineConfig, EXTI_Init, które wykonują
podobne zadanie, ale są ortogonalne, dużo mniej optymalne i też nie
zapewniają przenośności kodu źródłowego. */

/* Konfiguruj wyprowadzenie jako wejście cyfrowe:
  gpio    -- nazwa układu peryferyjnego obsługującego port (adres początku
             obszaru pamięci z rejestrami tego portu): GPIOA, GPIOB, ...;
  pin_n   -- numer wyprowadzenia: 0, 1, 2, ..., 15;
  pull    -- rezystor: brak GPIO_PuPd_NOPULL (0), podciągający do zasilania
             GPIO_PuPd_UP (1), ściągający do masy GPIO_PuPd_DOWN (2);
  mode    -- EXTI_Mode_Interrupt (0), EXTI_Mode_Event (4), EXTI_Mode_Disable (255);
  trigger -- zbocze wyzwalające: EXTI_Trigger_Irrelevant (0), EXTI_Trigger_Rising (8),
             EXTI_Trigger_Falling (12), EXTI_Trigger_Rising_Falling (16). */
void GPIOinConfigure(GPIO_TypeDef * const gpio, uint32_t pin_n,
                     GPIOPuPd_TypeDef pull, EXTIMode_TypeDef mode,
                     EXTITrigger_TypeDef trigger) {
  uint32_t reg, pin;

  reg = gpio->PUPDR; /* Pull Up Pull Down Register */
  reg &= ~(3U << (2U * pin_n));
  reg |= pull << (2U * pin_n);
  gpio->PUPDR = reg;

  reg = gpio->MODER; /* MODE Register */
  reg &= ~(3U << (2U * pin_n));
  reg |= GPIO_Mode_IN << (2U * pin_n); /* GPIO_Mode_IN == 0, optymalizator usuwa tę instrukcję. */
  gpio->MODER = reg;

  if (mode == EXTI_Mode_Interrupt || mode == EXTI_Mode_Event) {
    reg = SYSCFG->EXTICR[pin_n >> 2U]; /* EXTernal Interrupt Configuration Register */
    reg &= ~(0xFU << (4U * (pin_n & 3U)));
    reg |= (((uint32_t)gpio - AHB1PERIPH_BASE) >> 10U) << (4U * (pin_n & 3U));
    SYSCFG->EXTICR[pin_n >> 2U] = reg;

    /* Zmienna pin_n zawiera numer wyprowadzenia, a pin jest maską bitową. */
    pin = 1U << pin_n;

    if (trigger == EXTI_Trigger_Rising) {
      EXTI->FTSR &= ~pin; /* Falling Trigger Selection Register */
      EXTI->RTSR |= pin;  /* Rising Trigger Selection Register */
    }
    else if (trigger == EXTI_Trigger_Falling) {
      EXTI->RTSR &= ~pin;
      EXTI->FTSR |= pin;
    }
    else if (trigger == EXTI_Trigger_Rising_Falling) {
      EXTI->RTSR |= pin;
      EXTI->FTSR |= pin;
    }

    if (mode == EXTI_Mode_Interrupt) {
      /* Najpierw zablokuj zdarzenie, a potem uaktywnij przerwanie. */
      EXTI->EMR &= ~pin; /* Event Mask Register */
      EXTI->IMR |= pin;  /* Interrupt Mask Register */
    }
    else { /* mode == EXTI_Mode_Event */
      /* Najpierw zablokuj przerwanie, a potem uaktywnij zdarzenie. */
      EXTI->IMR &= ~pin;
      EXTI->EMR |= pin;
    }
  }
  /* W przeciwnym przypadku nie modyfikuj rejestrów SYSCFG->EXTICR i EXTI,
  bo być może zostało uaktywnione przerwanie lub zdarzenie z wyprowadzenia
  o tym samym numerze, ale na innym porcie. */
}

/* Konfiguruj wyprowadzenie jako wyjście cyfrowe:
  gpio  -- nazwa układu peryferyjnego obsługującego port (adres początku
           obszaru pamięci z rejestrami tego portu): GPIOA, GPIOB, ...;
  pin_n -- numer wyprowadzenia: 0, 1, 2, ..., 15;
  otype -- wyjście przeciwsobne GPIO_OType_PP (0) lub otwarty dren GPIO_OType_OD (1);
  speed -- szybkość zmian poziomu: GPIO_Low_Speed (0), GPIO_Medium_Speed (1),
           GPIO_Fast_Speed (2), GPIO_High_Speed (3);
  pull  -- rezystor: brak GPIO_PuPd_NOPULL (0), podciągający do zasilania
           GPIO_PuPd_UP (1), ściągający do masy GPIO_PuPd_DOWN (2). */
void GPIOoutConfigure(GPIO_TypeDef * const gpio, uint32_t pin_n,
                      GPIOOType_TypeDef otype, GPIOSpeed_TypeDef speed,
                      GPIOPuPd_TypeDef pull) {
  uint32_t reg;

  reg = gpio->OSPEEDR; /* Output SPEED Register */
  reg &= ~(3U << (2U * pin_n));
  reg |= speed << (2U * pin_n);
  gpio->OSPEEDR = reg;

  reg = gpio->PUPDR; /* Pull Up Pull Down Register */
  reg &= ~(3U << (2U * pin_n));
  reg |= pull << (2U * pin_n);
  gpio->PUPDR = reg;

  reg = gpio->OTYPER; /* Output Type Register */
  reg &= ~(1U << pin_n);
  reg |= otype << pin_n;
  gpio->OTYPER = reg;

  reg = gpio->MODER; /* MODE Register */
  reg &= ~(3U << (2U * pin_n));
  reg |= GPIO_Mode_OUT << (2U * pin_n); /* GPIO_Mode_OUT == 1 */
  gpio->MODER = reg;
}

/* Konfiguruj alternatywną funkcję wyprowadzenia:
  gpio  -- nazwa układu peryferyjnego obsługującego port (adres początku
           obszaru pamięci z rejestrami tego portu): GPIOA, GPIOB, ...;
  pin_n -- numer wyprowadzenia: 0, 1, 2, ..., 15;
  otype -- wyjście przeciwsobne GPIO_OType_PP (0) lub otwarty dren GPIO_OType_OD (1);
  speed -- szybkość zmian poziomu: GPIO_Low_Speed (0), GPIO_Medium_Speed (1),
           GPIO_Fast_Speed (2), GPIO_High_Speed (3);
  pull  -- rezystor: brak GPIO_PuPd_NOPULL (0), podciągający do zasilania
           GPIO_PuPd_UP (1), ściągający do masy GPIO_PuPd_DOWN (2);
  fun   -- wybór funkcji alternatywnej. */
void GPIOafConfigure(GPIO_TypeDef * const gpio, uint32_t pin_n,
                     GPIOOType_TypeDef otype, GPIOSpeed_TypeDef speed,
                     GPIOPuPd_TypeDef pull, uint32_t fun) {
  uint32_t reg;

  reg = gpio->OSPEEDR; /* Output SPEED Register */
  reg &= ~(3U << (2U * pin_n));
  reg |= speed << (2U * pin_n);
  gpio->OSPEEDR = reg;

  reg = gpio->PUPDR; /* Pull Up Pull Down Register */
  reg &= ~(3U << (2U * pin_n));
  reg |= pull << (2U * pin_n);
  gpio->PUPDR = reg;

  reg = gpio->OTYPER; /* Output Type Register */
  reg &= ~(1U << pin_n);
  reg |= otype << pin_n;
  gpio->OTYPER = reg;

  reg = gpio->AFR[pin_n >> 3U]; /* Alternate Function Register */
  reg &= ~(0xFU << (4U * (pin_n & 7U)));
  reg |= fun << (4U * (pin_n & 7U));
  gpio->AFR[pin_n >> 3U] = reg;

  reg = gpio->MODER; /* MODE Register */
  reg &= ~(3U << (2U * pin_n));
  reg |= GPIO_Mode_AF << (2U * pin_n); /* GPIO_Mode_AF == 2 */
  gpio->MODER = reg;
}

/* Konfiguruj wyprowadzenie jako analogowe:
  gpio  -- nazwa układu peryferyjnego obsługującego port (adres początku
           obszaru pamięci z rejestrami tego portu): GPIOA, GPIOB, ...;
  pin_n -- numer wyprowadzenia: 0, 1, 2, ..., 15. */
void GPIOainConfigure(GPIO_TypeDef * const gpio, uint32_t pin_n) {
  uint32_t reg;

  reg = gpio->PUPDR;            /* Dla wyprowadzeń analogowych rezystory */
  reg &= ~(3U << (2U * pin_n)); /* nie mają sensu, choć wbrew dokumentacji */
  gpio->PUPDR = reg;            /* dają się skonfigurować. */

  reg = gpio->MODER;
  reg &= ~(3U << (2U * pin_n)); /* Ta instrukcja jest zbędna, bo GPIO_Mode_AN == 3. */
  reg |= GPIO_Mode_AN << (2U * pin_n);
  gpio->MODER = reg;
}
