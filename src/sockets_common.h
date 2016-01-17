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

#ifdef LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <Windows.h>
#include <WinSock2.h>

typedef int socklen_t;
#endif

/* Socket types*/

typedef int SOCKET_HANDLE;


/* SOCKET_LISTSs types */

typedef int (*socket_event_handler_func_t)(SOCKET_HANDLE socket, void *context_data);

typedef struct {
	SOCKET_HANDLE socket;
	socket_event_handler_func_t handler;
	void *context_data;
	char *label;

	short deleted;

	short is_special_descriptor;
} SOCKET_ENTRY;


typedef struct {
	list_t sockets;

	int n_special_descriptors;
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

int socket_list_select_and_handle_events(SOCKET_LIST * socket_list);


/*
The  accept()  system  call  is used with connection - based socket types
(SOCK_STREAM, SOCK_SEQPACKET).
The  argument  sockfd is a socket

The argument addr is a pointer to a sockaddr structure.This structure
is filled in with the address of the peer socket, as known to the  com‐
munications  layer.The  exact format of the address returned addr is
determined by the  socket's  address  family  (see  socket(2)  and  the
respective  protocol  man pages).When addr is NULL, nothing is filled
in; in this case, addrlen is not used, and should also be NULL.

The addrlen argument is a value - result argument : the caller  must  ini‐
tialize  it  to contain the size(in bytes) of the structure pointed to
by addr; on return it will contain the actual size of the peer address.
*/

int socket_init_api();

int socket_accept(SOCKET_HANDLE sockfd, struct sockaddr *addr, socklen_t *addrlen);
int socket_close(SOCKET_HANDLE sockfd);

int socket_read(int fd, void *buf, size_t count);
int socket_write(int fd, void *buf, size_t count);

int socket_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int socket_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

char* socket_handle2string(SOCKET_HANDLE socket);

#endif