#include <stdlib.h>

#include "vmci_sockets.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define VMWARE_HOST_CID 0

//
// Connection-oriented 
//
int try_ipv4_connection(int CID, int port)
{
   int afVMCI = VMCISock_GetAFValue();

   int sockfd_stream;

   fprintf(stderr, "vmci connecting to=[CID]%d:%d\n", CID, port);

   sockfd_stream = socket(afVMCI, SOCK_STREAM, 0);
   if (sockfd_stream < 0) {
     perror("error creating socket\n");  
     exit(1);
   }

   fprintf(stderr, "vmci socket=%d\n", sockfd_stream);

   struct sockaddr_vm my_addr = {0};
   my_addr.svm_family = afVMCI;
   my_addr.svm_cid = VMADDR_CID_ANY;
   my_addr.svm_port = VMADDR_PORT_ANY;

   if (bind(sockfd_stream, (struct sockaddr *) &my_addr, sizeof my_addr) == -1) {
     perror("bind");
     close(sockfd_stream);
     exit(1);
   }

   struct sockaddr_vm their_addr = {0};
   their_addr.svm_family = afVMCI;
   their_addr.svm_cid = CID;
   their_addr.svm_port = port;
   if ((connect(sockfd_stream, (struct sockaddr *) &their_addr, sizeof their_addr)) == -1) {
     perror("connect");
     close(sockfd_stream);
     return -1;
   }

   return sockfd_stream;
}


int try_ipv4_listen(int port)
{

   int sockfd_stream;
   struct sockaddr_in my_addr;

   fprintf(stderr, "ipv4 listening in port %d\n", port);

   sockfd_stream = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd_stream < 0) {
     perror("error creating socket\n");  
     exit(1);
   }

   fprintf(stderr, "ipv4 socket=%d\n", sockfd_stream);

   int enable = 1;
   if (setsockopt(sockfd_stream, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
     perror("setsockopt(SO_REUSEADDR) failed");

   my_addr.sin_family = AF_INET;
   my_addr.sin_addr.s_addr = INADDR_ANY;
   my_addr.sin_port = ntohs(port);

   if (bind(sockfd_stream, (struct sockaddr *) &my_addr, sizeof my_addr) == -1) {
     perror("bind");
     close(sockfd_stream);
     return -1;
   }

   if (listen(sockfd_stream, 100) == -1) {
      perror("listen");
     return -1;
   }

   return sockfd_stream;
   return -1;
}



