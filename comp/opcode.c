/****************************************************************************
*
* Filename: opcode.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  N compiler: opcode and register handling functions
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

#define CC_NEXT();  if (! cclist_gonext ()) {return (FALSE);}

extern S4 **cclist;

extern S4 cclist_ind;
extern S4 maxcclist;
extern S4 cclist_size;
extern S4 plist_ind;

extern struct vmreg_comp vmreg_comp;
extern struct vmreg_comp_info vmreg_comp_info;

extern U1 nested_code;
extern U1 nested_code_global_off;

void init_vmreg (void)
{
    S2 i;

    for (i = 0; i < MAXVMREG; i++)
    {
        vmreg_comp.l[i] = EMPTY;
        vmreg_comp.d[i] = EMPTY;
        vmreg_comp.s[i] = EMPTY;
        
        vmreg_comp.lp[i] = FALSE;
        vmreg_comp.dp[i] = FALSE;
        vmreg_comp.sp[i] = FALSE;
        
        vmreg_comp_info.l[i] = 0;
        vmreg_comp_info.d[i] = 0;
        vmreg_comp_info.s[i] = 0;
    }
}

S2 get_vmreg_l (void)
{
    S2 i, ret;

    for (i = 0; i <= MAXVMREG_L; i++)
    {
        if (vmreg_comp.l[i] == EMPTY) return (i);
    }
    ret = spill_vmreg_l ();
    return (ret);
}

S2 get_vmreg_d (void)
{
    S2 i, ret;

    for (i = 0; i < MAXVMREG; i++)
    {
        if (vmreg_comp.d[i] == EMPTY) return (i);
    }
    ret = spill_vmreg_d ();
    return (ret);
}

S2 get_vmreg_s (void)
{
    S2 i, ret;

    for (i = 0; i < MAXVMREG; i++)
    {
        if (vmreg_comp.s[i] == EMPTY) return (i);
    }
    ret = spill_vmreg_s ();
    return (ret);
}

void set_vmreg_l (S2 reg, S4 var, U1 fprivate)
{
    S2 i, info = 1;

    for (i = 0; i <= MAXVMREG_L; i++)
    {
        if (vmreg_comp.l[i] == var)
        {
            vmreg_comp.l[i] = EMPTY;
            info = vmreg_comp_info.l[i];
        }
    }

    vmreg_comp.l[reg] = var;
    vmreg_comp.lp[reg] = fprivate;
    vmreg_comp_info.l[reg] = vmreg_comp_info.l[reg] + info;
}

void set_vmreg_d (S2 reg, S4 var, U1 fprivate)
{
    S2 i, info = 1;

    for (i = 0; i < MAXVMREG; i++)
    {
        if (vmreg_comp.d[i] == var)
        {
            vmreg_comp.d[i] = EMPTY;
            info = vmreg_comp_info.d[i];
        }
    }
    
    vmreg_comp.d[reg] = var;
    vmreg_comp.dp[reg] = fprivate;
    vmreg_comp_info.d[reg] = vmreg_comp_info.d[reg] + info;
}

void set_vmreg_s (S2 reg, S4 var, U1 fprivate)
{
    S2 i, info = 1;

    for (i = 0; i <= MAXVMREG_L; i++)
    {
        if (vmreg_comp.s[i] == var)
        {
            vmreg_comp.s[i] = EMPTY;
            info = vmreg_comp_info.s[i];
        }
    }
    
    vmreg_comp.s[reg] = var;
    vmreg_comp.sp[reg] = fprivate;
    vmreg_comp_info.s[reg] = vmreg_comp_info.s[reg] + info;
}

/* ============================================================================ */

void unset_vmreg_l (S4 var)
{
    S2 i;

    if (nested_code_global_off == FALSE)
    {
    for (i = 0; i <= MAXVMREG_L; i++)
    {
        if (vmreg_comp.l[i] == var)
        {
            vmreg_comp.l[i] = EMPTY;
            vmreg_comp_info.l[i] = 0;
        }
    }
    }
}

void unset_vmreg_d (S4 var)
{
    S2 i;

    if (nested_code_global_off == FALSE)
    {
    for (i = 0; i <= MAXVMREG_L; i++)
    {
        if (vmreg_comp.d[i] == var)
        {
            vmreg_comp.d[i] = EMPTY;
            vmreg_comp_info.d[i] = 0;
        }
    }
    }
}

void unset_vmreg_s (S4 var)
{
    S2 i;

    if (nested_code_global_off == FALSE)
    {
    for (i = 0; i <= MAXVMREG_L; i++)
    {
        if (vmreg_comp.s[i] == var)
        {
            vmreg_comp.s[i] = EMPTY;
            vmreg_comp_info.s[i] = 0;
        }
    }
    }
}


/* search register for variable index */
S2 get_vmreg_var_l (S4 var)
{
    S2 i;

    for (i = 0; i < MAXVMREG; i++)
    {
        if (vmreg_comp.l[i] == var && nested_code == FALSE) return (i);
    }
    return (EMPTY);     /* variable not found in registers */
}

S2 get_vmreg_var_d (S4 var)
{
    S2 i;

    for (i = 0; i < MAXVMREG; i++)
    {
        if (vmreg_comp.d[i] == var && nested_code == FALSE) return (i);
    }
    return (EMPTY);     /* variable not found in registers */
}

S2 get_vmreg_var_s (S4 var)
{
    S2 i;

    for (i = 0; i < MAXVMREG; i++)
    {
        if (vmreg_comp.s[i] == var && nested_code == FALSE) return (i);
    }
    return (EMPTY);     /* variable not found in registers */
}

/* get variable from register x */

S2 get_vmreg_val_l (S4 i)
{
    return (vmreg_comp.l[i]);
}

S2 get_vmreg_val_d (S4 i)
{
    return (vmreg_comp.d[i]);
}

S2 get_vmreg_val_s (S4 i)
{
    return (vmreg_comp.s[i]);
}


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

U1 set0 (S4 comm)
{
    CC_NEXT ();

    cclist[cclist_ind][0] = comm;

    return (TRUE);
}

U1 set1 (S4 comm, S4 vind1)
{
    CC_NEXT ();

    cclist[cclist_ind][0] = comm;
    cclist[cclist_ind][1] = vind1;

    return (TRUE);
}

U1 set2 (S4 comm, S4 vind1, S4 vind2)
{
    CC_NEXT ();

    cclist[cclist_ind][0] = comm;
    cclist[cclist_ind][1] = vind1;
    cclist[cclist_ind][2] = vind2;

    return (TRUE);
}

U1 set3 (S4 comm, S4 vind1, S4 vind2, S4 vind3)
{
    CC_NEXT ();

    cclist[cclist_ind][0] = comm;
    cclist[cclist_ind][1] = vind1;
    cclist[cclist_ind][2] = vind2;
    cclist[cclist_ind][3] = vind3;

    return (TRUE);
}
/*
void set_pline (S4 linenumber)
{
    if (cclist_ind > -1)
    {
        cclist[cclist_ind][BCELIST] = plist[linenumber].srcline;
        cclist[cclist_ind][INCLUDELIST] = plist[linenumber].include;
    }
}
*/
