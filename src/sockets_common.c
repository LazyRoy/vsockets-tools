/*
 * sockets_common.c - support for cross-platform sockets
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


#include <stdlib.h>

#include <stdio.h>

#include "vmci_sockets.h"

#include "list.h"

#include "sockets_common.h"

/* TODO: implementar umas cascas */

/*
socket(afVMCI, SOCK_STREAM, 0);

   if (bind(sockfd_stream, (struct sockaddr *) &my_addr, sizeof my_addr) == -1) {


   if ((connect(sockfd_stream, (struct sockaddr *) &their_addr, sizeof their_addr)) == -1) {


   if (setsockopt(sockfd_stream, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)


   if (listen(sockfd_stream, 100) == -1) {

accept

read

write

*/



int socket_list_init(SOCKET_LIST *socket_list)
{
    return ( socket_list->sockets = list_init() ) != NULL;
}


int socket_list_insert(SOCKET_LIST *socket_list, SOCKET_HANDLE socket, 
	                   socket_event_handler_func_t handler, 
				       void *context_data, int context_len)
{
	SOCKET_ENTRY entry;

	entry.socket = socket;
	entry.handler = handler;

	if (context_data != NULL) {

		entry.context_data = malloc(context_len);
		if (entry.context_data == NULL) {
			return FALSE;
		}
		else {
			memcpy(entry.context_data, context_data, context_len);
		}
	}
	else {
		entry.context_data = NULL;
	}

	return list_insert_after(socket_list->sockets, &entry, sizeof(entry)) != NULL;
}

/* typedef int(*list_traverse_func_t)(void *data, void *element); */


int sock_delete_list_traverse_func(void *data, void *element)
{
	SOCKET_ENTRY *pEntry;
	SOCKET_HANDLE socket;

	pEntry = (SOCKET_ENTRY *)element;
	socket = *((SOCKET_HANDLE*)data);

	if (pEntry->socket == socket) {
	
		if (pEntry->context_data != NULL)
			free(pEntry->context_data);

		/* Element found, stop traversing */
		return FALSE;
	}
	else {
		/* Element not found, continue traversing */
		return TRUE;
	}

	
}

int socket_list_delete(SOCKET_LIST *socket_list, SOCKET_HANDLE socket)
{
	
	if ( list_traverse(socket_list->sockets, (void*)&socket, sock_delete_list_traverse_func, 0) == LIST_OK ) {			

		list_remove_curr(socket_list->sockets);

		return TRUE;
	}
	else {
		return FALSE;
	}
	
}


int sock_debug_dump_list_traverse_func(void *data, void *element)
{
	SOCKET_ENTRY *pEntry;

	pEntry = (SOCKET_ENTRY *)element;


	fprintf(stderr, "----> socket=%d [h=%p cdata=%p]\n", 
		pEntry->socket, pEntry->handler, pEntry->context_data);
	
	/* continue traversing */
	return TRUE;
}

void socket_list_debug_dump(SOCKET_LIST *socket_list)
{

	list_traverse(socket_list->sockets, NULL, sock_debug_dump_list_traverse_func, 0);

}





int sock_fdset_list_traverse_func(void *data, void *element)
{
	SOCKET_ENTRY *pEntry;
	fd_set *mask;

	fprintf(stderr, ">sock_fdset_list_traverse_func\n");

	pEntry = (SOCKET_ENTRY *)element;
	mask = (fd_set*)data;

	FD_SET(pEntry->socket, mask);

	/* continue traversing */
	fprintf(stderr, "<sock_fdset_list_traverse_func\n");

	return TRUE;
}


void socket_list2fd_set(SOCKET_LIST *socket_list, fd_set *socket_mask)
{
	FD_ZERO(socket_mask);

	list_traverse(socket_list->sockets, socket_mask, sock_fdset_list_traverse_func, 0);
}




int sock_handle_event_list_traverse_func(void *data, void *element)
{
	SOCKET_ENTRY *pEntry;
	fd_set *mask;

	fprintf(stderr, ">sock_handle_event_list_traverse_func\n");

	pEntry = (SOCKET_ENTRY *)element;
	mask = (fd_set*)data;

	if (FD_ISSET(pEntry->socket, mask)) {
		pEntry->handler(pEntry->socket, pEntry->context_data);
	}

	fprintf(stderr, "<sock_handle_event_list_traverse_func\n");

	/* continue traversing */
	return TRUE;	
}


void socket_list_select_and_handle_events(SOCKET_LIST *socket_list)
{
	fd_set socket_mask;

	fprintf(stderr, ">socket_list_select_and_handle_events\n");

	socket_list_debug_dump(socket_list);

	socket_list2fd_set(socket_list, &socket_mask);

	if (select(100, &socket_mask, NULL, NULL, NULL) > 0) { // TODO: fix N_FDs
	
		fprintf(stderr, "selected...");

		list_traverse(socket_list->sockets, &socket_mask, sock_handle_event_list_traverse_func, 0);

	
	}

	fprintf(stderr, "<socket_list_select_and_handle_events\n");

}
