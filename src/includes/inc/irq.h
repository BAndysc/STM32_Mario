#ifndef _IRQ_H
#define _IRQ_H 1

#include <stm32.h>

/* We define four preemption priority levels and four subpriorities. */
/* WARNING: Calling IRQprotect() with VERY_HIGH_IRQ_PRIO has no effect. */

#define VERY_HIGH_IRQ_PRIO     0U
#define HIGH_IRQ_PRIO          1U
#define MIDDLE_IRQ_PRIO        2U
#define LOW_IRQ_PRIO           3U

#define VERY_HIGH_IRQ_SUBPRIO  0U
#define HIGH_IRQ_SUBPRIO       1U
#define MIDDLE_IRQ_SUBPRIO     2U
#define LOW_IRQ_SUBPRIO        3U

typedef uint32_t irq_level_t;

/* CMSIS >= 2.00 defines inline functions, hence
   there is no more need for using assembler macros. */

#if __CORTEX_M == 0
  /* Cortex-M0 does not have the BASEPRI register and it does not
     allow to disable interrupts depending on an preemption priotity.
     There are 2 bits reserved for the preemption priority. There are
     no subpriorities. */

  /* This function is defined for compatibility with Cortex-M3/M4. */
  static inline void IRQprotectionConfigure(void) {
  }

  static inline irq_level_t IRQprotect(uint32_t priority) {
    volatile irq_level_t level;
    level = __get_PRIMASK();
    __disable_irq();
    return level;
  }

  static inline void IRQunprotect(irq_level_t level) {
    __set_PRIMASK(level);
  }

  /* This function is defined for compatibility with Cortex-M3/M4. */
  static inline void IRQsetPriority(IRQn_Type irq, uint32_t prio, uint32_t subprio) {
    NVIC_SetPriority(irq, prio);
  }

#else /* __CORTEX_M != 0 */
  /* Cortex-M3 and Cortex-M4 allow to disable interrupts depending on
     the preemption priority. */

  #define PREEMPTION_PRIORITY_BITS  2U

  static inline void IRQprotectionConfigure(void) {
    NVIC_SetPriorityGrouping(7U - PREEMPTION_PRIORITY_BITS);
  }

  static inline irq_level_t IRQprotect(uint32_t priority) {
    irq_level_t level;
    level = __get_BASEPRI();
    priority <<= 8U - PREEMPTION_PRIORITY_BITS;
    /* Do not lower the priority or do not set the same priority.
       The condition level == 0 means that protection was not activated. */
    if (level == 0 || priority < level) {
       __set_BASEPRI(priority);
    }
    return level;
  }

  static inline void IRQunprotect(irq_level_t level) {
    __set_BASEPRI(level);
  }

  #undef PREEMPTION_PRIORITY_BITS

  static inline void IRQsetPriority(IRQn_Type irq, uint32_t prio, uint32_t subprio) {
    NVIC_SetPriority(irq, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), prio, subprio));
  }

#endif /* __CORTEX_M */

/* Disable all interrupts. Return the previous interrupt activation state. */
static inline irq_level_t IRQprotectAll(void) {
  volatile irq_level_t level;
  level = __get_PRIMASK();
  __disable_irq();
  return level;
}

/* Restore previous interrupt activation state. */
static inline void IRQunprotectAll(irq_level_t level) {
  __set_PRIMASK(level);
}

#endif /* _IRQ_H */
