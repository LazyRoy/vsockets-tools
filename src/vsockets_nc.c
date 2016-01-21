/*
 * vsockets_nc.c - vsockets netcat
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
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include "list.h"
#include "sockets_common.h"
#include "vsockets_common.h"
#include "ipv4_common.h"

#define true 1
#define false 0



/******* Command-line options:

	root@kali:~/projects/vmci# vsockets_nc -h
	[v0.01]
	connect to somewhere:	vsockets_nc [-options] -c CID -s port ... 
	listen for inbound:	vsockets_nc -l [port]
		

*******/

#define BRIDGE_BUF_SIZE 64000

SOCKET_LIST sockets;

typedef struct {

	int flag_listen_vsockets;
	int flag_listen_ipv4;
	int flag_tunnel_connections;

	unsigned int local_port;
	unsigned int remote_port;
	unsigned int remote_cid;
	
} VSOCKETS_NC_SESSION_OPTIONS;

VSOCKETS_NC_SESSION_OPTIONS session_options;


int bridged_socket_event_handler(SOCKET_HANDLE socket_in, void *context_data)
{
	char buffer[BRIDGE_BUF_SIZE];
	int len;
	
	SOCKET_HANDLE socket_out = *((SOCKET_HANDLE*)context_data);

	fprintf(stderr, "...Reading from socket %d ...\n", socket_in);

	len = socket_read(socket_in, buffer, BRIDGE_BUF_SIZE);

	if (len > 0) {

		fprintf(stderr, "Read %u bytes from channel 1\n", len);

		len = socket_write(socket_out, buffer, len);

		if (len <= 0) {
			perror("Error writing to descriptor");
			exit(1);
		}

		fprintf(stderr, "Wrote %u bytes to channel 2\n", len);
		
		return true;
	}
	else {
		perror("Lost connection channel 1\n");
		
		// TODO: remove bridged sockets from list and close

		// Corrigir: marcar a entrada como apagada até terminar o ciclo de eventos
		// Quando voltar ao ciclo principal, apaga o lixo

		socket_list_delete(&sockets, socket_in);
		socket_close(socket_in);

		socket_list_delete(&sockets, socket_out);
		socket_close(socket_out);

		// exit(1);

		return 0;
	}
}


void bridge_descriptors(int fd1_in, int fd1_out, int fd2_in, int fd2_out)
{
	

	fprintf(stderr, "...bridging\n");

	socket_list_insert(&sockets,
		fd1_in,
		bridged_socket_event_handler,
		&fd2_in, sizeof(fd2_in));

	socket_list_insert(&sockets,
		fd2_in,
		bridged_socket_event_handler,
		&fd1_in, sizeof(fd1_in));

}





void bridge_sockets_and_descriptors(int socket, FILE *f_in, FILE *f_out)
{
   int fd_in, fd_out;

   //printf("Bridging socket=%u, FILE IN %p, FILE OUT %p\n", socket, f_in, f_out);

   fd_in = fileno( f_in );

   fd_out = fileno( f_out );

   bridge_descriptors(socket, socket, fd_in, fd_out );

}





int vsocket_listen_event_handler(SOCKET_HANDLE socket, void *context_data)
{
	int new_sock = -1;
	struct sockaddr_vm their_addr;
	socklen_t their_addr_len = sizeof their_addr;

	if ((new_sock = socket_accept(socket, (struct sockaddr *) &their_addr, &their_addr_len)) == -1) {
		perror("accept");
		exit(1);
	}
	else {
		fprintf(stderr, "....Accepted\n");

		if (session_options.flag_tunnel_connections) {
			// Tunelling

			int tunnel_socket;

			fprintf(stderr, "[Tunnel]...Connecting to CID=%hu : Port:%hu\n", 
				session_options.remote_cid, session_options.remote_port);


			tunnel_socket = try_connection(session_options.remote_cid, 
				session_options.remote_port);

			if (tunnel_socket > 0) {
				fprintf(stderr, "[Tunnel]...Connection established to CID=%u : Port:%hu\n", 
					session_options.remote_cid, session_options.remote_port);

				bridge_descriptors(new_sock, new_sock, tunnel_socket, tunnel_socket);

			}
			else {
				perror("[Tunnel]...Connection failed");
			}
		}
		else {

			// Single connection
			bridge_sockets_and_descriptors(new_sock, stdin, stdout);

		}

		/* continue traversing */
		return TRUE;
	}
}


