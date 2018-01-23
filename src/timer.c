#include "timer.h"
#include "interrupts.h"
#include "device.h"
#include "clock.h"
#include "debug.h"

#define MICROSECONDS_IN_SECOND 1000000

typedef struct
{
    uint32_t prescaler;
    uint32_t arr;
} TimerSettings;


static TimerSettings CalculateTimerSettingsUs(uint32_t us, uint8_t bits)
{
    TimerSettings settings;

    settings.arr = GetSystemCoreClock() / MICROSECONDS_IN_SECOND * us;
    settings.prescaler = 1; // let's start with no prescaler (PSC = 0)

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

static void EnableTimerOnce(Timer* timer)
{
    timer->Tim->CR1 |= TIM_CR1_OPM | TIM_CR1_CEN;
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

static void Init(Timer* timer, DeviceTimer* device, uint32_t us, TimerDirection direction, int8_t bits)
{
    TurnTimerClockOn(device->Ptr);

    timer->Tim = device->Ptr;
    timer->start = &EnableTimer;
    timer->startOnce = &EnableTimerOnce;
    timer->bind = &BindTimer;
    timer->_interrupt = device->Interrupt;

    TimerSettings settings = CalculateTimerSettingsUs(us, 16);
    timer->Tim->PSC = settings.prescaler;
    timer->Tim->ARR = settings.arr;
    TimerSetDirection(timer, direction);
}


void InitNextTimer16(Timer* timer, uint32_t us, TimerDirection direction)
{
    DeviceTimer device = GetNextUnusedTimer16();

    if (!device.Ptr)
        Abort("All 16 bit timers are already used! Aborting!");

    Init(timer, &device, us, direction, 16);
}

void InitNextTimer32(Timer* timer, uint32_t us, TimerDirection direction)
{
    DeviceTimer device = GetNextUnusedTimer32();

    if (!device.Ptr)
        Abort("All 32 bit timers are already used! Aborting!");

    Init(timer, &device, us, direction, 32);
}
