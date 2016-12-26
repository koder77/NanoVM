/****************************************************************************
*
* Filename: regspill.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  N compiler: register spill functions
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

extern struct vmreg_comp vmreg_comp;
extern struct vmreg_comp_info vmreg_comp_info;

extern struct varlist *varlist;
extern struct varlist *pvarlist_obj;

extern struct vmreg_comp vmreg_comp;


S2 spill_vmreg_l_old (S2 spillreg);
S2 spill_vmreg_d_old (S2 spillreg);
S2 spill_vmreg_s_old (S2 spillreg);

S2 spill_vmreg_l (void)
{
    S2 i;
    
    for (i = 0; i < (MAXVMREG_L / 2); i++)
    {
        /* spill half of registers */
        
        if (spill_vmreg_l_old (i) != i)
        {
            printf ("spill_vmreg_l: ERROR can't pull register %i to variable!\n", i);
            
            return (-1);
        }
    }
    
    return (0);
}

S2 spill_vmreg_l_old (S2 spillreg)
{
    S4 var;
    U1 type, private;
    
    var = vmreg_comp.l[spillreg];
    if (vmreg_comp.lp[spillreg] == PRIVATE)
    {
        type = pvarlist_obj[var].type;
        private = TRUE;
    }
    else
    {
        type = varlist[var].type;
        private = FALSE;
    }
    
    switch (type)
    {
        case INT_VAR:
            if (private)
            {
                if (! set2 (PPULL_I, spillreg, var))
                {
                    return (-1);
                }
            }
            else
            {
                if (! set2 (PULL_I, spillreg, var))
                {
                    return (-1);
                }
            }
            break;
            
        case LINT_VAR:
            if (private)
            {
                if (! set2 (PPULL_L, spillreg, var))
                {
                    return (-1);
                }
            }
            else
            {
                if (! set2 (PULL_L, spillreg, var))
                {
                    return (-1);
                }
            }
            break;
            
        case QINT_VAR:
            if (private)
            {
                if (! set2 (PPULL_Q, spillreg, var))
                {
                    return (-1);
                }
            }
            else
            {
                if (! set2 (PULL_Q, spillreg, var))
                {
                    return (-1);
                }
            }
            break;
        
        case BYTE_VAR:
            if (private)
            {
                if (! set2 (PPULL_B, spillreg, var))
                {
                    return (-1);
                }
            }
            else
            {
                if (! set2 (PULL_B, spillreg, var))
                {
                    return (-1);
                }
            }
            break;
    }
    unset_vmreg_l (var);
    
    return (spillreg);
}

S2 spill_vmreg_d (void)
{
    S2 i;
    
    for (i = 0; i < (MAXVMREG / 2); i++)
    {
        /* spill half of registers */
        
        if (spill_vmreg_d_old (i) != i)
        {
            printf ("spill_vmreg_d: ERROR can't pull register %i to variable!\n", i);
            
            return (-1);
        }
    }
    
    return (0);
}

S2 spill_vmreg_d_old (S2 spillreg)
{
    S4 var;
    U1 type, private;
            
    var = vmreg_comp.d[spillreg];
    if (vmreg_comp.dp[spillreg] == PRIVATE)
    {
        type = pvarlist_obj[var].type;
        private = TRUE;
    }
    else
    {
        type = varlist[var].type;
        private = FALSE;
    }
    
    if (private)
    {
        if (! set2 (PPULL_D, spillreg, var))
        {
            return (-1);
        }
    }
    else
    {
        if (! set2 (PULL_D, spillreg, var))
        {
            return (-1);
        }
    }
    
    unset_vmreg_d (var);
    return (spillreg);
}
   
S2 spill_vmreg_s (void)
{
    S2 i;
    
    for (i = 0; i < (MAXVMREG / 2); i++)
    {
        /* spill half of registers */
        
        if (spill_vmreg_s_old (i) != i)
        {
            printf ("spill_vmreg_s: ERROR can't pull register %i to variable!\n", i);
            
            return (-1);
        }
    }
    
    return (0);
}
   
S2 spill_vmreg_s_old (S2 spillreg)
{
    S4 var;
    U1 type, private;
            
    var = vmreg_comp.s[spillreg];
    if (vmreg_comp.sp[spillreg] == PRIVATE)
    {
        type = pvarlist_obj[var].type;
        private = TRUE;
    }
    else
    {
        type = varlist[var].type;
        private = FALSE;
    }
    
    if (private)
    {
        if (! set2 (PPULL_S, spillreg, var))
        {
            return (-1);
        }
    }
    else
    {
        if (! set2 (PULL_S, spillreg, var))
        {
            return (-1);
        }
    }
    
    unset_vmreg_s (var);
    return (spillreg);
}
