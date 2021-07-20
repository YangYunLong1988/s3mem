
/*
 * sleepmemtester                                                   version 1
 *
 * memaddr.h
 * Header file of memaddr.c
 *
 * Memory tester utility. Aim for system sleep/resume
 * memory verification.
 *
 * version 1 by Jonathan Wang <juquanff@gmail.com>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 */

#ifndef _MEM_ADDR_H_
#define _MEM_ADDR_H_

#include "types.h"

/* Function statement*/
int mem_pagesize(void);
void mem_phyaddr(ul vaddr, ul *paddr);

#endif