/****************************************************************************
*
* Filename: ccl.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  increase cclist index
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

extern S4 cclist_ind;
extern S4 maxcclist;
extern S4 cclist_size;
extern S4 plist_ind;


U1 cclist_gonext (void)
{
    /* go to next step in list */

    if (cclist_ind < cclist_size - 1)
    {
        cclist_ind++;
        maxcclist = cclist_ind;

        return (TRUE);
    }
    else
    {
        /* error: list is full! */

        printerr (CCLIST, plist_ind, ST_PRE, "");
        return (FALSE);
    }
}

