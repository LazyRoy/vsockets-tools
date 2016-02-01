/*
 * debug_common.h - support for cross-platform debug logging
 *
 * Copyright (C) 2015 Pedro Mendes da Silva
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef DEBUG_COMMON_H
#define DEBUG_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "vmci_sockets.h"



void debug_enable();

void debug_disable();

int debug_is_active();

char *debug_get_time_str_now();
char *debug_get_time_stack_tabs();


extern int debug_stack_depth;


/* Reference: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html#Variadic-Macros */

#define debug_printf(...) { if (debug_is_active()) { fprintf (stderr,"(%s) %s", debug_get_time_str_now(), debug_get_time_stack_tabs()); fprintf (stderr, __VA_ARGS__); fprintf (stderr,"\n"); } }

#define DEBUG_ENTER(function) { debug_printf(">" #function ); debug_stack_depth++; }
#define DEBUG_LEAVE(function) { debug_stack_depth--; debug_printf("<" #function );  }

#endif