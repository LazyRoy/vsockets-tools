#
# * vm_backdump - project Makefile
# *
# * Copyright (C) 2016 Pedro Mendes da Silva
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

host-type := $(shell uname)

ifeq ($(host-type),Linux)
PLATFORM := LINUX
BUILD_DATE := $(shell date --utc)

else

PLATFORM := WIN32
BUILD_DATE := $(shell date /t) $(shell time /t)

endif

ifeq ($(PLATFORM),WIN32)

INCLUDES:= ../lib/tmp/liblist-2.4.mingw32/include
LIBS:= -L../lib/tmp/liblist-2.4.mingw32/lib  -llist -lws2_32

PLATFORM_DIR=win32

else

# -L../lib/tmp/liblist-2.4.mingw32/lib

#LIBS:= -L../lib/tmp/  -llist 

LIBS:= 
INCLUDES:= . -I..

PLATFORM_DIR=linux

endif

DEFINES:= -DBUILD_DATE="$(BUILD_DATE)"

COPTIONS:=-g -static -Wall -I$(INCLUDES) $(DEFINES)

COMMON_C_FILES:=

TARGETDIR:=../../target/$(PLATFORM_DIR)

TARGETBINDIR:=$(TARGETDIR)/bin

BINS:=

all: vm_backdump

vm_backdump: ./vm_backdump.c $(COMMON_C_FILES)
	@echo "### Making vm_backdump"
	$(CC) $(COPTIONS)  vm_backdump.c $(COMMON_C_FILES) $(LIBS) -o $(TARGETBINDIR)/vm_backdump

