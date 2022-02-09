/*
 * sleepmemtester                                   version 1
 *
 * tests.h
 * Header file for tests.c.
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
 * Very simple yet very effective memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004-2020 Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 * This file contains the declarations for the functions for the actual tests,
 * called from the main routine in memtester.c.  See other comments in that
 * file.
 *
 */
#ifndef _TESTS_H_
#define _TESTS_H_

#include "types.h"

/* Function declaration. */
int test_sleep_mem(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count);
int compare_regions(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count);
void sleep_system(struct sleep_param);
void print_with_log(
    char *Format,
    ...
);

#endif