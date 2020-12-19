#include "simple_watchdog.h"
#include "stm32.h"

#define WATCHDOG_START 0xCCCC
#define WATCHDOG_PING 0xAAAA
#define WATCHDOG_ENABLE_WRITE 0x5555

void StartWatchdog()
{
    IWDG->KR = WATCHDOG_ENABLE_WRITE;

    IWDG->PR = 256;   // prescaler 256
    IWDG->RLR = 125;  // reload 125

    IWDG->KR = WATCHDOG_PING;
    IWDG->KR = WATCHDOG_START;
}

void WatchdogPingAlive()
{
    IWDG->KR = WATCHDOG_PING;
}