/****************************************************************************
*
* Filename: verifyo.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  verify object code
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2006
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

extern struct opcode opcode[];
extern struct varlist *varlist;
extern struct vm_mem vm_mem;
extern S4 **cclist;

extern S4 maxcclist;

extern struct varlist *pvarlist_obj;

U1 exe_verify_object ()
{
    U1 opcode_type, exit_opcode = FALSE, private_var;
    S4 i, j;

    for (i = 0; i <= maxcclist; i++)
    {
        if (cclist[i][0] < 0 || cclist[i][0] > MAXOPCODE)
        {
            /* opcode not in legal range */

            #if DEBUG
                printf ("verify error: opcode not in legal range\n");
            #endif

            return (FALSE);
        }

        if (cclist[i][0] == PEXIT)
        {
            /* found exit opcode */

            exit_opcode = TRUE;
        }

        /* check if it's a private variable opcode */
        switch (cclist[i][0])
        {
            case ALLOC_PRIVATE:
            case DEALLOC_PRIVATE:

            case PPUSH_I:
            case PPUSH_L:
            case PPUSH_Q:
            case PPUSH_D:
            case PPUSH_B:
            case PPUSH_S:

            case PPULL_I:
            case PPULL_L:
            case PPULL_Q:
            case PPULL_D:
            case PPULL_B:
            case PPULL_S:

            case PMOVE_A_I:
            case PMOVE_A_L:
            case PMOVE_A_Q:
            case PMOVE_A_D:
            case PMOVE_A_B:
            case PMOVE_A_S:

            case PMOVE_I_A:
            case PMOVE_L_A:
            case PMOVE_Q_A:
            case PMOVE_D_A:
            case PMOVE_B_A:
            case PMOVE_S_A:

            case PFREAD_AB:
            case PFWRITE_AB:
            case PSREAD_AB:
            case PSWRITE_AB:

            case PHASH32_AB:
            case PHASH64_AB:
                    
            case PMOVE_ADYN_Q:
            case PMOVE_ADYN_D:
            case PMOVE_ADYN_S:

            case PMOVE_Q_ADYN:
            case PMOVE_D_ADYN:
            case PMOVE_S_ADYN:

            case PDYNGETTYPE:

            case PVADD_L:
            case PVADD_D:
            case PVSUB_L:
            case PVSUB_D:

            case PVMUL_L:
            case PVMUL_D:

            case PVDIV_L:
            case PVDIV_D:


            case PVADD2_L:
            case PVADD2_D:

            case PVSUB2_L:
            case PVSUB2_D:

            case PVMUL2_L:
            case PVMUL2_D:

            case PVDIV2_L:
            case PVDIV2_D:

            case PPOINTER:
            case PGETTYPE:

            case PMOVE_IP_B:
            case PMOVE_LP_B:
            case PMOVE_QP_B:
            case PMOVE_DP_B:

            case PMOVE_B_IP:
            case PMOVE_B_LP:
            case PMOVE_B_QP:
            case PMOVE_B_DP:
                private_var = TRUE;
                break;

            default:
                private_var = FALSE;
                break;
        }

        if (opcode[cclist[i][0]].args > 0)
        {
            /* check opcode arguments */

            for (j = 1; j <= opcode[cclist[i][0]].args; j++)
            {
                opcode_type = opcode[cclist[i][0]].type[j - 1];

                if (opcode_type == L_REG || opcode_type == D_REG || opcode_type == S_REG)
                {
                     /* check registers */

                    if (cclist[i][j] < 0 || cclist[i][j] > MAXVMREG)
                    {
                        /* register not in legal range */

                        #if DEBUG
                            printf ("verify error: register not in legal range\n");
                        #endif

                        return (FALSE);
                    }
                }
                else
                {
                    if (opcode_type == LABEL)
                    {
                        /* check labels */

                        if (cclist[i][j] < 0 || cclist[i][j] > maxcclist)
                        {
                            /* label not in legal range */

                            #if DEBUG
                                printf ("verify error: label not in legal range\n");
                                printf ("line number: %li\n", cclist[i][BCELIST]);
                            #endif

                            return (FALSE);
                        }
                    }
                    else
                    {
                        /* check variables */

                        if (private_var == TRUE)
                        {
                            if (cclist[i][j] < 0 || cclist[i][j] > vm_mem.obj_maxpvarlist)
                            {
                                /* argument not in legal variable range */

                                #if DEBUG
                                    printf ("verify error: argument not in legal range\n");
                                    printf ("cclist: %i, maxpvarlist: %i, opcode: %i, line: %i\n", (int) cclist[i][j], (int) vm_mem.obj_maxpvarlist, (int) i, (int) cclist[i][BCELIST]);
                                #endif

                                return (FALSE);
                            }
                        }
                        else
                        {
                            if (cclist[i][j] < 0 || cclist[i][j] > vm_mem.obj_maxvarlist)
                            {
                                /* argument not in legal variable range */

                                #if DEBUG
                                    printf ("verify error: argument not in legal range\n");
                                    printf ("cclist: %i, maxvarlist: %i, opcode: %i, line %i\n", (int) cclist[i][j], (int) vm_mem.obj_maxvarlist, (int) i, (int) cclist[i][BCELIST]);
                                #endif

                                return (FALSE);
                            }
                        }

                        if (private_var == FALSE)
                        {
                            switch (opcode_type)
                            {
                                case INT_A_VAR:
                                    if (varlist[cclist[i][j]].type != INT_VAR || varlist[cclist[i][j]].dims == NODIMS)
                                    {
                                        return (FALSE);
                                    }
                                    break;

                                case LINT_A_VAR:
                                    if (varlist[cclist[i][j]].type != LINT_VAR || varlist[cclist[i][j]].dims == NODIMS)
                                    {
                                        return (FALSE);
                                    }
                                    break;

                                case QINT_A_VAR:
                                    if (varlist[cclist[i][j]].type !=QINT_VAR || varlist[cclist[i][j]].dims == NODIMS)
                                    {
                                        return (FALSE);
                                    }
                                    break;

                                case DOUBLE_A_VAR:
                                    if (varlist[cclist[i][j]].type != DOUBLE_VAR || varlist[cclist[i][j]].dims == NODIMS)
                                    {
                                        return (FALSE);
                                    }
                                    break;

                                case BYTE_A_VAR:
                                    if (varlist[cclist[i][j]].type != BYTE_VAR || varlist[cclist[i][j]].dims == NODIMS)
                                    {
                                        return (FALSE);
                                    }
                                    break;
                                    
                               case DYNAMIC_A_VAR:
                                    if (varlist[cclist[i][j]].type != DYNAMIC_VAR || varlist[cclist[i][j]].dims == NODIMS)
                                    {
                                        return (FALSE);
                                    }
                                    break; 

                                default:
                                    if (opcode_type != VAR)
                                    {
                                        if (opcode_type != varlist[cclist[i][j]].type)
                                        {
                                            /* wrong argument type */

                                            #if DEBUG
                                                printf ("verify error: wrong argument type\n");
                                                printf ("opcode: %i, arg: %i\n", (int) cclist[i][0], (int) cclist[i][j]);
                                            #endif

                                            return (FALSE);
                                        }
                                    }
                                    break;
                            }
                        }
                        else
                        {
                            /* private variable use pvarlist_obj! */
                            switch (opcode_type)
                            {
                                case INT_A_VAR:
                                    if (pvarlist_obj[cclist[i][j]].type != INT_VAR || pvarlist_obj[cclist[i][j]].dims == NODIMS)
                                    {
                                        return (FALSE);
                                    }
                                    break;

                                case LINT_A_VAR:
                                    if (pvarlist_obj[cclist[i][j]].type != LINT_VAR || pvarlist_obj[cclist[i][j]].dims == NODIMS)
                                    {
                                        return (FALSE);
                                    }
                                    break;

                                case QINT_A_VAR:
                                    if (pvarlist_obj[cclist[i][j]].type != QINT_VAR || pvarlist_obj[cclist[i][j]].dims == NODIMS)
                                    {
                                        return (FALSE);
                                    }
                                    break;

                                case DOUBLE_A_VAR:
                                    if (pvarlist_obj[cclist[i][j]].type != DOUBLE_VAR || pvarlist_obj[cclist[i][j]].dims == NODIMS)
                                    {
                                        return (FALSE);
                                    }
                                    break;

                                case BYTE_A_VAR:
                                    if (pvarlist_obj[cclist[i][j]].type != BYTE_VAR || pvarlist_obj[cclist[i][j]].dims == NODIMS)
                                    {
                                        return (FALSE);
                                    }
                                    break;

                                case DYNAMIC_A_VAR:
                                    if (pvarlist_obj[cclist[i][j]].type != DYNAMIC_VAR || pvarlist_obj[cclist[i][j]].dims == NODIMS)
                                    {
                                        return (FALSE);
                                    }
                                    break; 

                                default:
                                    if (opcode_type != VAR)
                                    {
                                        if (opcode_type != pvarlist_obj[cclist[i][j]].type)
                                        {
                                            /* wrong argument type */

                                            #if DEBUG
                                                printf ("PRIVATE VAR:\n");
                                                printf ("verify error: wrong argument type\n");
                                                printf ("opcode: %i, arg: %i\n", (int) cclist[i][0], (int) cclist[i][j]);
                                            #endif

                                            return (FALSE);
                                        }
                                    }
                                    break;
                            }
                        }
                    }
                }
            } /* for */
        }
    }
    if (exit_opcode)
    {
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}
