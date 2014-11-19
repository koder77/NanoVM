/****************************************************************************
*
* Filename: openend.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  open and convert endianess
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2003
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

extern U1 open_err;

void read_string (U1 *str, U1 *retstr, S4 pos, S2 len)
{
    S4 i, j = 0;

    for (i = pos; i <= pos + len - 1; i++)
    {
        retstr[j] = str[i];
        j++;
    }
    retstr[j] = BINUL;
}

S2 read_stringbn (U1 *str, U1 *retstr, S4 pos, S4 size)
{
    U1 read = TRUE;
    S4 i = pos, j = 0, endpos = size - 1;

    retstr[j] = BINUL;

    while (read)
    {
        retstr[j] = str[i];

        if (str[i] == BINUL || i == endpos)
        {
            read = FALSE;
        }
        i++; j++;
    }
    return (strlen (retstr));
}

void read_char (U1 *str, U1 *retstr, S4 pos)
{
    retstr[0] = str[pos];
}

void write_char (U1 *str, U1 wstr, S4 pos)
{
    str[pos] = wstr;
}

S2 read_int (U1 *str, S4 pos)
{
    S2 n;
    S4 i;
    U1 *ptr;

    ptr = (U1 *) &n;

    for (i = pos; i <= pos + sizeof (S2) - 1; i++)
    {
        *ptr++ = str[i];
    }
    return (n);
}

S4 read_lint (U1 *str, S4 pos)
{
    S4 i;
    S4 n;
    U1 *ptr;

    ptr = (U1 *) &n;

    for (i = pos; i <= pos + sizeof (S4) - 1; i++)
    {
        *ptr++ = str[i];
    }
    return (n);
}

S8 read_qint (U1 *str, S4 pos)
{
    S4 i;
    S8 n;
    U1 *ptr;

    ptr = (U1 *) &n;

    for (i = pos; i <= pos + sizeof (S8) - 1; i++)
    {
        *ptr++ = str[i];
    }
    return (n);
}

F8 read_double (U1 *str, S4 pos)
{
    S4 i;
    F8 n;
    U1 *ptr;

    ptr = (U1 *) &n;

    for (i = pos; i <= pos + sizeof (F8) - 1; i++)
    {
        *ptr++ = str[i];
    }
    return (n);
}

