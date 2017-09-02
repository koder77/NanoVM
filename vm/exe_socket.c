/****************************************************************************
*
* Filename: exe_socket.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  socket wrapper file for automatic socket recognization
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2011
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
*
***************************************************************************/

#include "host.h"
#include "global_d.h"

#if OS_AMIGA || OS_AROS || OS_ANDROID || OS_DRAGONFLY
    #include "exe_socket_ipv4_amiga.h"
#endif

#if OS_LINUX || OS_WINDOWS || OS_WINDOWS_CYGWIN
	#if OS_LINUX_RPI
		#include "exe_socket_ipv4_amiga.h"
	#else
		#include "exe_socket_ipv6.h"
	#endif
#endif
