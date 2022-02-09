/*
 * sleepmemtester                                   version 1
 *
 * memaddr.c
 * Provide memory address related method.
 *
 * Memory tester utility. Aim for system sleep/resume
 * memory verification.
 *
 * version 1 by Jonathan Wang <juquanff@gmail.com>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "memaddr.h"
/**
 * provide method to get system page size.

  @param  none

  @retval pagesize      System page size.

**/
#ifdef _SC_PAGE_SIZE
int mem_pagesize(void) {
    int pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1) {
        print_with_log("get page size failed");
        exit(EXIT_FAIL_NONSTARTER);
    }
    //printf("pagesize is %ld\n", (long) pagesize);
    return pagesize;
}
#else
int mem_pagesize(void) {
    //printf("sysconf(_SC_PAGE_SIZE) not supported; using getpagesize()\n");
    return getpagesize();
}
#endif

/**
 * provide method to decode process virtual address to physical address.

  @param  vaddr         The virtual address to be decode.

  @param  *paddr        The pointer of phisical address buffer.

  @retval none

**/
void mem_phyaddr(ul vaddr, ul *paddr)
{
    int pageSize = mem_pagesize(); // Get Page size

    ul v_pageIndex = vaddr / pageSize; // Calculate virtual page index
    ul v_offset = v_pageIndex * sizeof(ul); // Calculate virtual page offset
    ul page_offset = vaddr % pageSize; // calculate relative page offset
    ul item = 0;

    int fd = open("/proc/self/pagemap", O_RDONLY);
    if(fd == -1)
    {
        print_with_log("open /proc/self/pagemap error\n");
        return;
    }

    if(lseek(fd, v_offset, SEEK_SET) == -1)
    {
        print_with_log("sleek error\n");
        return;
    }

    if(read(fd, &item, sizeof(ul)) != sizeof(ul))
    {
        print_with_log("read item error\n");
        return;
    }

    if((((ul)1 << 63) & item) == 0)
    {
        print_with_log("page present is 0\n");
        return ;
    }

    ul phy_pageIndex = (((ul)1 << 55) - 1) & item;

    *paddr = (phy_pageIndex * pageSize) + page_offset;
}