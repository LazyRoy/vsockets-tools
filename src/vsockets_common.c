/*
 * vsockets_common.c - vsockets simplified API
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


#include <stdio.h>
#include <stdlib.h>
//#include <sys/wait.h>

#include "vmci_sockets.h"
#include "sockets_common.h"
#include "vsockets_common.h"
#include "debug_common.h"



/* #include <sys/socket.h> */

//
// Connection-oriented 
//
int try_connection(int CID, int port)
{
   int afVMCI = VMCISock_GetAFValue();

   int sockfd_stream;

   debug_printf( "vmci connecting to=[CID]%u:%u", CID, port);

   sockfd_stream = socket(afVMCI, SOCK_STREAM, 0);

   if (sockfd_stream < 0) {
     psockerror("error creating socket"); 
     exit(1);
   }

   debug_printf( "vmci socket=%d", sockfd_stream);

   struct sockaddr_vm my_addr = {0};
   my_addr.svm_family = afVMCI;
   my_addr.svm_cid = VMADDR_CID_ANY;
   my_addr.svm_port = VMADDR_PORT_ANY;

   if (bind(sockfd_stream, (struct sockaddr *) &my_addr, sizeof my_addr) == -1) {
     psockerror("bind");
     socket_close(sockfd_stream);
     exit(1);
   }

   struct sockaddr_vm their_addr = {0};
   their_addr.svm_family = afVMCI;
   their_addr.svm_cid = (CID==-1) ? VMADDR_CID_ANY : CID;
   their_addr.svm_port = port;
   if ((connect(sockfd_stream, (struct sockaddr *) &their_addr, sizeof their_addr)) == -1) {
     psockerror("connect");
	 debug_printf("connect failed");
     debug_printf("Closing vmci socket %u", sockfd_stream );
	 socket_close(sockfd_stream);
     return -1;
   }

   return sockfd_stream;
}

// Connection-oriented port scan
//
// TODO: refactor for WINDOWS

int host_port_scan(int CID)
{
   int i;

   DEBUG_ENTER(host_port_scan);

   printf("Scanning vsocket ports for CID=%u...\n", CID);

   for(i=0;i<66000;i++)
   {

      // Fork due to ESXi socket leakage problem

      //if ( fork()== 0 ) {

	      int sock = try_connection( CID, i);
	      if (sock > 0) {
	        printf("-+-host port %d open\n",i);
	        socket_close(sock);
	      } else {
	        //printf("---host port %d closed\n",i);   
	      }

	      //exit(0);
/*
      } else {
        int status;

	 // wait(&status);
      } */
   }

   DEBUG_LEAVE(host_port_scan);

   return FALSE;
}





int try_listen(int port)
{
   int afVMCI = VMCISock_GetAFValue();

   int sockfd_stream;

   debug_printf( "vmci listening in port %d", port);

   sockfd_stream = socket(afVMCI, SOCK_STREAM, 0);

   if (sockfd_stream < 0) {
     psockerror("error creating socket");  
     exit(1);
   }

   debug_printf( "vmci socket=%d", sockfd_stream);

   struct sockaddr_vm my_addr = {0};
   my_addr.svm_family = afVMCI;
   my_addr.svm_cid = VMADDR_CID_ANY;
   my_addr.svm_port = port;

   if (bind(sockfd_stream, (struct sockaddr *) &my_addr, sizeof my_addr) == -1) {
     psockerror("bind");
	 fprintf(stderr, "Maybe you are not running as root or Administrator\n");
	 if (port < 1024)
		 fprintf(stderr, "Listening in ports lower than 1024 require running as root or Administrator\n");
     socket_close(sockfd_stream);
     return -1;
   }

   if (listen(sockfd_stream, 100) == -1) {
     psockerror("listen");
     return -1;
   }

   return sockfd_stream;
}


int vsockets_is_available()
{

	int afVMCI = VMCISock_GetAFValue();
	int localCID = -1;

	debug_printf( "vsocket_is_available>>VMware vmci address familly=%d", afVMCI);

	if (afVMCI > 0) {
	   localCID = VMCISock_GetLocalCID();
	   debug_printf( "VMware vmci local CID=%u", localCID);
	}

	if ((afVMCI > 0) && (localCID !=-1)) {
		debug_printf( "TRUE");
		return TRUE;
	}
	else {
		debug_printf( "FALSE %d %d", afVMCI, localCID);
		return FALSE;
	}

}

int dump_vsocket_properties()
{

   int afVMCI = VMCISock_GetAFValue();

   fprintf(stderr, "VMware vsockets environment properties\n");
   fprintf(stderr, "=======================================\n");

   fprintf(stderr, "VMware vmci address familly=%d\n", afVMCI);

   if (afVMCI > 0) {

     fprintf(stderr, "VMware vmci is present\n" );

     int localCID = VMCISock_GetLocalCID();
     fprintf(stderr, "VMware vmci local CID=%u\n", localCID);

	 /****
     if (localCID == VMWARE_PLAYER_HOST_CID) {
        fprintf(stderr, "vmware player host machine detected (CID=0)\n");
     } else if (localCID == VMWARE_ESXI_HOST_CID) {
        fprintf(stderr, "vmware ESXi host machine detected (CID=0)\n");
		*/

	if (localCID == VMWARE_HYPERVISOR_CID) {
		fprintf(stderr, "VMware Hypervisor host machine detected (CID=%u)\n", VMWARE_HYPERVISOR_CID);

     } else if (localCID == VMWARE_INVALID_CID) {
		 fprintf(stderr, "Physical machine without hypervisor installation detected (CID=%u)\n", VMWARE_INVALID_CID);

	 }
	 else {
	fprintf(stderr, "Guest machine detected (CID=%u)\n", localCID);
     }

     return 1;

   } else {
 
    fprintf(stderr, "vmci is NOT present\n");
    return 0;
   }

}

