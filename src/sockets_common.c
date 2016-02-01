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

#include <unistd.h>
#include <fcntl.h>

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

#ifdef WINNT

#define STDIN_READ_BUFFER_SIZE 1024

HANDLE hReadPipe;
HANDLE hWritePipe;
HANDLE hStdInThread;

static DWORD WINAPI stdin_read_thread(void *param)
{
	char read_buffer[STDIN_READ_BUFFER_SIZE];
	
	HANDLE inhandle;

	DWORD nBytesRead, nBytesWriten;

	fprintf(stderr, ">>>>Starting stdin thread\n\n");

	inhandle = GetStdHandle(STD_INPUT_HANDLE);

	fprintf(stderr, ">>>>THREAD: inhandle=%p\n\n", inhandle);

	CreatePipe(&hReadPipe, &hWritePipe,	NULL, 0);

	while (ReadFile(inhandle, read_buffer, sizeof(read_buffer),
		&nBytesRead, NULL) && (nBytesRead > 0) ) {

		fprintf(stderr, ">>>>THREAD: read some bytes=%d\n\n", nBytesRead);

		WriteFile(hWritePipe, read_buffer, nBytesRead, &nBytesWriten, NULL);
		
		fprintf(stderr, ">>>>THREAD: wrote some bytes=%d\n\n", nBytesWriten);
	}

	fprintf(stderr, ">>>>THREAD: found EOF, ending\n\n");

	return 0;
}

#endif // WINNT

int socket_list_init(SOCKET_LIST *socket_list)
{
	socket_list->n_special_descriptors = 0;

    return ( socket_list->sockets = list_init() ) != NULL;
}


int socket_list_insert(SOCKET_LIST *socket_list, SOCKET_HANDLE socket, 
	                   socket_event_handler_func_t handler, 
				       void *context_data, int context_len)
{
	SOCKET_ENTRY entry;

	entry.socket = socket;
	entry.handler = handler;
	entry.deleted = FALSE;
	entry.is_special_descriptor = FALSE;

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

#ifndef linux
	if (socket == fileno(stdout) || socket == fileno(stdin) || socket == fileno(stderr)) {
		socket_list->n_special_descriptors++;
		entry.is_special_descriptor = TRUE;

		if (socket == fileno(stdin)) {

		}
	}
#endif

	return list_insert_after(socket_list->sockets, &entry, sizeof(entry)) != NULL;
}

/* typedef int(*list_traverse_func_t)(void *data, void *element); */


