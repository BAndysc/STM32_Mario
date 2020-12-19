#include "clock.h"
#include <gpio.h>
#include <stm32.h>

#define _M 8
#define _N 384 //300
#define _P 4 // 6
#define _Q 8 // 9

uint32_t SystemCoreClock = 16000000;

// PLL_IN = 8
// VCO = 384
// PLL_OUT = 96
//
// F_SYSCLCK = PLL_OUT
// F_HCLK = F_SYSCLCK / 1 = F_SYSCLCK = PLL_OUT = 96
// F_PLK1 = F_HCLK / 2 = 48
// F_PLK2 = F_HCLK / 1 = 96

static void ChangeClock(int M, int N, int P, int Q)
{
    RCC->CR &= ~(RCC_CR_PLLI2SON | RCC_CR_PLLON | RCC_CR_HSEBYP | RCC_CR_HSEON);

    // Włączamy oscylator HSE z rezonatorem kwarcowym
    RCC->CR |= RCC_CR_HSEON;

    //Czekamy, aż oscylator „zaskoczy”
    while (!(RCC->CR & RCC_CR_HSERDY))
        __NOP();

    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    uint32_t reg = RCC->CFGR;;

    // Konfigurujemy podzielnik dla fHCLK
    reg &= ~RCC_CFGR_HPRE;
    reg |= RCC_CFGR_HPRE_DIV1;

    // Konfigurujemy podzielnik dla fPCLK2
    reg &= ~RCC_CFGR_PPRE2;
    reg |= RCC_CFGR_PPRE2_DIV1;

    // Konfigurujemy podzielnik dla fPCLK1
    reg &= ~RCC_CFGR_PPRE1;
    reg |= RCC_CFGR_PPRE1_DIV2;

    // Aplikujemy nowe podzielniki
    RCC->CFGR = reg;

    RCC->PLLCFGR = M | (N << 6) | (((P >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSE) | (Q << 24);

    // Włączamy oscylator pętli
    RCC->CR |= RCC_CR_PLLON;

    // Czekamy na zsynchronizowanie się pętli
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
        __NOP();

    // Żeby nie spowalniać pracy mikrokontrolera, trzeba włączyć pmięci podręczne flash
    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_3WS;

    // Ustawiamy fSYSCLK = fPLL OUT
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    // To chwilę trwa, czekamy na spełnienie warunku
    while ((RCC->CFGR & RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL)
        __NOP();

    SystemCoreClock = 96000000;
}

void Turn100MHzClock()
{
    ChangeClock(8, 384, 4, 8);
}

inline uint32_t GetSystemCoreClock()
{
    return SystemCoreClock;
}

void SleepAndWaitForInterrupts()
{
    __WFI();
}