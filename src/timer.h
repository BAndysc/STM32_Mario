#ifndef TIMER_H
#define TIMER_H

#include "stm32.h"
#include "interrupts.h"
#include <stdint.h>

typedef struct Timer {
	TIM_TypeDef* Tim;
    void (*start)(struct Timer* timer);
    void (*bind)(struct Timer* timer, void (*handler)(void* data), void* data, InterruptPriority priority);
	void (*handler)(void* data);
	void* handlerData;

    InterruptType _interrupt;
} Timer;

typedef enum {
	TIMER_DIRECTION_UP,
	TIMER_DIRECTION_DOWN,
	TIMER_DIRECTION_UP_DOWN,
} TimerDirection;


void InitNextTimer16(Timer* timer, uint32_t us, TimerDirection direction);

#endif