int sock_delete_list_traverse_func(void *data, void *element)
{
	SOCKET_ENTRY *pEntry;
	SOCKET_HANDLE socket;

	pEntry = (SOCKET_ENTRY *)element;
	socket = *((SOCKET_HANDLE*)data);

	fprintf(stderr, ">sock_delete_list_traverse_func (socket_entry=%u socket2delete=%u)\n", socket, pEntry->socket);

	if (pEntry->socket == socket) {
	
		if (pEntry->context_data != NULL)
			free(pEntry->context_data);

		pEntry->deleted = TRUE;

		fprintf(stderr, "<sock_delete_list_traverse_func\n");
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

	fprintf(stderr, ">socket_list_delete (socket=%u)\n", socket);

	if ( list_traverse(socket_list->sockets, (void*)&socket, sock_delete_list_traverse_func, 0) == LIST_OK ) {			

		/* list_remove_curr(socket_list->sockets); */


		fprintf(stderr, "<socket_list_delete(TRUE)\n");

		return TRUE;
	}
	else {
		
		fprintf(stderr, "<socket_list_delete(FALSE)\n");

		return FALSE;
	}
	
}


int sock_debug_dump_list_traverse_func(void *data, void *element)
{
	SOCKET_ENTRY *pEntry;

	pEntry = (SOCKET_ENTRY *)element;


	fprintf(stderr, "----> socket=%d [h=%p cdata=%p del=%hu %s]\n", 
		pEntry->socket, pEntry->handler, pEntry->context_data, pEntry->deleted,
		socket_handle2string(pEntry->socket)
		);
	
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

	if (!pEntry->deleted) {
#ifdef linux
		FD_SET(pEntry->socket, mask);
#else 
		if (!pEntry->is_special_descriptor)
			FD_SET(pEntry->socket, mask);
		else
			fprintf(stderr, "file descriptor %d ignored\n", pEntry->socket);
#endif
	}

	/* continue traversing */
	fprintf(stderr, "<sock_fdset_list_traverse_func\n");

	return TRUE;
}


void socket_list2fd_set(SOCKET_LIST *socket_list, fd_set *socket_mask)
{
	FD_ZERO(socket_mask);

	list_traverse(socket_list->sockets, socket_mask, sock_fdset_list_traverse_func, 0);
}



int sock_remove_marked_for_removal_list_traverse_func(void *data, void *element)
{
	SOCKET_ENTRY *pEntry;

	pEntry = (SOCKET_ENTRY *)element;

	fprintf(stderr, ">sock_remove_marked_for_removal_list_traverse_func (socket_entry=%u )\n", pEntry->socket);

	if (pEntry->deleted) {	

		fprintf(stderr, "<sock_remove_marked_for_removal_list_traverse_func\n");
		/* Element found, stop traversing */
		return FALSE;
	}
	else {
		/* Element not found, continue traversing */
		return TRUE;
	}


}

int sock_handle_poll_for_special_descriptor_event(SOCKET_ENTRY *entry)
{
#ifdef WINNT
	int fd = entry->socket;

	/* STDIN */

	if (fd == fileno(stdout) || fd == fileno(stdin)) {

		/*if (WaitForSingleObject(hReadPipe, 0) == WAIT_OBJECT_0) {*/

		char next_byte;
		DWORD nBytesRead;

		if (PeekNamedPipe(
			hReadPipe,
			&next_byte,
			1,
			&nBytesRead,
			NULL,
			NULL) && nBytesRead==1) {

			/* Data from stdin is available, so fire the handler */
			return TRUE;
		}
	}
		
#endif
	/* Don't know how to handle this one, don't fire the handler */
	return FALSE;
}

int sock_handle_event_list_traverse_func(void *data, void *element)
{
	SOCKET_ENTRY *pEntry;
	fd_set *mask;

	fprintf(stderr, ">sock_handle_event_list_traverse_func\n");

	pEntry = (SOCKET_ENTRY *)element;
	mask = (fd_set*)data;

	if (!(pEntry->is_special_descriptor)) {
		if (FD_ISSET(pEntry->socket, mask)) {

			pEntry->handler(pEntry->socket, pEntry->context_data);
		}
	}
	else {
		if (sock_handle_poll_for_special_descriptor_event(pEntry)) {

			pEntry->handler(pEntry->socket, pEntry->context_data);
		}
	}

	fprintf(stderr, "<sock_handle_event_list_traverse_func\n");

	/* continue traversing */
	return TRUE;	
}


int socket_list_select_and_handle_events(SOCKET_LIST *socket_list)
{
	fd_set socket_mask;
	struct timeval timeout, *pTimeout;

	fprintf(stderr, ">socket_list_select_and_handle_events\n");

	socket_list_debug_dump(socket_list);

	// Remove marked items

	while (list_traverse(socket_list->sockets, NULL, sock_remove_marked_for_removal_list_traverse_func, 0) == LIST_OK) {
		fprintf(stderr, "Going to remove...\n");
		list_remove_curr(socket_list->sockets); 
	}

	socket_list2fd_set(socket_list, &socket_mask);

	if (socket_list->n_special_descriptors == 0) {
		pTimeout = NULL;
	}
	else {
		/* If special descriptors are present, use polling for them */
		timeout.tv_sec = 1; /* 1s */
		timeout.tv_usec = 0;
		pTimeout = &timeout;
	}

	if (!list_empty(socket_list->sockets)) {
		if (select(500, &socket_mask, NULL, NULL, pTimeout) >= 0) { // TODO: fix N_FDs

			fprintf(stderr, "selected...");

			list_traverse(socket_list->sockets, &socket_mask, sock_handle_event_list_traverse_func, 0);


		}
		else {
			perror("select failed");
		}
	}

	fprintf(stderr, "<socket_list_select_and_handle_events\n");

	return !list_empty(socket_list->sockets);
}




/**

int inet_aton(const char *cp, struct in_addr *inp);

in_addr_t inet_addr(const char *cp);

in_addr_t inet_network(const char *cp);

char *inet_ntoa(struct in_addr in);

struct in_addr inet_makeaddr(int net, int host);

in_addr_t inet_lnaof(struct in_addr in);

in_addr_t inet_netof(struct in_addr in);

**/

#define SOCKET_ADDR_STRING_SIZE 1000

char* socket_addr2string_ipv4(struct sockaddr_in *addr)
{
	static char buffer[SOCKET_ADDR_STRING_SIZE];

	sprintf(buffer, "%s:%hu",
		inet_ntoa(addr->sin_addr),
		ntohs(addr->sin_port)
		);

	return buffer;
}


char* socket_addr2string_vsockets(struct sockaddr_vm *addr)
{
	static char buffer[SOCKET_ADDR_STRING_SIZE];

	sprintf(buffer, 
		"[CID:%u]:%hu", 
		addr->svm_cid,
		addr->svm_port
		);

	return buffer;
}

char* socket_addr2string_unknown(struct sockaddr *addr)
{
	static char buffer[SOCKET_ADDR_STRING_SIZE];

	sprintf(buffer, "Unknown AF=%d", addr->sa_family);			

	return buffer;
}

char* socket_addr2string(struct sockaddr *addr)
{
	int afVMCI = VMCISock_GetAFValue();

	if (addr->sa_family == afVMCI) {
		return socket_addr2string_vsockets((struct sockaddr_vm*)addr);
	}

	switch (addr->sa_family) {
	case AF_INET:
		return socket_addr2string_ipv4((struct sockaddr_in*)addr);
	default:
		return socket_addr2string_unknown(addr);
	}
}

char* socket_handle2string(SOCKET_HANDLE socket)
{
	static char buffer[SOCKET_ADDR_STRING_SIZE];

	char addr_local_str[SOCKET_ADDR_STRING_SIZE], 
		 addr_peer_str[SOCKET_ADDR_STRING_SIZE];

	struct sockaddr addr_local;
	struct sockaddr addr_peer;
	socklen_t addrlen_local = sizeof(addr_local);
	socklen_t addrlen_peer = sizeof(addr_peer);

	getsockname(socket, &addr_local, &addrlen_local);
	getpeername(socket, &addr_peer, &addrlen_peer);

	strcpy(addr_local_str, socket_addr2string(&addr_local) );
	strcpy(addr_peer_str,  socket_addr2string(&addr_peer) );

	sprintf(buffer, "%s => %s", addr_local_str, addr_peer_str);

	return buffer;
}



/* platform-independent socket API */

int socket_init_api()
{
#ifdef linux
	return TRUE;
#else
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		fprintf(stderr,"WSAStartup failed with error: %d\n", err);
		return FALSE;
	}
	else {

		hStdInThread = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			stdin_read_thread,       // thread function name
			NULL,          // argument to thread function 
			0,                      // use default creation flags 
			NULL); 	// returns the thread identifier 

		return TRUE;
	}
