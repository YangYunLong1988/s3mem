#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <commonlib.h>
#include <errno.h>

int suspendSystem(uint64_t Delay)
{
    int sysStatefd, rtcTimerfd;
    char rtcwakeparam[20];

    // set timer
    sprintf(rtcwakeparam, "+%d", Delay);
    rtcTimerfd = open("/sys/class/rtc/rtc0/wakealarm", O_WRONLY);
    if (rtcTimerfd == -1)
    {
        print("failed to open /sys/class/rtc/rtc0/wakealarm for wake timer setting!\n%s\n", strerror(errno));
        exit(0);
    }

    //Clear timer
    write(rtcTimerfd, "0", 1);

    if (write(rtcTimerfd, rtcwakeparam, strlen(rtcwakeparam)) != -1)
    {
        //fall a sleep
        sysStatefd = open("/sys/power/state", O_WRONLY);
        if (sysStatefd == -1)
        {
            print("failed to open /sys/power/state for system sleep!\n%s\n", strerror(errno));
            exit(EXIT_SUCCESS);
        }
        sleep(2);
        write(sysStatefd, "mem", 3);
        close(sysStatefd);
    }
    else
    {
        print("Set wake up timer failed!\n");
    }

    close(rtcTimerfd);
}