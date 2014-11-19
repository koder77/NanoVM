/****************************************************************************
*
* Filename: if.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  N compiler: if functions
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

extern struct if_comp if_comp[MAXIF];
extern S4 if_ind;

extern struct jumplist *jumplist;
extern S4 jumplist_ind;

void init_if (void)
{
    S4 i;

    for (i = 0; i < MAXIF; i++)
    {
        if_comp[i].used = FALSE;
        if_comp[i].else_pos = EMPTY;
        if_comp[i].endif_pos = EMPTY;
    }
}

S4 get_if_pos (void)
{
    S4 i;

    for (i = 0; i < MAXIF; i++)
    {
        if (if_comp[i].used == FALSE)
        {
            if_comp[i].used = TRUE;
            return (i);
        }
    }

    /* no empty if found => error! */

    return (-1);
}

S4 get_act_if (void)
{
    S4 i;

    for (i = MAXIF - 1; i >= 0; i--)
    {
        if (if_comp[i].used == TRUE)
        {
            return (i);
        }
    }
}

U1 set_else_jmp (S4 ind)
{
    U1 label[MAXJUMPNAME + 1];
    U1 labelnum[10];

    strcpy (label, "else_");
    sprintf (labelnum, "%d", ind);
    strcat (label, labelnum);

    printf ("set_else_jmp: label: %s\n", label);

    if (jumplist_gonext () == FALSE)
    {
        return (FALSE);
    }

    strcpy (jumplist[jumplist_ind].lab, label);
    jumplist[jumplist_ind].pos = NOTDEF;
    jumplist[jumplist_ind].islabel = FALSE;

    if_comp[ind].else_pos = jumplist_ind;
    return (TRUE);
}

U1 set_endif_jmp (S4 ind)
{
    U1 label[MAXJUMPNAME + 1];
    U1 labelnum[10];

    strcpy (label, "endif_");
    sprintf (labelnum, "%d", ind);
    strcat (label, labelnum);

    printf ("set_endif_jmp: label: %s\n", label);

    if (jumplist_gonext () == FALSE)
    {
        return (FALSE);
    }

    strcpy (jumplist[jumplist_ind].lab, label);
    jumplist[jumplist_ind].pos = NOTDEF;
    jumplist[jumplist_ind].islabel = FALSE;

    if_comp[ind].endif_pos = jumplist_ind;
    return (TRUE);
}

U1 set_else_label (S4 ind)
{
    U1 ret;
    U1 label[MAXJUMPNAME + 1];
    U1 labelnum[10];

    strcpy (label, "else_");
    sprintf (labelnum, "%d", ind);
    strcat (label, labelnum);

    printf ("DEBUG: set_else_label: '%s'\n", label);

    ret = compile_set_label (label);

    printf ("ret: %i\n", ret);

    return (ret);
}

U1 set_endif_label (S4 ind)
{
    U1 ret;
    U1 label[MAXJUMPNAME + 1];
    U1 labelnum[10];

    strcpy (label, "endif_");
    sprintf (labelnum, "%d", ind);
    strcat (label, labelnum);

    if_comp[ind].used = IF_FINISHED;

    ret = compile_set_label (label);
    return (ret);
}

void set_endif_finished (S4 ind)
{
    if_comp[ind].used = IF_FINISHED;
}

S4 get_else_lab (S4 ind)
{
    return (if_comp[ind].else_pos);
}

S4 get_endif_lab (S4 ind)
{
    return (if_comp[ind].endif_pos);
}

S4 get_else_set (S4 ind)
{
    return (jumplist[if_comp[ind].else_pos].pos);
}