#endif
}

int socket_accept(SOCKET_HANDLE sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int result;

	fprintf(stderr, ">socket_accept\n");

	result = accept(sockfd, addr, addrlen);

	if (addr != NULL) {
		fprintf(stderr, "[accepted connection from %s]\n", socket_addr2string(addr));
	}

	fprintf(stderr, "<socket_accept (sock=%d)\n", result);

	return result;
}

int socket_read(int fd, void *buf, size_t count)
{
#ifdef linux
	return read(fd, buf, count);
#else
	if (fd == fileno(stdin)) {
		DWORD nBytesRead=0;
		
		ReadFile(hReadPipe, buf, count, &nBytesRead, NULL);

		return nBytesRead;
	}
	else {
		return recv(fd, (char*)buf, count, 0);
	}
#endif
}

int socket_write(int fd, void *buf, size_t count)
{
#ifdef linux
	return write(fd, buf, count);
#else

	fprintf(stderr, "> socket_write fd=%d count=%d...\n",fd, count);
	fprintf(stderr, "stdinfd=%d stdoutfd=%d stderrfd=%d \n", 
		fileno(stdin), fileno(stdout), fileno(stderr));


	if (fd == fileno(stdout) || fd == fileno(stdin)) {		
		return fwrite(buf, count, 1, stdout);
	} else 	if (fd == fileno(stderr)) {
		return fwrite(buf, count, 1, stderr);
	}
	else {
		return send(fd, (char*)buf, count, 0);
	}
#endif
}



int socket_close(SOCKET_HANDLE sockfd)
{
#ifdef linux
	if (sockfd != fileno(stdin))
		return close(sockfd);
	else
		return FALSE;
#else
	return closesocket(sockfd);
#endif
}

