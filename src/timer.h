#ifndef TIMER_H
#define TIMER_H

#include "stm32.h"
#include <stdint.h>

typedef struct {
	TIM_TypeDef* Tim;
	void (*handler)(void* data);
	void* handlerData;
} Timer;

typedef enum {
	TIMER_DIRECTION_UP,
	TIMER_DIRECTION_DOWN,
	TIMER_DIRECTION_UP_DOWN,
} TimerDirection;

void InitTimer3();
void InitTimer4();
void SetTimerHandler(void (*func)());

void InitTimer32(Timer* timer, TIM_TypeDef* ptr, uint32_t prescaler, uint32_t toReach, TimerDirection dir);
void InitTimer16(Timer* timer, TIM_TypeDef* ptr, uint16_t prescaler, uint16_t toReach, TimerDirection dir);

void TimerStart(Timer* timer);
void TimerStop(Timer* timer);

void TimerSetDirection(Timer* timer, TimerDirection dir);

void TimerSetHandler(Timer* timer, void (*func)(void* data), void* data);
#endif 