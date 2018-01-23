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
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY))
		__NOP();

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

	RCC->PLLCFGR = M | (N << 6) | (((P >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSE) | (Q << 24);

    // Enable the main PLL
    RCC->CR |= RCC_CR_PLLON;

    // Wait till the main PLL is ready 
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
	    __NOP();

    FLASH->ACR = FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_3WS;

	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    // Wait till the main PLL is used as system clock source
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL)
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