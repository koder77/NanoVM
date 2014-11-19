/****************************************************************************
*
* Filename: jumpl.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  jumplist functions
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

extern struct jumplist *jumplist;

extern S4 jumplist_ind;
extern S4 maxjumplist;
extern S4 jumplist_size;
extern S4 plist_ind;

void init_jumplist (void)
{
    S4 i;

    for (i = 0; i <= jumplist_size - 1; i++)
    {
        strcpy (jumplist[i].lab, "");
        jumplist[i].pos = 0;
        jumplist[i].source_pos = 0;
        jumplist[i].islabel = FALSE;
    }
}

U1 jumplist_gonext (void)
{
    if (jumplist_ind < jumplist_size - 1)
    {
        jumplist_ind++;
        maxjumplist = jumplist_ind;
        return (TRUE);
    }
    else
    {
        printerr (JUMPLIST, plist_ind, ST_PRE, "");
        return (FALSE);
    }
}

S4 getjumpind (U1 *str)
{
    S4 i = 0, ind = NOTDEF;
    U1 ok = FALSE;

    while (! ok)
    {
        if (strcmp (jumplist[i].lab, str) == 0)
        {
            ind = i, ok = TRUE;
        }
        if (i < maxjumplist)
        {
            i++;
        }
        else
        {
            ok = TRUE;
        }
    }
    return (ind);
}

void debug_jumplist (void)
{
    S4 i;

    printf ("\n\ndebug jumplist:\n");
    for (i = 0; i <= maxjumplist; i++)
    {
        printf ("label: %s, %li  on line: %li\n", jumplist[i].lab, jumplist[i].pos, jumplist[i].source_pos);
    }
}

