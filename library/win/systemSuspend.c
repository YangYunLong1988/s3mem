#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <synchapi.h>
#include <powrprof.h>

HANDLE          RtcTimer = NULL;

int suspendSystem(uint64_t Delay)
{
    int64_t        NanoSecs, Secs;
    LARGE_INTEGER   due;

    if(RtcTimer == NULL)
    {
        RtcTimer = CreateWaitableTimer(NULL, TRUE, "RTCTimer");
        if(RtcTimer == NULL)
        {
            return EXIT_FAILURE;
        }
    }

    Secs = (int64_t)Delay;
    NanoSecs = -Secs * 1000 * 1000 * 10;
    due.LowPart = (DWORD) (NanoSecs & 0xFFFFFFFF);
    due.HighPart = (LONG) (NanoSecs >> 32);

    if(!SetWaitableTimer(RtcTimer, &due, 0, NULL, NULL, TRUE))
    {
        printf("SetWaitableTimer failed (%u)\n", GetLastError());
        return EXIT_FAILURE;
    }

    // Go to Sleep
    if(!SetSuspendState(FALSE, TRUE, FALSE))
    {
        printf("SetSuspendState failed (%u)\n", GetLastError());
        return EXIT_FAILURE;
    }

    // Wait for the timer.

    if (WaitForSingleObject(RtcTimer, INFINITE) != WAIT_OBJECT_0)
    {
        printf("WaitForSingleObject failed (%d)\n", GetLastError());
    }

    // Wakeup now

    return EXIT_SUCCESS;
}

int SleepInSeconds(
    uint32_t Seconds
)
{
    Sleep(Seconds * 1000);

    return 0;
}
