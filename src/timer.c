#include "timer.h"
#include "interrupts.h"
#include "device.h"

void (*hfunc)();

static void handler(void* data)
{
	hfunc();
}

Timer timer4;
void InitTimer4()
{
	InitTimer16(&timer4, TIM4, 6500, 450, TIMER_DIRECTION_DOWN);
	TimerSetHandler(&timer4, &handler, 0);
	TimerStart(&timer4);

}

uint32_t TICKS =0;

void handlerSek(void* data)
{
	++TICKS;
}

Timer timer3;
void InitTimer3()
{
	InitTimer16(&timer3, TIM3, 9600, 10, TIMER_DIRECTION_UP);
	TimerSetHandler(&timer3, &handlerSek, 0);
	TimerStart(&timer3);
}

void TimerSetDirection(Timer* timer, TimerDirection dir)
{
	timer->Tim->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS_0 | TIM_CR1_CMS_1);
	switch (dir)
	{
		case TIMER_DIRECTION_UP:
			// no flag
			break;
		case TIMER_DIRECTION_DOWN:
			timer->Tim->CR1 |= TIM_CR1_DIR;
			break;
		case TIMER_DIRECTION_UP_DOWN:
			timer->Tim->CR1 |= TIM_CR1_CMS_0 | TIM_CR1_CMS_1;
			break;
	}
}


void TimerStart(Timer* timer)
{
	timer->Tim->CR1 |= TIM_CR1_CEN;
}

void TimerStop(Timer* timer)
{
	timer->Tim->CR1 &= ~TIM_CR1_CEN;
}

void TimerSetHandler(Timer* timer, void (*func)(void* data), void* data)
{
	timer->handler = func;
	timer->handlerData = data;
}

void HandleTimerFinish(void* data)
{
	Timer* timer = (Timer*)data;

	if (timer->handler)
		timer->handler(timer->handlerData);
}

void InitTimer32(Timer* timer, TIM_TypeDef* ptr, uint32_t prescaler, uint32_t toReach, TimerDirection dir)
{
	TurnTimerClockOn(ptr);
	timer->Tim = ptr;
	timer->Tim->PSC = prescaler;
	timer->Tim->ARR = toReach;

	AddInterruptHandler(GetInterruptForTimer(ptr), INTERRUPT_PRIORITY_HIGH, &HandleTimerFinish, timer);
	timer->Tim->DIER = TIM_DIER_UIE;
}

void InitTimer16(Timer* timer, TIM_TypeDef* ptr, uint16_t prescaler, uint16_t toReach, TimerDirection dir)
{
	TurnTimerClockOn(ptr);
	timer->Tim = ptr;
	timer->Tim->PSC = prescaler;
	timer->Tim->ARR = toReach;
	TimerSetDirection(timer, dir);

	AddInterruptHandler(GetInterruptForTimer(ptr), INTERRUPT_PRIORITY_HIGH, &HandleTimerFinish, timer);
	timer->Tim->DIER = TIM_DIER_UIE;
}

void SetTimerHandler(void (*func)())
{
	hfunc = func;
}