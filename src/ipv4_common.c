/*
 * ipv4_common.c - support for IPv4 bridging with vsockets
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

#include "vmci_sockets.h"
#include "sockets_common.h"
#include <stdio.h>
#include "ipv4_common.h"
#include "debug_common.h"


//
// Connection-oriented 
//
int try_ipv4_connection(int CID, int port)
{
   int afVMCI = VMCISock_GetAFValue();

   int sockfd_stream;

   debug_printf( "vmci connecting to=[CID]%d:%d", CID, port);

   sockfd_stream = socket(afVMCI, SOCK_STREAM, 0);
   if (sockfd_stream < 0) {
     perror("error creating socket\n");  
     exit(1);
   }

   debug_printf( "vmci socket=%d", sockfd_stream);

   struct sockaddr_vm my_addr = {0};
   my_addr.svm_family = afVMCI;
   my_addr.svm_cid = VMADDR_CID_ANY;
   my_addr.svm_port = VMADDR_PORT_ANY;

   if (bind(sockfd_stream, (struct sockaddr *) &my_addr, sizeof my_addr) == -1) {
     perror("bind");
     socket_close(sockfd_stream);
     exit(1);
   }

   struct sockaddr_vm their_addr = {0};
   their_addr.svm_family = afVMCI;
   their_addr.svm_cid = CID;
   their_addr.svm_port = port;
   if ((connect(sockfd_stream, (struct sockaddr *) &their_addr, sizeof their_addr)) == -1) {
     perror("connect");
     socket_close(sockfd_stream);
     return -1;
   }

   return sockfd_stream;
}


int try_ipv4_listen(int port)
{

   int sockfd_stream;
   struct sockaddr_in my_addr;

   debug_printf( "ipv4 listening in port %d", port);

   sockfd_stream = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd_stream < 0) {
     perror("error creating socket\n");  
     exit(1);
   }

   debug_printf( "ipv4 socket=%d", sockfd_stream);

   int enable = 1;
   if (setsockopt(sockfd_stream, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(int)) < 0)
     perror("setsockopt(SO_REUSEADDR) failed");

   my_addr.sin_family = AF_INET;
   my_addr.sin_addr.s_addr = INADDR_ANY;
   my_addr.sin_port = ntohs(port);

   if (bind(sockfd_stream, (struct sockaddr *) &my_addr, sizeof my_addr) == -1) {
     perror("bind");
     socket_close(sockfd_stream);
     return -1;
   }

   if (listen(sockfd_stream, 100) == -1) {
      perror("listen");
     return -1;
   }

   return sockfd_stream;
   return -1;
}



