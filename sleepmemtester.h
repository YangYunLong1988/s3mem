/*
 * sleepmemtester                                   version 1
 *
 * sleepmemtester.h
 * Header file of sleepmemtester.c
 *
 * Memory tester utility. Aim for system sleep/resume
 * memory verification.
 *
 * version 1 by Jonathan Wang <juquanff@gmail.com>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 */
/*
 * Very simple (yet, for some reason, very effective) memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004-2020 Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 * This file contains the declarations for external variables from the main file.
 * See other comments in that file.
 *
 */

#include <sys/types.h>

/* extern declarations. */

extern int use_phys;
extern off_t physaddrbase;
extern unsigned long sleeptestvalue;
