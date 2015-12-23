#
# * vsockets tools - project Makefile
# *
# * Copyright (C) 2015 Pedro Mendes da Silva
# *
# *  This program is free software; you can redistribute it and/or modify
# *  it under the terms of the GNU General Public License as published by
# *  the Free Software Foundation; either version 2 of the License, or
# *  (at your option) any later version.
# *
# *  This program is distributed in the hope that it will be useful,
# *  but WITHOUT ANY WARRANTY; without even the implied warranty of
# *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# *  GNU General Public License for more details.
# *
# *  You should have received a copy of the GNU General Public License
# *  along with this program; if not, write to the Free Software
# *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# *

CC:=gcc

COPTIONS:=-g -static

TARGETDIR:=./target

TARGETBINDIR:=$(TARGETDIR)/bin

LIBS:=

BINS:=

all: vsockets_hostname vsockets_nc 

vsockets_hostname: ./vsockets_hostname.c vsockets_common.c
	@echo "### Making vsockets_hostname"
	$(CC) $(COPTIONS) vsockets_hostname.c vsockets_common.c -o $(TARGETBINDIR)/vsockets_hostname

vsockets_nc: ./vsockets_nc.c vsockets_common.c ipv4_common.c
	@echo "### Making vsockets_nc"
	$(CC) $(COPTIONS) vsockets_nc.c vsockets_common.c ipv4_common.c -o $(TARGETBINDIR)/vsockets_nc 