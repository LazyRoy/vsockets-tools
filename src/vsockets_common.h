/*
 * vsockets_common.h - support for cross-platform vsockets
 *
 * Copyright (C) 2016 Pedro Mendes da Silva
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

#ifndef VSOCKETS_COMMON_H
#define VSOCKETS_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include "vmci_sockets.h"

#define VMWARE_PLAYER_HOST_CID 0
#define VMWARE_ESXI_HOST_CID 2

// TODO: define typedef for CID

 //
 // Connection-oriented 
 //
int try_connection(int CID, int port);


// Connection-oriented port scan
//
int host_port_scan(int CID);


int try_listen(int port);

int dump_vsocket_properties();


#endif