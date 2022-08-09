#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <osDepsLib.h>
#include <commonlib.h>
#include <errno.h>

char progress[] = "-\\|/";
#define PROGRESSLEN 4
#define PROGRESSOFTEN 2500

int fillMem(
    uint64_t volatile  *bufa,
    uint64_t volatile  *bufb,
    uint64_t           testData,
    size_t             count)
{
    uint64_t volatile  *p1 = bufa;
    uint64_t volatile  *p2 = bufb;
    uint64_t            j = 0;
    size_t              i;

    putchar(' ');
    fflush(stdout);
    for (i = 0; i < count; i++) {
        *p1++ = *p2++ = testData;
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

int verifyMem(
    uint64_t volatile  *bufa,
    uint64_t volatile  *bufb,
    uint64_t           testData,
    size_t             count)
{
    int                status = 0;
    uint64_t volatile  *p1 = bufa;
    uint64_t volatile  *p2 = bufb;
    uint64_t           physaddr;
    uint32_t           errorCount = 0;

    printf("\n");
    for (uint32_t Index = 0; Index < count; Index++, p1++, p2++)
    {
        if (*p1 != *p2)
        {
            errorCount++;

            if((*p1) != testData)
            {
                virtToPhy(p1, &physaddr);
                print("FAILURE: 0x%016llx (Phy - %016llX) != %016llx.\n",
                        *p1, physaddr, testData);
            }

            if ((*p2) != testData)
            {
                virtToPhy(p2, &physaddr);
                print("FAILURE: 0x%016llx (Phy - %016llX) != %016llx.\n",
                        *p2, physaddr, testData);
            }
            status = EFAULT;
        }
    }

    print("Total failed count: %d\n", errorCount);

    return status;
}