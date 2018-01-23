#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

void Turn100MHzClock();

uint32_t GetSystemCoreClock();

void SleepAndWaitForInterrupts();

#define PCLK1_HZ (GetSystemCoreClock() / 2)

#endif