/****************************************************************************
*
* Filename: socket.h
*
* Author:   Stefan Pietzonke
* Project:  flow, the gfx plugin for nano VM
*
* Purpose:  socket prototypes
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2009
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

/* socket protos --------------------------------------------------------- */

#define PACKET_SIZE  257

int code_packet (unsigned char *data, unsigned char *packet, int len);
int send_packet (int socket, unsigned char *buf, int len);
int decode_packet (unsigned char *packet, unsigned char *data, int len);
int receive_packet (int socket, unsigned char *buf);

