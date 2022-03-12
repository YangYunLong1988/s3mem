#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <commonlib.h>
#include <errno.h>

/**
 * provide method to get system page size.

  @param  none

  @retval pagesize      System page size.

**/
#ifdef _SC_PAGE_SIZE
static size_t getSystemPageSize(void) {
    int pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1) {
        print("get page size failed");
        exit(EXIT_FAILURE);
    }
    //printf("pagesize is %ld\n", (long) pagesize);
    return pagesize;
}
#else
static size_t getSystemPageSize(void) {
    //printf("sysconf(_SC_PAGE_SIZE) not supported; using getpagesize()\n");
    return (size_t)getpagesize();
}
#endif

int prepareTestMemArea(
    size_t           *MemorySize,
    void    volatile **MemoryAddress
)
{
    void volatile   *TestMemoryAddress = NULL, *alignedAddress = NULL;
    size_t          TestMemSize = *MemorySize;
    size_t          systemPageSize = getSystemPageSize();
    size_t          pagesizemask = (ptrdiff_t) ~(systemPageSize - 1);
    size_t          lockedMemSize;
    int             do_mlock = 1;

    while (!TestMemoryAddress && TestMemSize) {
        TestMemoryAddress = (void volatile *) malloc(TestMemSize);
        if (!TestMemoryAddress) TestMemSize -= systemPageSize;
    }

    lockedMemSize = TestMemSize;
    printf("got  %lluMB (%llu bytes)", (size_t) TestMemSize >> 20, (size_t) TestMemSize);
    fflush(stdout);
    if (do_mlock) {
        printf(", trying mlock ...");
        fflush(stdout);
        if ((size_t) TestMemoryAddress % systemPageSize) {
            /* printf("aligning to page -- was 0x%tx\n", buf); */
            alignedAddress = (void volatile *) ((size_t) TestMemoryAddress & pagesizemask) + systemPageSize;
            /* printf("  now 0x%tx -- lost %d bytes\n", aligned,
                *      (size_t) aligned - (size_t) buf);
                */
            lockedMemSize -= ((size_t) alignedAddress - (size_t) TestMemoryAddress);
        } else {
            alignedAddress = TestMemoryAddress;
        }
        /* Try mlock */
        if (mlock((void *) alignedAddress, lockedMemSize) < 0) {
            switch(errno) {
                case EAGAIN: /* BSDs */
                    printf("over system/pre-process limit, reducing...\n");
                    free((void *) TestMemoryAddress);
                    TestMemoryAddress = NULL;
                    TestMemSize -= systemPageSize;
                    break;
                case ENOMEM:
                    printf("too many pages, reducing...\n");
                    free((void *) TestMemoryAddress);
                    TestMemoryAddress = NULL;
                    TestMemSize -= systemPageSize;
                    break;
                case EPERM:
                    printf("insufficient permission.\n");
                    printf("Trying again, unlocked:\n");
                    do_mlock = 0;
                    free((void *) TestMemoryAddress);
                    TestMemoryAddress = NULL;
                    TestMemSize = *MemorySize;
                    break;
                default:
                    printf("failed for unknown reason.\n");
                    do_mlock = 0;

            }
        } else {
            printf("locked.\n");

        }
    } else {

        printf("\n");
    }

    if (!do_mlock) printf("Continuing with unlocked memory; testing will be slower and less reliable.\n");

    /* Do alighnment here as well, as some cases won't trigger above if you
       define out the use of mlock() (cough HP/UX 10 cough). */
    if ((size_t) TestMemoryAddress % systemPageSize) {
        /* printf("aligning to page -- was 0x%tx\n", buf); */
        alignedAddress = (void volatile *) (((size_t) TestMemoryAddress & pagesizemask) + systemPageSize);
        /* printf("  now 0x%tx -- lost %d bytes\n", aligned,
         *      (size_t) aligned - (size_t) buf);
         */
        lockedMemSize -= ((size_t) alignedAddress - (size_t) TestMemoryAddress);
    } else {
        alignedAddress = TestMemoryAddress;
    }

    // Write return value
    *MemorySize = lockedMemSize;
    *MemoryAddress = alignedAddress;

    return EXIT_SUCCESS;
}

int virtToPhy(
    uint64_t volatile *VirtualAddress,
    uint64_t volatile *PhysicalAddress
)
{
    int         pageSize = getSystemPageSize(); // Get Page size

    uint64_t    v_pageIndex = (uint64_t)VirtualAddress / pageSize; // Calculate virtual page index
    uint64_t    v_offset = v_pageIndex * sizeof(uint64_t); // Calculate virtual page offset
    uint64_t    page_offset = (uint64_t)VirtualAddress % pageSize; // calculate relative page offset
    uint64_t    item = 0;
    uint64_t    phy_pageIndex;

    int fd = open("/proc/self/pagemap", O_RDONLY);
    if(fd == -1)
    {
        print("open /proc/self/pagemap error\n");
        return EIO;
    }

    if(lseek(fd, v_offset, SEEK_SET) == -1)
    {
        print("sleek error\n");
        return EIO;
    }

    if(read(fd, &item, sizeof(uint64_t)) != sizeof(uint64_t))
    {
        print("read item error\n");
        return EIO;
    }

    if((((uint64_t)1 << 63) & item) == 0)
    {
        print("page present is 0\n");
        return EFAULT;
    }

    phy_pageIndex = (((uint64_t)1 << 55) - 1) & item;

    *PhysicalAddress = (phy_pageIndex * pageSize) + page_offset;
}