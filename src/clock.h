#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

void Turn100MHzClock();

uint32_t GetSystemCoreClock();

void SleepAndWaitForInterrupts();

#endif