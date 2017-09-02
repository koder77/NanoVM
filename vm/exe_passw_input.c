/****************************************************************************
*
* Filename: exe_passw_input.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  password input functions (no echo)
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
#include "arch.h"

#if OS_LINUX || OS_WINDOWS_CYGWIN || OS_DRAGONFLY

#include <stdio.h>
#include <unistd.h>


void get_password (U1 *buffer, S8 size)
{
    S8 pos;
    S8 len;
    S8 max;
    
    unsigned char *password;
    
    password = getpass ("");
    len = strlen (password);
    
    if (len > size)
    {
        max = size - 1;
    }
    else
    {
        max = len;
    }
    
    for (pos = 0; pos < max; pos++)
    {
        buffer[pos] = password[pos];
    }
    
    buffer[pos] = BINUL;
}


#endif

#if OS_WINDOWS

#include <conio.h>

void get_password (U1 *buffer, S8 size)
{
    S8 pos = 0;
    U1 ch;

    for (;;)
    {
        ch = getch ();
        if (ch == 13)
        {
            buffer[pos] = BINUL;

            printf ("\n");
            break;
        }
        else
        {
            if (pos < size - 1)
            {
                buffer[pos] = ch;
                pos++;

                printf ("*");
            }
            else
            {
                buffer[pos] = BINUL;

                printf ("\n");
                break;
            }
        }
    }
}

#endif

#if OS_AROS || OS_ANDROID

void get_password (U1 *buffer, S8 size)
{
}

#endif
