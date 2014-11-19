/****************************************************************************
*
* Filename: var_comp.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  N compiler: variable functions special for N compiler
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

extern struct varlist *varlist;
extern struct varlist *pvarlist_obj;                /* thread private varlist */
extern struct varlist_state varlist_state;
extern struct varlist_state pvarlist_state;
extern struct t_var t_var;

extern struct function function[MAXFUNCTIONS];
extern S2 function_ind;

extern S4 plist_ind;

#if DEBUG
    extern U1 print_debug;
#endif

S4 getvarind_comp (U1 *str)
{
    S4 i, ind = NOTDEF;
    U1 ok = FALSE;
    U1 varname[MAXLINELEN + 1];

    if (str[0] != '_')
    {
        if ((searchstr (str, AT_SB, 0, strlen (str) - 1, TRUE) == -1) || strlen (str) == 1)
        {
            /* function name not already tacked on = do it! */

            #if DEBUG
                printf ("getvarind: @ tacked on function name!\n");
            #endif

            strcpy (varname, str);
            strcat (varname, AT_SB);
            strcat (varname, function[function_ind].name);
        }
        else
        {
            strcpy (varname, str);
        }
    }
    else
    {
        /* global variable starting with "_": exp: _foobar */
        strcpy (varname, str);
    }

    #if DEBUG
        printf ("getvarind_comp: '%s'\n", varname);
    #endif

    if (varname[0] == PRIVATE_VAR_SB)
    {
        /* private varlist */

        i = 0;

        #if DEBUG
            printf ("PRIVATE VAR\n");
        #endif

        while (! ok)
        {
            if (strcmp (pvarlist_obj[i].name, varname) == 0)
            {
                ind = i, ok = TRUE;
            }
            if (i < pvarlist_state.maxvarlist)
            {
                i++;
            }
            else
            {
                ok = TRUE;
            }
        }
    }
    else
    {
        /* normal varlist */

        i = VARLIST_START;

        while (! ok)
        {
            if (strcmp (varlist[i].name, varname) == 0)
            {
                ind = i, ok = TRUE;
            }
            if (i < varlist_state.maxvarlist)
            {
                i++;
            }
            else
            {
                ok = TRUE;
            }
        }
    }

    #if DEBUG
        printf ("getvarind_comp: ind: %li\n", ind);
    #endif

    return (ind);
}

