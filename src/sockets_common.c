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

typedef struct {
    LIST *sockets;
} SOCKET_LIST;

int socket_list_init(SOCKET_LIST *socket_list)
{
    socket_list->sockets = list_init();
}