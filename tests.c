/*
 * sleepmemtester                                   version 1
 *
 * tests.c
 * Test program instance.
 *
 * Memory tester utility. Aim for system sleep/resume
 * memory verification.
 *
 * This program developed based on memtester version 4.
 * version 1 by Jonathan Wang <juquanff@gmail.com>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 */
/*
 * Very simple but very effective user-space memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004-2020 Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 * This file contains the functions for the actual tests, called from the
 * main routine in memtester.c.  See other comments in that file.
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "types.h"
#include "sizes.h"
#include "sleepmemtester.h"
#include "memaddr.h"

char progress[] = "-\\|/";
#define PROGRESSLEN 4
#define PROGRESSOFTEN 2500
#define ONE 0x00000001L

union {
    unsigned char bytes[UL_LEN/8];
    ul val;
} mword8;

union {
    unsigned short u16s[UL_LEN/16];
    ul val;
} mword16;

/* Function definitions. */

int compare_regions(ulv *bufa, ulv *bufb, size_t count) {
    int r = 0;
    size_t i;
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    off_t physaddr;
    ul  phyaddtemp;
    ul  errorCount = 0;

    printf("\n");
    for (i = 0; i < count; i++, p1++, p2++)
    {
        if (*p1 != *p2)
        {
            errorCount++;

            if (use_phys)
            {
                physaddr = physaddrbase + (i * sizeof(ul));
                print_with_log("FAILURE: 0x%016x != 0x%016x at physical address "
                        "0x%016x.\n",
                        (ul) *p1, (ul) *p2, physaddr);
            }
            else
            {
                if(((ul) *p1) != sleeptestvalue)
            	{
            		mem_phyaddr((ul)p1, &phyaddtemp);
                    print_with_log("FAILURE: 0x%016llx (Phy - %016llX) != %016llx.\n",
                            (ul) *p1, phyaddtemp, sleeptestvalue);
            	}

                if (((ul) *p2) != sleeptestvalue)
                {
                    mem_phyaddr((ul)p2, &phyaddtemp);
                    print_with_log("FAILURE: 0x%016llx (Phy - %016llX) != %016llx.\n",
                            (ul) *p2, phyaddtemp, sleeptestvalue);
                }
            }
            r = -1;
        }
    }

    print_with_log("Total failed count: %d\n", errorCount);

    return r;
}

void sleep_system(struct sleep_param param)
{
    int sysStatefd, rtcTimerfd;
    char rtcwakeparam[20];

    // set timer
    sprintf(rtcwakeparam, "+%d", param.sleeptime);
    rtcTimerfd = open("/sys/class/rtc/rtc0/wakealarm", O_WRONLY);
    if (rtcTimerfd == -1)
    {
        print_with_log("failed to open /sys/class/rtc/rtc0/wakealarm for wake timer setting!\n%s\n", strerror(errno));
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
            print_with_log("failed to open /sys/power/state for system sleep!\n%s\n", strerror(errno));
            exit(0);
        }
        sleep(2);
        write(sysStatefd, param.sleeptype, strlen(param.sleeptype));
        close(sysStatefd);
    }
    else
    {
        print_with_log("Set wake up timer failed!\n");
    }

    close(rtcTimerfd);
    return;
}

int test_sleep_mem(ulv *bufa, ulv *bufb, size_t count) {
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    ul j = 0;
    size_t i;

    putchar(' ');
    fflush(stdout);
    for (i = 0; i < count; i++) {
        *p1++ = *p2++ = sleeptestvalue;
        if (!(i % PROGRESSOFTEN)) {
            putchar('\b');
            putchar(progress[++j % PROGRESSLEN]);
            fflush(stdout);
        }
    }
    printf("\b \b");
    fflush(stdout);

    return 0;
}
