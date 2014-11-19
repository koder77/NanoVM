/****************************************************************************
*
* Filename: dynamic_array.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  dynamic array functions: every index can be any type!
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

U1 exe_let_dyn_array2_qint (struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3)
{
    S8 *new_q_memptr;
    struct dynamic_array *dyn;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    dyn = (struct dynamic_array *) varlist[reg1].s_m;
    new_q_memptr = (S8 *) dyn[reg2].memptr;

    *reg3 = *new_q_memptr;

    return (TRUE);
}

U1 exe_let_dyn_array2_double (struct varlist *varlist, S4 reg1, S8 reg2, F8 *reg3)
{
    F8 *new_d_memptr;
    struct dynamic_array *dyn;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    dyn = (struct dynamic_array *) varlist[reg1].s_m;
    new_d_memptr = (F8 *) dyn[reg2].memptr;

    *reg3 = *new_d_memptr;

    return (TRUE);
}

U1 exe_let_dyn_array2_string (struct varlist *varlist, S4 reg1, S8 reg2, U1 *s_reg3)
{
    struct dynamic_array *dyn;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    dyn = (struct dynamic_array *) varlist[reg1].s_m;
    strcpy (s_reg3, dyn[reg2].memptr);

    return (TRUE);
}

/* ============================================================================ */

U1 exe_let_2_dyn_array_qint (struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3)
{
    /* allocate RAM for dynamic variable entry */
    /* reg2[reg3] = reg1 */

    S8 *new_q_memptr;
    struct dynamic_array *dyn;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    dyn = (struct dynamic_array *) varlist[reg2].s_m;
    
    /* free allocated RAM */
    if (dyn[reg3].memptr) free (dyn[reg3].memptr);
    
    dyn[reg3].memptr = (S8 *) malloc (sizeof (S8));
    if (dyn[reg3].memptr == NULL)
    {
        /* no memory */
        
        return (FALSE);
    }

    new_q_memptr = (S8 *) dyn[reg3].memptr;

    *new_q_memptr = reg1;
    
    /* set type */
    dyn[reg3].type = QINT_VAR;

    return (TRUE);
}

U1 exe_let_2_dyn_array_double (struct varlist *varlist, S8 reg1, S4 reg2, S8 reg3)
{
    /* allocate RAM for dynamic variable entry */
    /* reg2[reg3] = reg1 */

    F8 *new_d_memptr;
    struct dynamic_array *dyn;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    dyn = (struct dynamic_array *) varlist[reg2].s_m;
    
    /* free allocated RAM */
    if (dyn[reg3].memptr) free (dyn[reg3].memptr);
    
    dyn[reg3].memptr = (F8 *) malloc (sizeof (F8));
    if (dyn[reg3].memptr == NULL)
    {
        /* no memory */
        
        return (FALSE);
    }

    new_d_memptr = (F8 *) dyn[reg3].memptr;

    *new_d_memptr = reg1;
    
    /* set type */
    dyn[reg3].type = DOUBLE_VAR;

    return (TRUE);
}

U1 exe_let_2_dyn_array_string (struct varlist *varlist, U1 *s_reg1, S4 reg2, S8 reg3)
{
 /* allocate RAM for dynamic variable entry */
    /* reg2[reg3] = reg1 */

    struct dynamic_array *dyn;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    dyn = (struct dynamic_array *) varlist[reg2].s_m;
    
    /* free allocated RAM */
    if (dyn[reg3].memptr) free (dyn[reg3].memptr);
    
    dyn[reg3].memptr = (U1 *) malloc (strlen (s_reg1) + 1);
    if (dyn[reg3].memptr == NULL)
    {
        /* no memory */
        
        return (FALSE);
    }

    strcpy (dyn[reg3].memptr, s_reg1);
    
    /* set type */
    dyn[reg3].type = STRING_VAR;

    return (TRUE);
}

U1 exe_dyngettype (struct varlist *varlist, S4 reg1, S8 reg2, S8 *reg3)
{
    struct dynamic_array *dyn;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    dyn = (struct dynamic_array *) varlist[reg1].s_m;
    *reg3 = dyn[reg2].type;
    
    return (TRUE);
}