/* Listening mode */
void listening_mode(short listen_on_vsockets, 
	                unsigned int local_port)
{
	
	int socket;

	if (listen_on_vsockets)
		socket = try_listen(local_port);
	else
		socket = try_ipv4_listen(local_port);


	if (socket > 0) {
		fprintf(stderr, "...listening in port:%hu\n", local_port);

		socket_list_insert(&sockets,
							socket,
							vsocket_listen_event_handler,
							NULL, 0);
	}
	else {
		exit(0);
	}

	while (1) {
		fprintf(stderr, "...entering main cycle\n");

		socket_list_select_and_handle_events(&sockets);
	}
}


void display_help(char *bin)
{
	fprintf(stderr, "Usage: %s [-c <connect to CID>] [-s <connect port>] \n\t\t[-l <listen port vsocket>] [-i <listen port ipv4>] [-t]\n",
		bin);
}

int
main(int argc, char *argv[])
{

           unsigned int remote_port = 0, local_port = 0;
           unsigned int remote_cid = 0;

           int flag_listen = false;            
           int flag_tcp_ip_v4_listen = false;                     
		   int flag_connect = false;


           int flag_tunnel_incomming_connections = false;

           int flag_port_scan = false;

           char fork_cmd_line[1000];

		   int flag_remote_port = false;

           char opt;


           while ((opt = getopt(argc, argv, "c:s:l:i:tn")) != -1) {
               switch (opt) {
                   
                /* Remote CID to connect to */
               case 'c':             
                   sscanf(optarg, "%u", &remote_cid);
				   flag_connect = true;
                   break;
                   
               /* Remote port to connect to */
               case 's':
                   remote_port = atoi(optarg);
				   flag_remote_port = true;
                   break;

               /* Listen port (local) for vsockets address family */
               case 'l':
                   local_port = atoi(optarg);
		           flag_listen = true;
                   break;

               /* Listen port (local) for IPv4 address family */
               case 'i':
                   local_port = atoi(optarg);
		           flag_tcp_ip_v4_listen = true;
                   break;               

               /* Tunnel connections ( connects inbound listen address => remote address ) */
               case 't':
		           flag_tunnel_incomming_connections = true;
                   break;

               /* vsockets port scan mode */
               case 'n':
		           flag_port_scan = true;
                   break;


               default: /* '?' */
				   display_help(argv[0]);
                   exit(EXIT_FAILURE);
               }
           }


    dump_vsocket_properties();

	if (!vsockets_is_available()) {
		fprintf(stderr, "vsockets are not available in this system\n");
		exit(EXIT_FAILURE);
	}

	socket_init_api();

	socket_list_init(&sockets);

    if ( flag_port_scan == true ) {
     host_port_scan(remote_cid);
	 exit(1);
    }

	session_options.flag_listen_vsockets = flag_listen;
	session_options.flag_listen_ipv4 = flag_tcp_ip_v4_listen;
	session_options.flag_tunnel_connections = flag_tunnel_incomming_connections;
	session_options.local_port = local_port;
	session_options.remote_port = remote_port;
	session_options.remote_cid = remote_cid;

    if ((flag_connect == true) && (flag_remote_port == true)) {

		/* Client-connect mode */

       int socket;
      
       fprintf(stderr, "...Connecting to CID=%hu : Port:%u\n", remote_cid, remote_port);

      
       socket = try_connection(remote_cid, remote_port);

       if (socket > 0) {
          fprintf(stderr, "...Connection established to CID=%u : Port:%hu\n", remote_cid, remote_port);

          bridge_sockets_and_descriptors(socket, stdin, stdout);

		  while ( socket_list_select_and_handle_events(&sockets) ) {
		  }

       } else {
          perror("...Connection failed");
       }


    } else if ((flag_listen == true) || (flag_tcp_ip_v4_listen == true))  {

	   /* Listening mode */

       int socket;

	   listening_mode(flag_listen,
						local_port);


	
	}
	else {

		/* Invalid arguments, display help */
		display_help(argv[0]);
	}
	

   return 0;
}