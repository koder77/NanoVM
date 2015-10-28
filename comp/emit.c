/****************************************************************************
*
* Filename: emit.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  N compiler: assembler code emitter (unparser)
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
extern struct varlist *pvarlist_obj;
extern S4 **cclist;
extern S4 cclist_ind;
extern struct jumplist *jumplist;
extern struct opcode opcode[];
extern struct plist *plist;

U1 emit_code (U1 *filename)
{
    S4 i, j, t, dimens_ind, pline_old = -1, pline_new;
    FILE *fptr;

    U1 ch;

    fptr = fopen (filename, "w");
    if (fptr == NULL)
    {
        printf ("emit_code: error open file '%s'\n", filename);
        return (FALSE);
    }

    printmsg (SAVE_PROG, filename);

    printf ("emit_code: opcodes: %li\n", cclist_ind + 1);

    fprintf (fptr, "    ston;\n");

    for (i = 0; i <= cclist_ind; i++)
    {
        #if DEBUG
            printf ("\n\nsaving opcode: %li = %li\n", i, cclist[i][0]);

            printf ("plist: %s\n", plist[cclist[i][PLINELIST]].memptr);
        #endif

        pline_new = cclist[i][PLINELIST];

        if (cclist[i][0] == ASSEMB_INLINE && pline_new > pline_old)
        {
            #if DEBUG
                printf ("INLINE ASSEMBLER!\n");
            #endif

            /* inline assembler: write source line */
            if (plist[pline_new].line_len > 0)
            {
                fprintf (fptr, "%s", plist[pline_new].memptr);
            }

            pline_old = cclist[i][PLINELIST];
            continue;
        }

        if (cclist[i][0] == VM_LAB)
        {
            /* save label name */

            fprintf (fptr, "\n%s ", opcode[VM_LAB].op);
            fprintf (fptr, "%s;\n", jumplist[cclist[i][1]].lab);
        }
        else
        {
            switch (cclist[i][0])
            {
                case ALLOC:
                case ALLOC_PRIVATE:

                    if (cclist[i][0] == ALLOC)
                    {
                        fprintf (fptr, "    ");

                        #if DEBUG
                            printf ("emit: ALLOC type: %li, var: %li\n", varlist[cclist[i][1]].type, cclist[i][1]);
                        #endif

                        switch (varlist[cclist[i][1]].type)
                        {
                            case INT_VAR:
                                fprintf (fptr, "%s", COMP_INT_SB);
                                break;

                            case LINT_VAR:
                                fprintf (fptr, "%s", COMP_LONG_INT_SB);
                                break;

                            case QINT_VAR:
                                fprintf (fptr, "%s", COMP_QINT_SB);
                                break;

                            case DOUBLE_VAR:
                                fprintf (fptr, "%s", COMP_DOUBLE_SB);
                                break;

                            case STRING_VAR:
                                fprintf (fptr, "%s", COMP_STRING_SB);
                                break;

                            case BYTE_VAR:
                                fprintf (fptr, "%s", COMP_BYTE_SB);
                                break;

                            case DYNAMIC_VAR:
                                fprintf (fptr, "%s", COMP_DYNAMIC_SB);
                                break;

                            case INT_A_VAR:
                                fprintf (fptr, "%s", COMP_INT_SB);
                                break;

                            case LINT_A_VAR:
                                fprintf (fptr, "%s", COMP_LONG_INT_SB);
                                break;

                            case DOUBLE_A_VAR:
                                fprintf (fptr, "%s", COMP_DOUBLE_SB);
                                break;

                            case STRING_A_VAR:
                                fprintf (fptr, "%s", COMP_STRING_SB);
                                break;

                            case BYTE_A_VAR:
                                fprintf (fptr, "%s", COMP_BYTE_SB);
                                break;

                            case DYNAMIC_A_VAR:
                                fprintf (fptr, "%s", COMP_DYNAMIC_SB);
                                break;
                        }
                        fprintf (fptr, " %s", varlist[cclist[i][1]].name);

                        /* check for dimens: x[5]... */
                        if (varlist[cclist[i][1]].dims != NODIMS)
                        {
                            for (j = 0; j <= varlist[cclist[i][1]].dims; j++)
                            {
                                dimens_ind = varlist[cclist[i][1]].dimens[j];

                                if (strcmp (varlist[dimens_ind].name, "") == 0)
                                {
                                    /* no name: constant */
                                    if (varlist[dimens_ind].type == INT_VAR)
                                    {
                                        fprintf (fptr, "[%li]", *varlist[dimens_ind].i_m);
                                    }
                                    
                                    if (varlist[dimens_ind].type == LINT_VAR)
                                    {
                                        fprintf (fptr, "[%liL]", *varlist[dimens_ind].li_m);
                                    }
                                    
                                    if (varlist[dimens_ind].type == QINT_VAR)
                                    {
                                        fprintf (fptr, "[%liQ]", *varlist[dimens_ind].q_m);
                                    }
                                }
                                else
                                {
                                    /* save name */
                                    fprintf (fptr, "[%s]", varlist[dimens_ind].name);
                                }
                            }
                        }
                        fprintf (fptr, ";\n");
                    }
                    else
                    {
                        /* private variable */

                        fprintf (fptr, "    ");

                        #if DEBUG
                            printf ("emit: ALLOC PRIVATE type: %li, var: %li\n", pvarlist_obj[cclist[i][1]].type, cclist[i][1]);
                        #endif

                        switch (pvarlist_obj[cclist[i][1]].type)
                        {
                            case INT_VAR:
                                fprintf (fptr, "%s", COMP_INT_SB);
                                break;

                            case LINT_VAR:
                                fprintf (fptr, "%s", COMP_LONG_INT_SB);
                                break;

                            case QINT_VAR:
                                fprintf (fptr, "%s", COMP_QINT_SB);
                                break;

                            case DOUBLE_VAR:
                                fprintf (fptr, "%s", COMP_DOUBLE_SB);
                                break;

                            case STRING_VAR:
                                fprintf (fptr, "%s", COMP_STRING_SB);
                                break;

                            case BYTE_VAR:
                                fprintf (fptr, "%s", COMP_BYTE_SB);
                                break;

                            case DYNAMIC_VAR:
                                fprintf (fptr, "%s", COMP_DYNAMIC_SB);
                                break;

                            case INT_A_VAR:
                                fprintf (fptr, "%s", COMP_INT_SB);
                                break;

                            case LINT_A_VAR:
                                fprintf (fptr, "%s", COMP_LONG_INT_SB);
                                break;

                            case DOUBLE_A_VAR:
                                fprintf (fptr, "%s", COMP_DOUBLE_SB);
                                break;

                            case STRING_A_VAR:
                                fprintf (fptr, "%s", COMP_STRING_SB);
                                break;

                            case BYTE_A_VAR:
                                fprintf (fptr, "%s", COMP_BYTE_SB);
                                break;

                            case DYNAMIC_A_VAR:
                                fprintf (fptr, "%s", COMP_DYNAMIC_SB);
                                break;
                        }
                        fprintf (fptr, " %s", pvarlist_obj[cclist[i][1]].name);

                        /* check for dimens: x[5]... */
                        if (pvarlist_obj[cclist[i][1]].dims != NODIMS)
                        {
                            for (j = 0; j <= pvarlist_obj[cclist[i][1]].dims; j++)
                            {
                                dimens_ind = pvarlist_obj[cclist[i][1]].dimens[j];

                                if (strcmp (pvarlist_obj[dimens_ind].name, "") == 0)
                                {
                                    /* no name: constant */
                                    if (pvarlist_obj[dimens_ind].type == INT_VAR)
                                    {
                                        fprintf (fptr, "[%li]", *pvarlist_obj[dimens_ind].i_m);
                                    }
                                    
                                    if (pvarlist_obj[dimens_ind].type == LINT_VAR)
                                    {
                                        fprintf (fptr, "[%liL]", *pvarlist_obj[dimens_ind].li_m);
                                    }
                                    
                                    if (pvarlist_obj[dimens_ind].type == QINT_VAR)
                                    {
                                        fprintf (fptr, "[%liQ]", *pvarlist_obj[dimens_ind].q_m);
                                    }
                                }
                                else
                                {
                                    /* save name */
                                    fprintf (fptr, "[%s]", pvarlist_obj[dimens_ind].name);
                                }
                            }
                        }
                        fprintf (fptr, ";\n");
                    }
                    break;

                case PPOINTER:
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
					
					
                case PDYNGETTYPE:
                case PGETADDRESS:
                    
                    if (cclist[i][0] == PGETADDRESS)
                    {
                        cclist[i][0] = GETADDRESS;
                    }
                    
                    fprintf (fptr, "    %s ", opcode[cclist[i][0]].op);

                    for (j = 1; j <= opcode[cclist[i][0]].args; j++)
                    {
                        switch (opcode[cclist[i][0]].type[j - 1])
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                            case DOUBLE_VAR:
                            case STRING_VAR:
                            case BYTE_VAR:
                            case DYNAMIC_VAR:
                            case INT_A_VAR:
                            case LINT_A_VAR:
                            case QINT_A_VAR:
                            case DOUBLE_A_VAR:
                            case STRING_A_VAR:
                            case BYTE_A_VAR:
                            case DYNAMIC_A_VAR:
                                fprintf (fptr, "%s", pvarlist_obj[cclist[i][j]].name);
                                break;

                            case L_REG:
                                fprintf (fptr, "L%li", cclist[i][j]);
                                break;

                            case D_REG:
                                fprintf (fptr, "D%li", cclist[i][j]);
                                break;

                            case S_REG:
                                fprintf (fptr, "S%li", cclist[i][j]);
                                break;

                            case LABEL:
                            case LABEL_SET:
                                /* save label name */

                                fprintf (fptr, "%s", jumplist[cclist[i][j]].lab);
                                break;

                            case VAR:
                                fprintf (fptr, "%s", pvarlist_obj[cclist[i][j]].name);
                                break;
                        }
                        if (j <= opcode[cclist[i][0]].args -1) fprintf (fptr, ", ");
                    }
                    /* line end */
                    fprintf (fptr, ";\n");
                    break;

                default:
                    fprintf (fptr, "    %s ", opcode[cclist[i][0]].op);

                    #if DEBUG
                        printf ("opcode: argnum: %li\n", opcode[cclist[i][0]].args);
                    #endif

                    for (j = 1; j <= opcode[cclist[i][0]].args; j++)
                    {
                        #if DEBUG
                            printf ("opcode: type: %li\n", opcode[cclist[i][0]].type[j - 1]);
                        #endif

                        switch (opcode[cclist[i][0]].type[j - 1])
                        {
                            case INT_VAR:
                                #if DEBUG
                                    printf ("emit.c : int-var: %li\n", cclist[i][j]);
                                #endif

                                if (strcmp (varlist[cclist[i][j]].name, "") == 0)
                                {
                                    fprintf (fptr, "%i", *varlist[cclist[i][j]].i_m);
                                }
                                else
                                {
                                    fprintf (fptr, "%s", varlist[cclist[i][j]].name);
                                }
                                break;

                            case LINT_VAR:
                                #if DEBUG
                                    printf ("emit.c : lint-var: %li\n", cclist[i][j]);
                                #endif

                                if (strcmp (varlist[cclist[i][j]].name, "") == 0)
                                {
                                    fprintf (fptr, "%liL", *varlist[cclist[i][j]].li_m);
                                }
                                else
                                {
                                    fprintf (fptr, "%s", varlist[cclist[i][j]].name);
                                }
                                break;

                            case QINT_VAR:
                                #if DEBUG
                                    printf ("emit.c : qint-var: %li\n", cclist[i][j]);
                                #endif

                                if (strcmp (varlist[cclist[i][j]].name, "") == 0)
                                {
                                    fprintf (fptr, "%lliQ", *varlist[cclist[i][j]].q_m);
                                }
                                else
                                {
                                    fprintf (fptr, "%s", varlist[cclist[i][j]].name);
                                }
                                break;

                            case DOUBLE_VAR:
                                if (strcmp (varlist[cclist[i][j]].name, "") == 0)
                                {
                                    fprintf (fptr, "%.10lf", *varlist[cclist[i][j]].d_m);
                                }
                                else
                                {
                                    fprintf (fptr, "%s", varlist[cclist[i][j]].name);
                                }
                                break;

                            case STRING_VAR:
                                if (strcmp (varlist[cclist[i][j]].name, "") == 0)
                                {
                                    fprintf (fptr, "\"%s\"", varlist[cclist[i][j]].s_m);
                                }
                                else
                                {
                                    fprintf (fptr, "%s", varlist[cclist[i][j]].name);
                                }
                                break;

                            case BYTE_VAR:
                                if (strcmp (varlist[cclist[i][j]].name, "") == 0)
                                {
                                    ch = varlist[cclist[i][j]].s_m[0];
                                    fprintf (fptr, "%i\%%", ch);
                                }
                                else
                                {
                                    fprintf (fptr, "%s", varlist[cclist[i][j]].name);
                                }
                                break;

                            case DYNAMIC_VAR:
                            case INT_A_VAR:
                            case LINT_A_VAR:
                            case QINT_A_VAR:
                            case DOUBLE_A_VAR:
                            case STRING_A_VAR:
                            case BYTE_A_VAR:
                            case DYNAMIC_A_VAR:
                                fprintf (fptr, "%s", varlist[cclist[i][j]].name);
                                break;

                            case L_REG:
                                fprintf (fptr, "L%li", cclist[i][j]);
                                break;

                            case D_REG:
                                fprintf (fptr, "D%li", cclist[i][j]);
                                break;

                            case S_REG:
                                fprintf (fptr, "S%li", cclist[i][j]);
                                break;

                            case LABEL:
                            case LABEL_SET:
                                /* save label name */

                                fprintf (fptr, "%s", jumplist[cclist[i][j]].lab);
                                break;

                            case VAR:
                                fprintf (fptr, "%s", varlist[cclist[i][j]].name);

                                #if DEBUG
                                    printf ("DEBUG: variable index: %li\n", cclist[i][j]);
                                #endif

                                break;
                        }
                        if (j <= opcode[cclist[i][0]].args -1) fprintf (fptr, ", ");
                    }
                    /* line end */
                    fprintf (fptr, ";\n");
                    fflush (fptr);              /* save line now!!! */
                    break;
            }
        }
    }
    fclose (fptr);
    printmsg (STATUS_OK, "");
    return (TRUE);
}
