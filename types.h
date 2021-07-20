/*
 * sleepmemtester                                   version 1
 *
 * types.h
 * Data types definition.
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
 * This file contains typedefs, structure, and union definitions.
 *
 */
#ifndef _TYPE_H_
#define _TYPE_H_

#include "sizes.h"

typedef unsigned long ul;
typedef unsigned long long ull;
typedef unsigned long volatile ulv;
typedef unsigned char volatile u8v;
typedef unsigned short volatile u16v;

#define EXIT_FAIL_NONSTARTER    0x01
#define EXIT_FAIL_ADDRESSLINES  0x02
#define EXIT_FAIL_OTHERTEST     0x04

struct test {
    char *name;
    int (*fillmem)();
    int (*verfmem)();
};

struct sleep_param {
    char *sleeptype;
    unsigned long sleeptime;
    unsigned long waketime;
};

#endif