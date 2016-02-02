/*
* vsockets_tools_version.h - version information
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

#ifndef vsockets_tools_version_H
#define vsockets_tools_version_H

#define VSOCKETS_TOOLS_MAJOR_VERSION 0
#define VSOCKETS_TOOLS_MINOR_VERSION 12

#ifndef BUILD_DATE
#define BUILD_DATE Unknown
#endif

#ifdef linux
#define PLATFORM_STRING "Linux"
#else
#define PLATFORM_STRING "Win32"
#endif



/* Using stringification (as described in https://gcc.gnu.org/onlinedocs/cpp/Stringification.html#Stringification ) */
#define QUOTE_STRING( S1 ) #S1
#define CONCAT_STRINGS( S1, S2 ) S1 ## S2
#define EXPAND_MACRO_TO_STRING( S1 ) QUOTE_STRING( S1 )

#define BUILD_DATE_STRING "[BUILD=" EXPAND_MACRO_TO_STRING(BUILD_DATE) "]"

#define VSOCKETS_TOOLS_VERSION_STRING EXPAND_MACRO_TO_STRING(VSOCKETS_TOOLS_MAJOR_VERSION) "." EXPAND_MACRO_TO_STRING(VSOCKETS_TOOLS_MINOR_VERSION) "." BUILD_DATE_STRING " " PLATFORM_STRING

#endif