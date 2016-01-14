/*
 * sockets_common.h - support for cross-platform sockets
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

#ifndef SOCKETS_COMMON_H
#define SOCKETS_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include "vmci_sockets.h"
#include "list.h"

/* Socket types*/

typedef int SOCKET_HANDLE;


/* SOCKET_LISTSs types */

typedef int (*socket_event_handler_func_t)(SOCKET_HANDLE socket, void *context_data);

typedef struct {
	SOCKET_HANDLE socket;
	socket_event_handler_func_t handler;
	void *context_data;
	char *label;
} SOCKET_ENTRY;


typedef struct {
	list_t sockets;
} SOCKET_LIST;


/* SOCKET_LISTSs functions */

int socket_list_init(SOCKET_LIST *socket_list);

int socket_list_insert(SOCKET_LIST * socket_list, SOCKET_HANDLE socket, socket_event_handler_func_t handler, void * context_data, int context_len);

int socket_list_insert(SOCKET_LIST *socket_list, SOCKET_HANDLE socket,
	socket_event_handler_func_t handler,
	void *context_data, int context_len);

int socket_list_delete(SOCKET_LIST *socket_list, SOCKET_HANDLE socket);

void socket_list_debug_dump(SOCKET_LIST *socket_list);

void socket_list2fd_set(SOCKET_LIST * socket_list, fd_set * socket_mask);

void socket_list_select_and_handle_events(SOCKET_LIST * socket_list);


#endif