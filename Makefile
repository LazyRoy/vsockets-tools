all: vsockets_hostname vsockets_nc 

vsockets_hostname: ./vsockets_hostname.c vsockets_common.c
	gcc -static vsockets_hostname.c vsockets_common.c -o vsockets_hostname

vsockets_nc: ./vsockets_nc.c vsockets_common.c ipv4_common.c
	gcc -g -static vsockets_nc.c vsockets_common.c -o vsockets_nc ipv4_common.c