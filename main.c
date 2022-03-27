
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "commonlib.h"
#include "osDepsLib.h"
#include "getopt.h"
#include "config.h"

#define MAX_ARCH_MEM_SIZE_IN_BYTE   (size_t)-1

#define DEFAULT_TEST_LOOPS      1                       // 1 Cycle
#define DEFAULT_TEST_MEMORYSIZE 4096                    // 4 KB
#define DEFAULT_TEST_DELAY      30                      // 30 seconds
#define DEFAULT_TEST_DATA       0xFFFFFFFFFFFFFFFF

void usage(char *me)
{
    fprintf(stderr, "\n"
            "Usage: %s [-s sleeptime(s)] [-t waketime(s)] [-v testvalue(hex)][-j journalfile] <mem>[B|K|M|G] [loops]\n",
            me);
    exit(EXIT_SUCCESS);
}

void startupMessages(void)
{
    printf("%s Version %s\n", PROJECT_NAME, PROJECT_VER);
    printf("Copyright (C) 2022 Jonathan Wang.\n");
    printf("Licensed under the GNU General Public License version 2 (only).\n\n");
}

int main(int argc, char *argv[])
{
    int         cmdOption;
    uint64_t    suspendDelay = DEFAULT_TEST_DELAY, wakeDelay = DEFAULT_TEST_DELAY;
    uint64_t    testData = DEFAULT_TEST_DATA;
    uint64_t    testTotalLoops = DEFAULT_TEST_LOOPS;
    char        *dataSuffix, *memsuffix, *loopsuffix;
    size_t      wantRaw, memorySizeInByte = DEFAULT_TEST_MEMORYSIZE;
    int         memsizeShift;
    void        volatile *testMemArea = NULL;

    // Start
    startupMessages();
    errno = AcquirePrivilidge();
    if(errno != 0)
    {
        // Exit the program if failed to acquire requested privilidge
        fprintf(stderr, "ERROR: Failed to acquire privilidges!\n");
        exit(EXIT_FAILURE);
    }

    // Parse the commandline parameters
    while ((cmdOption = getopt(argc, argv, "s:t:v:j:")) != -1)
    {
        switch (cmdOption) {
            case 's':
                errno = 0;
                suspendDelay = (uint64_t)strtoull(optarg, &dataSuffix, 10);
                if (errno != 0) {
                    fprintf(stderr,
                            "failed to parse sleep time arg; should be Decimal "
                            "sleeptime (123...)\n");
                    usage(argv[0]); /* doesn't return */
                }
                if (*dataSuffix != '\0') {
                    /* got an invalid character in the address */
                    fprintf(stderr,
                            "failed to parse sleep time arg; should be Decimal "
                            "address (123...)\n");
                    usage(argv[0]); /* doesn't return */
                }
                break;
            case 't':
                errno = 0;
                wakeDelay = (uint64_t)strtoull(optarg, &dataSuffix, 10);
                if (errno != 0) {
                    fprintf(stderr,
                            "failed to parse wake time arg; should be Decimal "
                            "sleeptime (123...)\n");
                    usage(argv[0]); /* doesn't return */
                }
                if (*dataSuffix != '\0') {
                    /* got an invalid character in the address */
                    fprintf(stderr,
                            "failed to parse wake time arg; should be Decimal "
                            "waketime (123...)\n");
                    usage(argv[0]); /* doesn't return */
                }
                break;
            case 'v':
                errno = 0;
                testData = (uint64_t)strtoull(optarg, &dataSuffix, 16);
                if (errno != 0) {
                    fprintf(stderr,
                            "failed to parse test value arg; should be hex "
                            "testvalue (0x123...)\n");
                    usage(argv[0]); /* doesn't return */
                }
                if (*dataSuffix != '\0') {
                    /* got an invalid character in the address */
                    fprintf(stderr,
                            "failed to parse test value arg; should be hex "
                            "testvalue (0x123...)\n");
                    usage(argv[0]); /* doesn't return */
                }
                break;
            case 'j':
                errno = 0;
                errno = openLogFile(optarg);
                if(errno == EEXIST)
                {
                    fprintf(stderr,
                            "Log file %s already exists. Delete it? (Y/N)", optarg);
                    //exit(0);
                    if((getchar() == 'y') || (getchar() == 'Y'))
                    {
                        remove(optarg);
                        if(!openLogFile(optarg))
                        {
                            break;
                        }
                    }
                    else
                    {
                         exit(0);
                    }
                }
                else if (errno != 0)
                {
                    fprintf(stderr,
                            "open log file failed for error(%s) ==> %s\n", optarg, strerror(errno));
                    exit(0);
                }
                break;
            default: /* '?' */
                usage(argv[0]); /* doesn't return */
        }
    }

    // Parse memory size from commandline
    if (optind >= argc) {
        fprintf(stderr, "need memory argument, in MB\n");
        usage(argv[0]); /* doesn't return */
    }

    errno = 0;
    wantRaw = (size_t) strtoull(argv[optind], &memsuffix, 0);
    if (errno != 0) {
        fprintf(stderr, "failed to parse memory argument");
        usage(argv[0]); /* doesn't return */
    }
    switch (*memsuffix) {
        case 'G':
        case 'g':
            memsizeShift = 30; /* gigabytes */
            break;
        case 'M':
        case 'm':
            memsizeShift = 20; /* megabytes */
            break;
        case 'K':
        case 'k':
            memsizeShift = 10; /* kilobytes */
            break;
        case 'B':
        case 'b':
            memsizeShift = 0; /* bytes*/
            break;
        case '\0':  /* no suffix */
            memsizeShift = 20; /* megabytes */
            break;
        default:
            /* bad suffix */
            usage(argv[0]); /* doesn't return */
    }
    memorySizeInByte = ((size_t) wantRaw << memsizeShift);
    // wantBytes_orig = wantBytes = ((size_t) wantRaw << memsizeShift);
    // wantMb = (wantBytes_orig >> 20);

    // if (wantMb > (MAX_ARCH_MEM_SIZE_IN_BYTE >>20 + 1)) {
    //     fprintf(stderr, "This system can only address %llu MB.\n", (uint64_t) (MAX_ARCH_MEM_SIZE_IN_BYTE >>20 + 1));
    //     exit(EXIT_FAILURE);
    // }
    // if (wantBytes < pagesize) {
    //     fprintf(stderr, "bytes %ld < pagesize %ld -- memory argument too large?\n",
    //             wantbytes, pagesize);
    //     exit(EXIT_FAILURE);
    // }

    // parse test loops from commandline
    optind++;
    if (optind >= argc) {
        // Use default test loop setting
        testTotalLoops = DEFAULT_TEST_LOOPS;
    } else {
        errno = 0;
        testTotalLoops = strtoull(argv[optind], &loopsuffix, 0);
        if (errno != 0) {
            fprintf(stderr, "failed to parse total test loops");
            usage(argv[0]); /* doesn't return */
        }
        if (*loopsuffix != '\0') {
            fprintf(stderr, "loop suffix %c\n", *loopsuffix);
            usage(argv[0]); /* doesn't return */
        }
    }
    // print("Test parameters sleep %d wake %d test data %llx mem %llx loop %d\n", \
    //     suspendDelay, wakeDelay, testData, memorySizeInByte, testTotalLoops);
    // prepare memory for test
    errno = prepareTestMemArea(&memorySizeInByte, &testMemArea);
    if(errno != 0)
    {
        fprintf(stderr, "Failed to prepare test memory ==> %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // main loop for test
    for(uint32_t Loop = 0; Loop < testTotalLoops; Loop++)
    {
        size_t                halflen, count;
        uint64_t    volatile  *bufa, *bufb;

        halflen = memorySizeInByte / 2;
        count = halflen / sizeof(uint64_t);
        bufa = (uint64_t volatile *) testMemArea;
        bufb = (uint64_t volatile *) ((size_t) testMemArea + halflen);

        // Fill test data
        fillMem(bufa, bufb, testData, count);

        // Call system suspend
        suspendSystem(suspendDelay);

        // Test data
        if(verifyMem(bufa, bufb, testData, count))
        {
            // Test failed
            // print("memory test failed！ Press any key to continue...");
            // if(debugTestMode)
            // getchar();
            print("memory test failed！\n");
        }
    }


    return EXIT_SUCCESS;
}
