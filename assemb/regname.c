/****************************************************************************
*
* Filename: regname.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  intialize register names
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

extern struct vmreg_name vmreg_name;

void init_regname_l (void)
{
    S4 i;

    for (i = 0; i <= MAXVMREG - 1; i++)
    {
        strcpy (vmreg_name.l[i], "");
    }
}

void init_regname_d (void)
{
    S4 i;

    for (i = 0; i <= MAXVMREG - 1; i++)
    {
        strcpy (vmreg_name.d[i], "");
    }
}

void init_regname_s (void)
{
    S4 i;

    for (i = 0; i <= MAXVMREG - 1; i++)
    {
        strcpy (vmreg_name.s[i], "");
    }
}

void init_regname_all (void)
{
    init_regname_l ();
    init_regname_d ();
    init_regname_s ();
}
