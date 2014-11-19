/****************************************************************************
*
* Filename: exe_arr.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  array functions
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

extern struct vm_mem vm_mem;


U1 exe_let_array2_int (struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3)
{
    /* reg3 = reg1[reg2] */

    S2 *new_i_memptr;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    new_i_memptr = (S2 *) get_vmvar (varlist, reg1, reg2, VM_READ);
    if (vm_mem.error)
    {
        return (FALSE);
    }

    *reg3 = *new_i_memptr;

    return (TRUE);
}

U1 exe_let_array2_lint (struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3)
{
    S4 *new_li_memptr;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    new_li_memptr = (S4 *) get_vmvar (varlist, reg1, reg2, VM_READ);
    if (vm_mem.error)
    {
        return (FALSE);
    }

    *reg3 = *new_li_memptr;

    return (TRUE);
}

U1 exe_let_array2_qint (struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3)
{
    S8 *new_q_memptr;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    new_q_memptr = (S8 *) get_vmvar (varlist, reg1, reg2, VM_READ);
    if (vm_mem.error)
    {
        return (FALSE);
    }

    *reg3 = *new_q_memptr;

    return (TRUE);
}

U1 exe_let_array2_double (struct varlist *varlist, S4 reg1, S8 reg2, F8 *reg3)
{
    F8 *new_d_memptr;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    new_d_memptr = (F8 *) get_vmvar (varlist, reg1, reg2, VM_READ);
    if (vm_mem.error)
    {
        return (FALSE);
    }

    *reg3 = *new_d_memptr;

    return (TRUE);
}

U1 exe_let_array2_byte (struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3)
{
    U1 *new_b_memptr;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    new_b_memptr = (U1 *) get_vmvar (varlist, reg1, reg2, VM_READ);
    if (vm_mem.error)
    {
        return (FALSE);
    }

    *reg3 = *new_b_memptr;

    return (TRUE);
}

U1 exe_let_array2_string (struct varlist *varlist, S4 reg1, S8 reg2, U1 *s_reg3)
{
    /* reg3 = reg1[reg2] */

    S4 var, pos, array_maxdim, array_string_len, i, j, ind, string_len;

    var = varlist[reg1].dimens[0];
    if (varlist[var].type == INT_VAR)
    {
        array_maxdim = *varlist[var].i_m;
    }
    else
    {
        array_maxdim = *varlist[var].li_m;
    }

    var = varlist[reg1].dimens[1];
    if (varlist[var].type == INT_VAR)
    {
        array_string_len = *varlist[var].i_m;
    }
    else
    {
        array_string_len = *varlist[var].li_m;
    }

    if (reg2 > array_maxdim - 1 || reg2 < 0)
    {
        /* array index overflow */
        return (FALSE);
    }

    /* calculate read start position */

    pos = reg2 * array_string_len;

    /* get string length */

    string_len = -1;
    j = pos;
    for (i = 1; i <= array_string_len; i++)
    {
        if (varlist[reg1].s_m[j] == BINUL)
        {
            string_len = i;
            break;
        }
        j++;
    }

    if (string_len == -1)
    {
        /* error: no string end found */
        return (FALSE);
    }

    if (string_len > MAXSTRING_VAR)
    {
        /* destination string overflow */
        return (FALSE);
    }

    /* all ok, copy string */

    j = pos; ind = 0;
    for (i = 1; i <= string_len; i++)
    {
        s_reg3[ind] = varlist[reg1].s_m[j];
        j++;
        ind++;
    }

    return (TRUE);
}


/* Always set vmcache_mod to TRUE on array write! */
/* This marks the cache as modified. */

U1 exe_let_2array_int (struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3)
{
    /* reg2[reg3] = reg1 */

    S2 *new_i_memptr;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    new_i_memptr = (S2 *) get_vmvar (varlist, reg2, reg3, VM_WRITE);
    if (vm_mem.error)
    {
        return (FALSE);
    }

    *new_i_memptr = reg1;

    return (TRUE);
}

U1 exe_let_2array_lint (struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3)
{
    S4 *new_li_memptr;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    new_li_memptr = (S4 *) get_vmvar (varlist, reg2, reg3, VM_WRITE);
    if (vm_mem.error)
    {
        return (FALSE);
    }

    *new_li_memptr = reg1;

    return (TRUE);
}

U1 exe_let_2array_qint (struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3)
{
    S8 *new_q_memptr;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    new_q_memptr = (S8 *) get_vmvar (varlist, reg2, reg3, VM_WRITE);
    if (vm_mem.error)
    {
        return (FALSE);
    }

    *new_q_memptr = reg1;

    return (TRUE);
}

U1 exe_let_2array_double (struct varlist *varlist, F8 reg1, S4 reg2, S8 reg3)
{
    F8 *new_d_memptr;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    new_d_memptr = (F8 *) get_vmvar (varlist, reg2, reg3, VM_WRITE);
    if (vm_mem.error)
    {
        return (FALSE);
    }

    *new_d_memptr = reg1;

    return (TRUE);
}

U1 exe_let_2array_byte (struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3)
{
    U1 *new_b_memptr;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    new_b_memptr = (U1 *) get_vmvar (varlist, reg2, reg3, VM_WRITE);
    if (vm_mem.error)
    {
        return (FALSE);
    }

    *new_b_memptr = reg1;

    return (TRUE);
}

U1 exe_let_2array_string (struct varlist *varlist, U1 *s_reg1, S4 reg2, S8 reg3)
{
    /* reg2[reg3] = reg1 */

    S4 var, pos, array_maxdim, array_string_len, i, j, ind, string_len;

    var = varlist[reg2].dimens[0];
    if (varlist[var].type == INT_VAR)
    {
        array_maxdim = *varlist[var].i_m;
    }
    else
    {
        array_maxdim = *varlist[var].li_m;
    }

    var = varlist[reg2].dimens[1];
    if (varlist[var].type == INT_VAR)
    {
        array_string_len = *varlist[var].i_m;
    }
    else
    {
        array_string_len = *varlist[var].li_m;
    }

    if (reg3 > array_maxdim - 1 || reg3 < 0)
    {
        /* array index overflow */
        return (FALSE);
    }

    /* calculate read start position */

    pos = reg3 * array_string_len;

    /* get string length */

    string_len = strlen (s_reg1) + 1;

    if (string_len > array_string_len)
    {
        /* array string overflow */
        return (FALSE);
    }

    /* all ok, copy string */

    j = pos; ind = 0;
    for (i = 1; i <= string_len; i++)
    {
        varlist[reg2].s_m[j] = s_reg1[ind];
        j++;
        ind++;
    }

    return (TRUE);
}
