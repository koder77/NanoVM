/****************************************************************************
*
* Filename: for.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  N compiler: for loop functions
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2012
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
#include "parser.h"

extern struct for_comp for_comp[MAXFOR];
extern S4 for_ind;

extern struct jumplist *jumplist;
extern S4 jumplist_ind;

void init_for (void)
{
    S4 i;

    for (i = 0; i < MAXFOR; i++)
    {
        for_comp[i].used = FALSE;
        for_comp[i].for_pos = EMPTY;
        for_comp[i].next = FALSE;
    }
}

S4 get_for_pos (void)
{
    S4 i;

    for (i = 0; i < MAXFOR; i++)
    {
        if (for_comp[i].used == FALSE)
        {
            for_comp[i].used = TRUE;
            return (i);
        }
    }

    /* no empty if found => error! */

    return (-1);
}

S4 get_act_for (void)
{
    S4 i;

    for (i = MAXFOR - 1; i >= 0; i--)
    {
        if (for_comp[i].used == TRUE && for_comp[i].next == FALSE)
        {
            return (i);
        }
    }
}

U1 set_for_label (S4 ind)
{
    U1 ret;
    U1 label[MAXJUMPNAME + 1];
    U1 labelnum[10];

    strcpy (label, "for_");
    sprintf (labelnum, "%d", ind);
    strcat (label, labelnum);

	#if DEBUG
		printf ("DEBUG: set_for_label: '%s'\n", label);
	#endif
	
    ret = compile_set_label (label);

    for_comp[ind].for_pos = jumplist_ind;
    return (ret);
}

S4 get_for_lab (S4 ind)
{
    return (for_comp[ind].for_pos);
}

void set_next (S4 ind)
{
    for_comp[ind].next = TRUE;
}