U1 getvarstr_comp (U1 *argstr, S2 pos_start)
{
    S2 pos, pos_end, endvn, dimstart, dimend;
    S4 varind, dims;
    U1 pstr[MAXLINELEN + 1], comm[2], dimensstr[MAXLINELEN + 1], private_var = NORMAL_VAR;
    U1 ok = FALSE;

    U1 varname[MAXLINELEN + 1];
    U1 buf[MAXLINELEN + 1];

    comm[0] = AROPEN_SB;
    comm[1] = BINUL;

     /* bugfix */
    pos_start = 0;
    pos_end = strlen (argstr) - 1;

    endvn = searchstr (argstr, comm, pos_start, pos_end, TRUE);
    if (endvn != -1)
    {
        if (argstr[0] != '_')
        {
            partstr (argstr, varname, pos_start, endvn - 1);
            strcat (varname, AT_SB);
            strcat (varname, function[function_ind].name);
            partstr (argstr, buf, endvn, pos_end);
            strcat (varname, buf);
        }
        else
        {
            partstr (argstr, varname, pos_start, pos_end);
        }
    }
    else
    {
        if (argstr[0] != '_')
        {
            strcpy (varname, argstr);
            strcat (varname, AT_SB);
            strcat (varname, function[function_ind].name);
        }
        else
        {
            /* global variable starting with "_": exp: _foobar */
            strcpy (varname, argstr);
        }
    }

    t_var.dims = NODIMS;
    pos_end = strlen (varname) - 1;

    if (varname[0] == PRIVATE_VAR_SB)
    {
        /* use pvarlist for dimens */

        private_var = PRIVATE_VAR;

        #if DEBUG
            printf ("getvarstr: PRIVATE_VAR SET\n");
        #endif
    }

    partstr (varname, pstr, pos_start, pos_end);

    #if DEBUG
        printf ("getvarstr: argstr: '%s'\n", varname);
    #endif

    /* bugfix */
    pos_start = 0;
    pos_end = strlen (pstr) - 1;

    endvn = searchstr (pstr, comm, pos_start, pos_end, TRUE);
    if (endvn != -1)
    {
        /* area variable, exp. a[5][x] */

        dimstart = endvn;
        endvn--;
        dims = NODIMS;

        while (! ok)
        {
            comm[0] = ARCLOSE_SB;
            comm[1] = BINUL;

            dimend = searchstr (pstr, comm, dimstart + 1, pos_end, TRUE);
            if (dimend != -1)
            {
                if (dims < MAXDIMENS - 1)
                {
                    dims++;
                }
                else
                {
                    printerr (OVERFLOW_IND, plist_ind, ST_PRE, t_var.varname);
                    return (FALSE);
                }

                /*  getstr (U1 *str, U1 *retstr, S2 maxlen, S2 start, S2 end) */

                if (private_var == NORMAL_VAR)
                {
                    getstr (pstr, dimensstr, MAXLINELEN, dimstart + 1, dimend - 1);
                    if (checkdigit (dimensstr) == TRUE)
                    {
                        if (! make_val (INT_VAR, varlist, NORMAL_VAR))
                        {
                            printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
                            return (FALSE);
                        }
                        t_var.dimens[dims] = t_var.varlist_ind;
                    }
                    else
                    {
                        /* variable, */
                        /* check if already defined */

                        varind = getvarind_comp (dimensstr);
                        if (varind != -1)
                        {
                            if (varlist[varind].type != STRING_VAR && varlist[varind].type != BYTE_VAR)
                            {
                                t_var.dimens[dims] = varind;
                            }
                            else
                            {
                                /* error: its a string or byte var */

                                printerr (WRONG_VARTYPE, plist_ind, ST_PRE, dimensstr);
                                return (FALSE);
                            }
                        }
                        else
                        {
                            printerr (NOTDEF_VAR, plist_ind, ST_PRE, dimensstr);
                            return (FALSE);
                        }
                    }
                }
                else
                {
                    /* private variable */

                    #if DEBUG
                        printf ("getvarstr: PRIVATE VAR!");
                    #endif

                    getstr (pstr, dimensstr, MAXLINELEN, dimstart + 1, dimend - 1);
                    if (checkdigit (dimensstr) == TRUE)
                    {
                        if (! make_val (INT_VAR, pvarlist_obj, PRIVATE_VAR))
                        {
                            printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
                            return (FALSE);
                        }
                        t_var.dimens[dims] = t_var.varlist_ind;

                        #if DEBUG
                            printf ("getvarstr: PRIVATE VAR: %li", t_var.varlist_ind);
                        #endif
                    }
                    else
                    {
                        /* variable, */
                        /* check if already defined */

                        varind = getvarind_comp (dimensstr);
                        if (varind != -1)
                        {
                            if (pvarlist_obj[varind].type != STRING_VAR && pvarlist_obj[varind].type != BYTE_VAR)
                            {
                                t_var.dimens[dims] = varind;
                            }
                            else
                            {
                                /* error: its a string or byte var */

                                printerr (WRONG_VARTYPE, plist_ind, ST_PRE, dimensstr);
                                return (FALSE);
                            }
                        }
                        else
                        {
                            printerr (NOTDEF_VAR, plist_ind, ST_PRE, dimensstr);
                            return (FALSE);
                        }
                    }
                }
            }
            else
            {
                /* no ] */

                printerr (BRNOT_OK, plist_ind, ST_PRE, "");
                return (FALSE);
            }

            comm[0] = AROPEN_SB;
            comm[1] = BINUL;

            if (dimend < pos_end)
            {
                pos = searchstr (pstr, comm, dimend + 1, pos_end, TRUE);
                if (pos != -1)
                {
                    dimstart = pos;
                }
                else
                {
                    ok = TRUE;
                }
            }
            else
            {
                ok = TRUE;
            }
        }
        getstr (pstr, t_var.varname, MAXVARNAME, pos_start, endvn);
        t_var.dims = dims;

        #if DEBUG
            printf ("getvarstr: dims = %li\n", dims);
        #endif
    }
    else
    {
        getstr (pstr, t_var.varname, MAXVARNAME, pos_start, pos_end);
    }
    return (TRUE);
}
