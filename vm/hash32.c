/****************************************************************************
*
* Filename: hash32.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  hash functions (32 bit)
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


S4 hash32 (U1 *str, S4 size)
{
    /* djb2 algorithm by Dan Bernstein (comp.lang.c.) (http://www.cse.yorku.ca/~oz/hash.html) */

    S4 hash = 5381;
    S4 c;
    S4 i;

    for (i = 0; i < size; i++)
    {
        c = *str++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

U1 exe_hash32_array_byte (struct varlist *varlist, S4 b_var, S4 len, S4 *reg)
{
    U1 ret;
    S4 i;

    if (len < 0)
    {
        return (FALSE);
    }
    if (len > varlist[b_var].size)
    {
        return (FALSE);
    }

    *reg = hash32 (varlist[b_var].s_m, len);
    return (TRUE);
}
