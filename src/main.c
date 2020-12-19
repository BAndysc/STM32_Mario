#include "clock.h"
#include "game/game.h"
#include "game/setup.h"
#include "simple_watchdog.h"

int main()
{
    StartWatchdog();

    Turn100MHzClock();

    SetupSerial();

    SetupMillisecondsTimer();

    SetupUpdateWorldTimer();

    SetupDrawTimer();

    InitGame();

    SetupJoystick();

    SetupLCD();

    while(1)
    {
        WatchdogPingAlive();
 
        MainLoopPass();

        SleepAndWaitForInterrupts();
    }
}
