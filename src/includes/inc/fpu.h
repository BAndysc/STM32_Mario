#ifndef _FPU_H
#define _FPU_H 1

#include <stm32.h>

/* Floating point hardware must be enabled before calling any function
which uses floating point instructions. This could be done in the
startup before main is called. */
static inline void FPUenable(void) {
  #if __FPU_USED == 1
  SCB->CPACR |= 0xf << 20; /* CoProcessor Access Control Register */
  __DSB(); /* Data Synchronization Barrier */
  __ISB(); /* Instruction Synchronization Barrier */
  #endif
}

#endif
