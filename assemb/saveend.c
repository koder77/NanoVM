/****************************************************************************
*
* Filename: saveend.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  convert and save endianess
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

void write_string (U1 *str, U1 *wstr, S4 pos)
{
    S4 i, j = 0, wstr_len;

    wstr_len = strlen (wstr);

    for (i = pos; i <= pos + wstr_len - 1; i++)
    {
        str[i] = wstr[j];
        j++;
    }
}

void write_char (U1 *str, U1 wstr, S4 pos)
{
    str[pos] = wstr;
}

void write_int (U1 *str, S2 n, S4 pos)
{
    S4 i;
    U1 *ptr;

    ptr = (U1 *) &n;

    for (i = pos; i <= pos + sizeof (S2) - 1; i++)
    {
        str[i] = *ptr++;
    }
}

void write_lint (U1 *str, S4 n, S4 pos)
{
    S4 i;
    U1 *ptr;

    ptr = (U1 *) &n;

    for (i = pos; i <= pos + sizeof (S4) - 1; i++)
    {
        str[i] = *ptr++;
    }
}

void write_qint (U1 *str, S8 n, S4 pos)
{
    S4 i;
    U1 *ptr;

    ptr = (U1 *) &n;

    for (i = pos; i <= pos + sizeof (S8) - 1; i++)
    {
        str[i] = *ptr++;
    }
}

void write_double (U1 *str, F8 n, S4 pos)
{
    S4 i;
    U1 *ptr;

    ptr = (U1 *) &n;

    for (i = pos; i <= pos + sizeof (F8) - 1; i++)
    {
        str[i] = *ptr++;
    }
}

