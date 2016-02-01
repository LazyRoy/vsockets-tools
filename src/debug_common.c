/*
 * debug_common.c - support for cross-platform debug logging
 *
 * Copyright (C) 2016 Pedro Mendes da Silva
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


#include <stdlib.h>

#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>

#include "list.h"

#include "debug_common.h"
#include "vsockets_tools_version.h"

#include <time.h>
#include <sys/time.h>

char *debug_get_time_str_now()
{
	struct timeval current_time;
	struct tm *current_time_aux;

	static char c_time_string[200];

	/* Obtain current time. */
	gettimeofday(&current_time, NULL);
	current_time_aux = gmtime(&current_time.tv_sec);

	sprintf(c_time_string, "%04u-%02u-%02u %02u:%02u:%02u.%06lu",
		current_time_aux->tm_year + 1900,
		current_time_aux->tm_mon + 1,
		current_time_aux->tm_mday,
		current_time_aux->tm_hour,
		current_time_aux->tm_min,
		current_time_aux->tm_sec,
		current_time.tv_usec);

	return c_time_string;
}


char *debug_get_time_stack_tabs()
{
	static char c_tab_string[20000];

	int i;

	c_tab_string[0] = '\0';

	for (i = 0; i < debug_stack_depth; i++) {
		strcat(c_tab_string, "    ");
	}

	return c_tab_string;
}

static int debug_status = FALSE;
int debug_stack_depth = 0;


void debug_start_message()
{
	debug_printf("############## Debug starting ##############");
	debug_printf("vsockets tools version=%s\n", VSOCKETS_TOOLS_VERSION_STRING);
}

void debug_enable()
{
	debug_status = TRUE;
	debug_stack_depth = 0;

	debug_start_message();
}

void debug_disable()
{
	debug_status = FALSE;
}

int debug_is_active()
{
	return debug_status == TRUE;
}



