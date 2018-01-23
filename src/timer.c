#include "timer.h"
#include "interrupts.h"
#include "device.h"
#include "clock.h"
#include "debug.h"

typedef struct
{
	uint32_t prescaler;
	uint32_t arr;
} TimerSettings;

static TimerSettings CalculateTimerSettings(uint32_t ms, uint8_t bits)
{
	TimerSettings settings;

	settings.arr = GetSystemCoreClock() / 1000 * ms;
	settings.prescaler = 1;

	uint32_t MAX_VAL = (uint32_t)(2 << (bits - 1)) - 1;

	while (settings.arr > MAX_VAL)
	{
		settings.arr /= 2;
		settings.prescaler *= 2;
	}

	settings.prescaler -= 1;

	return settings;
}


static TimerSettings CalculateTimerSettingsUs(uint32_t us, uint8_t bits)
{
	TimerSettings settings;

	settings.arr = GetSystemCoreClock() / 1000000 * us;
	settings.prescaler = 1;

	uint32_t MAX_VAL = (uint32_t)(2 << (bits - 1)) - 1;

	while (settings.arr > MAX_VAL)
	{
		settings.arr /= 2;
		settings.prescaler *= 2;
	}

	settings.prescaler -= 1;

	return settings;
}

static void EnableTimer(Timer* timer)
{
    timer->Tim->CR1 |= TIM_CR1_CEN;
}

static void HandleTimerInterrupt(void* data)
{
    Timer* timer = (Timer*)data;

    if (timer->handler)
        timer->handler(timer->handlerData);
}

static void TimerSetDirection(Timer* timer, TimerDirection dir)
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

static void BindTimer(Timer* timer, void (*handler)(void* data), void* data, InterruptPriority priority)
{
    timer->handlerData = data;
    timer->handler = handler;
    AddInterruptHandler(timer->_interrupt, priority, &HandleTimerInterrupt, timer);
    timer->Tim->DIER = TIM_DIER_UIE;
}

void InitNextTimer16(Timer* timer, uint32_t us, TimerDirection direction)
{
    DeviceTimer device = GetNextUnusedTimer16();

    if (!device.Ptr)
        Abort("All 16 bit timers are already used! Aborting!");

    TurnTimerClockOn(device.Ptr);

    timer->Tim = device.Ptr;
    timer->start = &EnableTimer;
    timer->bind = &BindTimer;
    timer->_interrupt = device.Interrupt;

    TimerSettings settings = CalculateTimerSettingsUs(us, 16);
    timer->Tim->PSC = settings.prescaler;
    timer->Tim->ARR = settings.arr;
    TimerSetDirection(timer, direction);
}
