#include <stdlib.h>

#include "vmci_sockets.h"
#include <stdio.h>
#include <sys/socket.h>


extern int dump_vsocket_properties();

int main()
{

   dump_vsocket_properties();

   return 0;
}