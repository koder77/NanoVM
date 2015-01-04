/****************************************************************************
*
* Filename: compiler.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  N compiler: compiler
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

extern struct opcode opcode;

extern struct src_line src_line;
extern struct plist *plist;

extern struct function function[MAXFUNCTIONS];

extern struct varlist *varlist;
extern struct varlist *pvarlist_obj;

extern struct varlist_state varlist_state;
extern struct varlist_state pvarlist_state;

extern struct t_var t_var;
extern struct if_comp if_comp[MAXIF];

/* ????? */
extern S4 function_ind;
extern S4 opcode_ind;
extern S4 if_ind;

extern S2 plist_ind;
extern S2 cclist_ind;
extern struct jumplist *jumplist;

extern S4 jumplist_ind;
extern S4 maxjumplist;
extern S4 jumplist_size;

extern U1 nested_code;
extern U1 nested_code_global_off;

extern U1 atomic;

U1 compile_set_init_var (S2 arg, S4 *var, U1 type)
{
    U1 create_variable = FALSE, create_variable_private = FALSE, private_variable = FALSE;

    if (! getvarstr_comp (src_line.arg[arg], 0))
    {
        return (FALSE);
    }

    if (t_var.varname[0] == COMP_PRIVATE_VAR_SB)
    {
         private_variable = TRUE;

         *var = getvarind_comp (t_var.varname);
         if (*var == NOTDEF) create_variable_private = TRUE;
    }
    else
    {
        *var = getvarind_comp (t_var.varname);
        if (*var == NOTDEF) create_variable = TRUE;
    }

    if (create_variable == TRUE || create_variable_private == TRUE)
    {
        /* set new variable */
        switch (type)
        {
            case COMP_BYTE:
                t_var.type = BYTE_VAR;
                break;

            case COMP_INT:
                t_var.type = INT_VAR;
                break;

            case COMP_LONG_INT:
                t_var.type = LINT_VAR;
                break;

            case COMP_QINT:
                t_var.type = QINT_VAR;
                break;

            case COMP_DOUBLE:
                t_var.type = DOUBLE_VAR;
                break;

            case COMP_STRING:
                t_var.type = STRING_VAR;
                break;

            case COMP_DYNAMIC:
                t_var.type = DYNAMIC_VAR;
                break;
        }

        if (create_variable == TRUE)
        {
            if (! init_var (varlist, &varlist_state))
            {
                return (FALSE);
            }
            *var = varlist_state.varlist_ind;
        }
        if (create_variable_private == TRUE)
        {
            if (! init_var (pvarlist_obj, &pvarlist_state))
            {
                return (FALSE);
            }
            *var = pvarlist_state.varlist_ind;
        }
    }

    if (private_variable == FALSE)
    {
        if (! set1 (ALLOC, *var))
        {
            return (MEMORY);
        }
    }
    else
    {
        if (! set1 (ALLOC_PRIVATE, *var))
        {
            return (MEMORY);
        }
    }
    return (TRUE);
}

U1 compile_set_label (U1 *name)
{
    S2 jump, jumpsave;

    jump = getjumpind (name);
    if (jump != NOTDEF)
    {
        if (jumplist[jump].islabel == FALSE)
        {
            /* LAB already defined by a GOTO, so we set 'pos' now. */

            #if DEBUG
                printf ("DEBUG: compile_set_label: found already defined '%s'\n", name);
            #endif

            jumplist[jump].pos = cclist_ind + 1;
            jumpsave = jump;
        }
        else
        {
            printerr (DOUBDEF_LAB, plist_ind, ST_PRE, jumplist[jump].lab);
            return (FALSE);
        }
    }
    else
    {
        if (jumplist_gonext () == FALSE)
        {
            return (FALSE);
        }

        strcpy (jumplist[jumplist_ind].lab, name);
        jumplist[jumplist_ind].pos = cclist_ind + 1;
        jumplist[jumplist_ind].islabel = TRUE;
        jumpsave = jumplist_ind;
    }

    if (! set1 (VM_LAB, jumpsave))
    {
        printf ("compile_set_label: error set opcode VM_LAB\n");
        return (MEMORY);
    }

    return (TRUE);
}

U1 compile_set_function_name (U1 *name)
{
    if (function_ind < MAXFUNCTIONS)
    {
        function_ind++;
    }
    else
    {
        printf ("error: max functions reached, can't save function info!\n");
        return (FALSE);
    }

    strcpy (function[function_ind].name, name);
    return (TRUE);
}

U1 compile ()
{
    S4 i, str_len, var, var2, var3, var_i, opcode, if_pos, for_pos;
    U1 private_variable = FALSE, func_call_reverse_args;
	U1 get_no_stack_pull;
    U1 ok, found, type, type2, type3, type_i, op_found, set_pull_all, type_const, array_type;
    U1 buf[MAXLINELEN + 1];
    S2 reg1, reg2, reg3, translate;
	S4 list;

    #if DEBUG
        printf ("type def: args: %li\n", src_line.args);
    #endif

    switch (src_line.opcode_n)
    {
        case COMP_FUNC:
            /* new function: set all registers as EMPTY: */
            init_vmreg ();

            nested_code = FALSE;        /* only set if gotos used */
            nested_code_global_off = FALSE;

            #if DEBUG
                printf ("COMP_FUNC: '%s'\n", src_line.arg[0]);
            #endif

            if (! compile_set_label (src_line.arg[0])) return (FALSE); /* set function name as label = PSEUDO opcode for emitter */
            if (! compile_set_function_name (src_line.arg[0])) return (FALSE);
            if (src_line.args > 0)
            {
                /* get arguments from stack */
                i = 1;
                ok = TRUE;
                while (ok)
                {
                    if (strcmp (src_line.arg[i], COMP_SYNC_SB) == 0)
                    {
                        /* thread sync object */

                        if (! set0 (THREAD_PULL_SYNC))
                        {
                            return (MEMORY);
                        }
                    }
                    else
                    {
                        found = FALSE;
                        if (strcmp (src_line.arg[i], COMP_BYTE_SB) == 0)
                        {
                            /* init byte var and pull byte */

                            found = TRUE;
                            if (compile_set_init_var (i + 1, &var, COMP_BYTE) != TRUE)
                            {
                                return (MEMORY);
                            }

                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_L, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_B, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PULL_B, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }

                            }
                        }

                        if (strcmp (src_line.arg[i], COMP_INT_SB) == 0)
                        {
                            /* init int var and pull int */

                            found = TRUE;
                            if (compile_set_init_var (i + 1, &var, COMP_INT) != TRUE)
                            {
                                return (MEMORY);
                            }

                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_L , reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_I, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PULL_I, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }

                            }
                        }

                        if (strcmp (src_line.arg[i], COMP_LONG_INT_SB) == 0)
                        {
                            /* init long int var and pull long int */

                            found = TRUE;
                            if (compile_set_init_var (i + 1, &var, COMP_LONG_INT) != TRUE)
                            {
                                return (MEMORY);
                            }

                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_L , reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_L, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PULL_L, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }
                            }
                        }

                        if (strcmp (src_line.arg[i], COMP_QINT_SB) == 0)
                        {
                            /* init long int var and pull long int */

                            found = TRUE;
                            if (compile_set_init_var (i + 1, &var, COMP_QINT) != TRUE)
                            {
                                return (MEMORY);
                            }

                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_L , reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_Q, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PULL_Q, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }  
                            }
                        }

                        if (strcmp (src_line.arg[i], COMP_DOUBLE_SB) == 0)
                        {
                            /* init double var and pull double */

                            found = TRUE;
                            if (compile_set_init_var (i + 1, &var, COMP_DOUBLE) != TRUE)
                            {
                                return (MEMORY);
                            }

                            reg1 = get_vmreg_d ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_D , reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_D, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                     set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PULL_D, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }
                            }
                        }

                        if (strcmp (src_line.arg[i], COMP_STRING_SB) == 0)
                        {
                            /* init string var and pull string */

                            found = TRUE;
                            if (compile_set_init_var (i + 1, &var, COMP_STRING) != TRUE)
                            {
                                return (MEMORY);
                            }

                            reg1 = get_vmreg_s ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_S , reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_S, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PULL_S, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }
                            }
                        }

                        if (found == FALSE)
                        {
                            printf ("compile: error: no argument type set: line %li\n", plist_ind);
                            return (FALSE);
                        }
                    }
                    if (i < src_line.args - 2)
                    {
                        i = i + 2;
                    }
                    else
                    {
                        ok = FALSE;
                    }
                }
            }
            break;

        case COMP_FUNC_END:
            /* set rts opcode */
            if (strcmp (function[function_ind].name, COMP_MAIN_SB) != 0)
            {
                /* not main function, set RTS opcode at function end */
                if (! set0 (RTS))
                {
                    return (MEMORY);
                }
            }
            break;

        case COMP_FUNC_CALL:
            /* function call, push arguments on stack in reverse order!!! */

            if (strcmp (src_line.arg[0], COMP_GET_MULTI_END_SB) == 0) goto getmultiend;
            
            if (src_line.args > 0)
            {
                if (strcmp (src_line.arg[0], COMP_PRINT_SB) == 0) goto print;
                if (strcmp (src_line.arg[0], COMP_PRINT_NEWLINE_SB) == 0) goto print_newline;
                if (strcmp (src_line.arg[0], COMP_GET_SB) == 0) goto get;
                if (strcmp (src_line.arg[0], COMP_GET_MULTI_SB) == 0) goto getmulti;
                if (strcmp (src_line.arg[0], COMP_RETURN_MULTI_SB) == 0) goto returnmulti;

				str_len = strlen (src_line.arg[0]) - 1;
				
				// printf ("'%c'\n", src_line.arg[0][i]);
				
				if (src_line.arg[0][str_len] == NOTREVERSE_SB)
				{
					func_call_reverse_args = FALSE;
					
					/* function call was: foobar> (x, y);
					 * 
					 * with the greater sign '>' to set not to reverse the order of the arguments pushed on stack!!!
					 * 
					 */
					src_line.arg[0][str_len] = BINUL; /* erase greater sign from function name */
				}
				else
				{
					func_call_reverse_args = TRUE;
				}
				
				printf ("src line arg 0: %s\n", src_line.arg[0]);
				
                translate = translate_code ();
                if (translate == TRUE)
                {
                    break;
                }

                if (strcmp (src_line.arg[0], COMP_RETURN_SB) != 0)
                {
                    /* save all registers */
                    
                    if (! set0 (STPUSH_ALL_ALL))
					{
						return (MEMORY);
					}
                }
                
                /* push arguments on stack */
                if (func_call_reverse_args == TRUE)
				{
					i = src_line.args;      /* beginn from back of list */
				}
				else
				{
					i = 1;					/* begin from start of list */
				}
				
                ok = TRUE;
                while (ok)
                {
					/* check if argument is string or number contstant */
					var = -1;
					if (checkstring (src_line.arg[i]))
					{
						if (! make_string ())
						{
							printf ("compile: error: can't create string, line %li\n", plist_ind);
							return (FALSE);
						}
						var = t_var.varlist_ind;
					}
					else
					{
						/* number variable */

						if (checkdigit (src_line.arg[i]) == TRUE)
						{
							if (! make_val (INT_VAR, varlist, NORMAL_VAR))
							{
								printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
								return (FALSE);
							}
							var = t_var.varlist_ind;
						}
					}

					if (var == -1)
					{
						/* it's a already defined var */

						var = getvarind_comp (src_line.arg[i]);
						if (var == NOTDEF)
						{
							printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[i], plist_ind);
							return (FALSE);
						}
					}
					
                    if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                    {
                        type = pvarlist_obj[var].type;
                    }
                    else
                    {
                        type = varlist[var].type;
                    }

                    switch (type)
                    {
                        case INT_VAR:
                            reg1 = get_vmreg_var_l (var);
                            if (reg1 == EMPTY)
                            {
                                reg1 = get_vmreg_l ();
                                if (reg1 != FULL)
                                {
                                    if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                    {
                                        if (! set2 (PPUSH_I, var, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg1, var, PRIVATE);
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_I, var, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg1, var, NORMAL);
                                    }
                                }
                            }

                            if (! set1 (STPUSH_L, reg1))
                            {
                                return (MEMORY);
                            }
                            break;

                        case LINT_VAR:
                            reg1 = get_vmreg_var_l (var);
                            if (reg1 == EMPTY)
                            {
                                reg1 = get_vmreg_l ();
                                if (reg1 != FULL)
                                {
                                    if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                    {
                                        if (! set2 (PPUSH_L, var, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg1, var, PRIVATE);
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_L, var, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg1, var, NORMAL);
                                    }
                                }
                            }

                            if (! set1 (STPUSH_L, reg1))
                            {
                                return (MEMORY);
                            }
                            break;

                        case QINT_VAR:
                            reg1 = get_vmreg_var_l (var);
                            if (reg1 == EMPTY)
                            {
                                reg1 = get_vmreg_l ();
                                if (reg1 != FULL)
                                {
                                    if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                    {
                                        if (! set2 (PPUSH_Q, var, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg1, var, PRIVATE);
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_Q, var, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg1, var, NORMAL);
                                    }
                                }
                            }

                            if (! set1 (STPUSH_L, reg1))
                            {
                                return (MEMORY);
                            }
                            break;

                        case DOUBLE_VAR:
                            reg1 = get_vmreg_var_d (var);
                            if (reg1 == EMPTY)
                            {
                                reg1 = get_vmreg_d ();
                                if (reg1 != FULL)
                                {
                                    if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                    {
                                        if (! set2 (PPUSH_D, var, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_d (reg1, var, PRIVATE);
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_D, var, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_d (reg1, var, NORMAL);
                                    }
                                }
                            }

                            if (! set1 (STPUSH_D, reg1))
                            {
                                return (MEMORY);
                            }
                            break;

                        case STRING_VAR:
                            reg1 = get_vmreg_var_s (var);
                            if (reg1 == EMPTY)
                            {
                                reg1 = get_vmreg_s ();
                                if (reg1 != FULL)
                                {
                                    if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                    {
                                        if (! set2 (PPUSH_S, var, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_s (reg1, var, PRIVATE);
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_S, var, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_s (reg1, var, NORMAL);
                                    }
                                }
                            }

                            if (! set1 (STPUSH_S, reg1))
                            {
                                return (MEMORY);
                            }
                            break;

                        case BYTE_VAR:
                            reg1 = get_vmreg_var_l (var);
                            if (reg1 == EMPTY)
                            {
                                reg1 = get_vmreg_l ();
                                if (reg1 != FULL)
                                {
                                    if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                    {
                                        if (! set2 (PPUSH_B, var, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg1, var, PRIVATE);
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_B, var, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg1, var, NORMAL);
                                    }
                                }
                            }

                            if (! set1 (STPUSH_L, reg1))
                            {
                                return (MEMORY);
                            }
                            break;
                    }
                    
                    if (func_call_reverse_args == TRUE)
					{
						if (i > 1)
						{
							i = i - 1;
						}
						else
						{
							ok = FALSE;
						}
					}
					else
					{
						if (i < src_line.args)
						{
							i = i + 1;
						}
						else
						{
							ok = FALSE;
						}
					}
                }
            }
            else
            {
                translate = translate_code ();
                if (translate == TRUE)
                {
                    break;
                }

                /* save all registers */
          
                if (! set0 (STPUSH_ALL_ALL))
                {
                    return (MEMORY);
                }
            }

            if (strcmp (src_line.arg[0], COMP_RETURN_SB) != 0)
            {
                /* not a return: set JSR opcode */
                if (src_line.arg[0][0] == '@')
                {
                    set_pull_all = FALSE;
                    getstr (src_line.arg[0], buf, MAXJUMPNAME, 1, strlen (src_line.arg[0]));
                }
                else
                {
					if (atomic == FALSE)
					{
						set_pull_all = TRUE;
					}
					else
					{
						set_pull_all = FALSE;
					}
					
					getstr (src_line.arg[0], buf, MAXJUMPNAME, 0, strlen (src_line.arg[0]));
                }
                if (strlen (buf) == 0)
                {
                    printerr (NOTDEF_LAB, plist_ind, ST_PRE, "");
                    return (FALSE);
                }

                reg1 = getjumpind (buf);
                if (reg1 == NOTDEF)
                {
                    /* LAB not defined, so we define it now.
                     * and set the 'pos' in jumplist when we reach
                     * the LAB definition.
                     */

                    if (jumplist_gonext () == FALSE)
                    {
                        return (FALSE);
                    }

                    strcpy (jumplist[jumplist_ind].lab, buf);
                    jumplist[jumplist_ind].pos = NOTDEF;
                    jumplist[jumplist_ind].islabel = FALSE;
                    reg1 = jumplist_ind;
                }
                if (! set1 (JSR, reg1))
                {
                    return (MEMORY);
                }

                if (set_pull_all)
                {
                    /* restore all registers */
                    if (! set0 (STPULL_ALL_ALL))
                    {
                        return (MEMORY);
                    }
                }
            }
            break;

        case COMP_BYTE:
        case COMP_INT:
        case COMP_LONG_INT:
        case COMP_QINT:
        case COMP_DOUBLE:
        case COMP_STRING:
        case COMP_DYNAMIC:
            #if DEBUG
                printf ("DEBUG: ASSIGN VAR:\n");
            #endif

            var2 = -1;

            if (src_line.args == 0)
            {
                printf ("compile: error: missing arguments in type definition, line %li\n", plist_ind);
                return (FALSE);
            }
            if (compile_set_init_var (1, &var, src_line.opcode_n) != TRUE)
            {
                return (MEMORY);
            }

            if (src_line.args == 3)
            {
                #if DEBUG
                    printf ("DEBUG: args = 3\n");
                #endif
                /* check if definition => get constant */

                if (strcmp (src_line.arg[2], EQUAL_SB) != 0)
                {
                    printf ("compile: error: missing = in type definition, line %li\n", plist_ind);
                    return (FALSE);
                }

                if (src_line.opcode_n == COMP_STRING)
                {
                    /* check if string => save string */

                    if (! checkstring (src_line.arg[3]))
                    {
                        printf ("compile: error: string type definition: no string, line %li\n", plist_ind);
                        return (FALSE);
                    }

                    if (! make_string ())
                    {
                        printf ("compile: error: can't create string, line %li\n", plist_ind);
                        return (FALSE);
                    }
                    var2 = t_var.varlist_ind;
                    type2 = t_var.type;
                }
                else
                {
                    /* number variable */
                    #if DEBUG
                        printf ("DEBUG: checkdigit arg3: '%s'\n", src_line.arg[3]);
                    #endif

                    if (checkdigit (src_line.arg[3]) == TRUE)
                    {
						 switch (src_line.opcode_n)
						 {
							 case COMP_BYTE:
								type_const = BYTE_VAR;
								break;
								
							 case COMP_INT:
								 type_const = INT_VAR;
								 break;
								 
							 case COMP_LONG_INT:
								 type_const = LINT_VAR;
								 break;
								 
							 case COMP_QINT:
								 type_const = QINT_VAR;
								 break;
								 
							 case COMP_DOUBLE:
								 type_const = DOUBLE_VAR;
								 break;
						 }
						
                         if (! make_val (type_const, varlist, NORMAL_VAR))
                         {
                             printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
                             return (FALSE);
                         }
                         var2 = t_var.varlist_ind;
                         type2 = t_var.type;
                    }
                }

                if (var2 == -1)
                {
                    /* it's a already defined var */

                    #if DEBUG
                        printf ("DEBUG: checking variable...\n");
                    #endif

                    var2 = getvarind_comp (src_line.arg[3]);
                    if (var2 == NOTDEF)
                    {
                        printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[3], plist_ind);
                        return (FALSE);
                    }

                    #if DEBUG
                        printf ("found var2: %li\n", var2);
                    #endif

                    if (src_line.arg[3][0] == COMP_PRIVATE_VAR_SB)
                    {
                        type2 = pvarlist_obj[var2].type;
                        private_variable = TRUE;
                    }
                    else
                    {
                        type2 = varlist[var2].type;
                    }
                }

                #if DEBUG
                    printf ("DEBUG: var2: %li\n", var2);
                #endif
                /* set assign code */

                switch (src_line.opcode_n)
                {
                    case COMP_BYTE:
                        /* get free L register */
                        
                        reg1 = get_vmreg_var_l (var2);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */
                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_B, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_B, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                        }

                        if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_B, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_B, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                        }
                        break;

                    case COMP_INT:
                        /* get free L register */
                        
                        reg1 = get_vmreg_var_l (var2);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */

                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_I, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_I, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                        }

                        if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_I, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_I, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                        }
                        break;

                   case COMP_LONG_INT:
                        /* get free L register */

                        reg1 = get_vmreg_var_l (var2);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */

                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_L, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_L, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                        }

                        if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_L, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_L, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                            
                        }
                        break;

                    case COMP_QINT:
                        /* get free L register */

                        reg1 = get_vmreg_var_l (var2);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */

                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_Q, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_Q, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                        }

                        if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_Q, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_Q, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                        }
                        break;

                    case COMP_DOUBLE:
                        /* get free D register */

                        reg1 = get_vmreg_var_d (var2);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_d ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */

                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_D, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_D, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                        }

                        if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_D, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_d (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_D, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_d (reg1, var, NORMAL);
                        }
                        break;

                    case COMP_STRING:
                        /* get free S register */

                        reg1 = get_vmreg_var_s (var2);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_s ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */

                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_S, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_S, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                        }

                        if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_S, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_s (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_S, reg1, var))
                            {
                               return (MEMORY);
                            }
                            set_vmreg_s (reg1, var, NORMAL);
                        }
                        break;
                }
            }
            break;

        case COMP_PRINT:
            print:
            if (src_line.args == 0)
            {
                printf ("compile: error: missing arguments in print, line %li\n", plist_ind);
                return (FALSE);
            }

            for (i = 1; i <= src_line.args; i++)
            {
                /* check if argument is string or number contstant */
                var = -1;
                if (checkstring (src_line.arg[i]))
                {
                    if (! make_string ())
                    {
                        printf ("compile: error: can't create string, line %li\n", plist_ind);
                        return (FALSE);
                    }
                    var = t_var.varlist_ind;
                }
                else
                {
                    /* number variable */

                    if (checkdigit (src_line.arg[i]) == TRUE)
                    {
                         if (! make_val (INT_VAR, varlist, NORMAL_VAR))
                         {
                             printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
                             return (FALSE);
                         }
                         var = t_var.varlist_ind;
                    }
                }

                if (var == -1)
                {
                    /* it's a already defined var */

                    var = getvarind_comp (src_line.arg[i]);
                    if (var == NOTDEF)
                    {
                        printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[i], plist_ind);
                        return (FALSE);
                    }
                }
                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                {
                    type = pvarlist_obj[var].type;
                }
                else
                {
                    type = varlist[var].type;
                }

                switch (type)
                {
                    case INT_VAR:
                         /* get free L register */
                        reg1 = get_vmreg_var_l (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_I, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_I, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }
                            }
                        }

                        if (! set1 (PRINT_L, reg1))
                        {
                            return (MEMORY);
                        }
                        break;

                    case LINT_VAR:
                        /* get free L register */
                        reg1 = get_vmreg_var_l (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_L, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_L, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }
                            }
                        }

                        if (! set1 (PRINT_L, reg1))
                        {
                            return (MEMORY);
                        }
                        break;

                    case QINT_VAR:
                        /* get free L register */
                        reg1 = get_vmreg_var_l (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_Q, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_Q, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }
                            }
                        }

                        if (! set1 (PRINT_L, reg1))
                        {
                            return (MEMORY);
                        }
                        break;

                    case DOUBLE_VAR:
                        /* get free L register */
                        reg1 = get_vmreg_var_d (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_d ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_D, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_d (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_D, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_d (reg1, var, NORMAL);
                                }
                            }
                        }

                        if (! set1 (PRINT_D, reg1))
                        {
                            return (MEMORY);
                        }
                        break;


                    case STRING_VAR:
                         /* get free L register */
                        reg1 = get_vmreg_var_s (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_s ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_S, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_s (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_S, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_s (reg1, var, NORMAL);
                                }
                            }
                        }

                        if (! set1 (PRINT_S, reg1))
                        {
                            return (MEMORY);
                        }
                        break;


                    case BYTE_VAR:
                         /* get free L register */
                        reg1 = get_vmreg_var_l (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_B, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_B, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }
                            }
                        }

                        if (! set1 (PRINT_L, reg1))
                        {
                            return (MEMORY);
                        }
                        break;
                }
            }
            break;

        case COMP_PRINT_NEWLINE:
            print_newline:
            if (src_line.args == 0)
            {
                printf ("compile: error: missing arguments in printn, line %li\n", plist_ind);
                return (FALSE);
            }

            for (i = 1; i <= src_line.args; i++)
            {
                /* check if argument is number contstant */
                var = -1;
                if (checkdigit (src_line.arg[i]) == TRUE)
                {
                     if (! make_val (INT_VAR, varlist, NORMAL_VAR ))
                     {
                         printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
                         return (FALSE);
                     }
                     var = t_var.varlist_ind;
                }

                if (var == -1)
                {
                    /* it's a already defined var */

                    var = getvarind_comp (src_line.arg[i]);
                    if (var == NOTDEF)
                    {
                        printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[i], plist_ind);
                        return (FALSE);
                    }
                }
                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                {
                    if (pvarlist_obj[var].type != INT_VAR && pvarlist_obj[var].type != LINT_VAR && pvarlist_obj[var].type != QINT_VAR)
                    {
                        printf ("compile: error: variable not a number: %s, line: %li\n", src_line.arg[i], plist_ind);
                        return (FALSE);
                    }
                    type = pvarlist_obj[var].type;
                }
                else
                {
                    if (varlist[var].type != INT_VAR && varlist[var].type != LINT_VAR && varlist[var].type != QINT_VAR)
                    {
                        printf ("compile: error: variable not a number: %s, line: %li\n", src_line.arg[i], plist_ind);
                        return (FALSE);
                    }
                    type = varlist[var].type;
                }

                switch (type)
                {
                    case INT_VAR:
                         /* get free L register */
                        reg1 = get_vmreg_var_l (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_I, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var2, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_I, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var2, NORMAL);
                                }
                            }
                        }

                        if (! set1 (PRINT_NEWLINE, reg1))
                        {
                            return (MEMORY);
                        }
                        break;

                    case LINT_VAR:
                        /* get free L register */
                        reg1 = get_vmreg_var_l (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_L, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var2, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_L, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var2, NORMAL);
                                }
                            }
                        }

                        if (! set1 (PRINT_NEWLINE, reg1))
                        {
                            return (MEMORY);
                        }
                        break;

                    case QINT_VAR:
                        /* get free L register */
                        reg1 = get_vmreg_var_l (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_Q, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var2, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_Q, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var2, NORMAL);
                                }
                            }
                        }

                        if (! set1 (PRINT_NEWLINE, reg1))
                        {
                            return (MEMORY);
                        }
                        break;

                    case BYTE_VAR:
                         /* get free L register */
                        reg1 = get_vmreg_var_l (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_B, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var2, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_B, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var2, NORMAL);
                                }
                            }
                        }

                        if (! set1 (PRINT_NEWLINE, reg1))
                        {
                            return (MEMORY);
                        }
                        break;
                }
            }
            break;

        case COMP_EXIT:
            if (src_line.args == 0)
            {
                printf ("compile: error: missing arguments in exit, line %li\n", plist_ind);
                return (FALSE);
            }

            /* check if argument is number contstant */
            var = -1;
            if (checkdigit (src_line.arg[1]) == TRUE)
            {
                 if (! make_val (INT_VAR, varlist, NORMAL_VAR ))
                 {
                     printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
                     return (FALSE);
                 }
                 var = t_var.varlist_ind;
            }

            if (var == -1)
            {
                /* it's a already defined var */

                var = getvarind_comp (src_line.arg[1]);
                if (var == NOTDEF)
                {
                    printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[1], plist_ind);
                    return (FALSE);
                }
            }
            if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
            {
                if (pvarlist_obj[var].type != INT_VAR && pvarlist_obj[var].type != LINT_VAR && pvarlist_obj[var].type != QINT_VAR)
                {
                    printf ("compile: error: variable not a number: %s, line: %li\n", src_line.arg[1], plist_ind);
                    return (FALSE);
                }
                type = pvarlist_obj[var].type;
            }
            else
            {
                if (varlist[var].type != INT_VAR && varlist[var].type != LINT_VAR && varlist[var].type != QINT_VAR)
                {
                    printf ("compile: error: variable not a number: %s, line: %li\n", src_line.arg[1], plist_ind);
                    return (FALSE);
                }
                type = varlist[var].type;
            }

            switch (type)
            {
                case INT_VAR:
                     /* get free L register */
                    reg1 = get_vmreg_var_l (var);
                    if (reg1 == EMPTY)
                    {
                        reg1 = get_vmreg_l ();
                        if (reg1 != FULL)
                        {
                            /* set assign code, using free register */
                            if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPUSH_I, var, reg1))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var2, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PUSH_I, var, reg1))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var2, NORMAL);
                            }
                        }
                    }

                    if (! set1 (PEXIT, reg1))
                    {
                        return (MEMORY);
                    }
                    break;

                case LINT_VAR:
                    /* get free L register */
                    reg1 = get_vmreg_var_l (var);
                    if (reg1 == EMPTY)
                    {
                        reg1 = get_vmreg_l ();
                        if (reg1 != FULL)
                        {
                            /* set assign code, using free register */
                            if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPUSH_L, var, reg1))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var2, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PUSH_L, var, reg1))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var2, NORMAL);
                            }
                        }
                    }

                    if (! set1 (PEXIT, reg1))
                    {
                        return (MEMORY);
                    }
                    break;

                case QINT_VAR:
                    /* get free L register */
                    reg1 = get_vmreg_var_l (var);
                    if (reg1 == EMPTY)
                    {
                        reg1 = get_vmreg_l ();
                        if (reg1 != FULL)
                        {
                            /* set assign code, using free register */
                            if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPUSH_Q, var, reg1))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var2, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PUSH_Q, var, reg1))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var2, NORMAL);
                            }
                        }
                    }

                    if (! set1 (PEXIT, reg1))
                    {
                        return (MEMORY);
                    }
                    break;

                case BYTE_VAR:
                    /* get free L register */
                    reg1 = get_vmreg_var_l (var);
                    if (reg1 == EMPTY)
                    {
                        reg1 = get_vmreg_l ();
                        if (reg1 != FULL)
                        {
                            /* set assign code, using free register */
                            if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPUSH_B, var, reg1))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var2, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PUSH_B, var, reg1))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var2, NORMAL);
                            }
                        }
                    }

                    if (! set1 (PEXIT, reg1))
                    {
                        return (MEMORY);
                    }
                    break;
            }
            break;

        case COMP_GET:
            get:
            get_no_stack_pull = FALSE;
            if (src_line.args > 0)
            {
                /* pull arguments from stack */
                i = src_line.args;      /* beginn from back of list */
                ok = TRUE;
                while (ok)
                {
					if (strcmp (src_line.arg[i], GET_NO_ST_PULL_SB) == 0)
					{
						get_no_stack_pull = TRUE;
						
						/* set i to next src line argument */
						
						if (i > 1)
						{
							i = i - 1;
						}
						else
						{
							ok = FALSE;
						}
					}
					
                    var = getvarind_comp (src_line.arg[i]);
                    if (var == NOTDEF)
                    {
                        printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[i], plist_ind);
                        return (FALSE);
                    }

                    if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                    {
                        type = pvarlist_obj[var].type;
                    }
                    else
                    {
                        type = varlist[var].type;
                    }

                    switch (type)
                    {
                        case INT_VAR:
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_L, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_I, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PULL_I, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    unset_vmreg_l (var);
                                }
                            }
                            break;

                        case LINT_VAR:
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_L, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_L, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PULL_L, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    unset_vmreg_l (var);
                                }
                            }
                            break;

                        case QINT_VAR:
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_L, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_Q, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PULL_Q, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    unset_vmreg_l (var);
                                }
                            }
                            break;

                        case DOUBLE_VAR:
                            reg1 = get_vmreg_d();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_D, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_D, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PULL_D, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    unset_vmreg_d (var);
                                }
                            }
                            break;

                        case STRING_VAR:
                            reg1 = get_vmreg_s ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_S, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_S, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PULL_S, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    unset_vmreg_s (var);
                                }
                            }
                            break;

                        case BYTE_VAR:
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_L, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_B, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PULL_B, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    unset_vmreg_l (var);
                                }
                            }
                            break;
                    }
                    if (i > 1)
                    {
                        i = i - 1;
                    }
                    else
                    {
                        ok = FALSE;
                    }
                }
                
            }
            
            if (get_no_stack_pull == FALSE)
			{
				/* restore all registers */
				if (! set0 (STPULL_ALL_S))
				{
					return (MEMORY);
				}
				if (! set0 (STPULL_ALL_D))
				{
					return (MEMORY);
				}
				if (! set0 (STPULL_ALL_L))
				{
					return (MEMORY);
				}
            }
            break;

        case COMP_TOKEN:
            #if DEBUG
                printf ("compile: COMP_TOKEN\n");
                printf ("src_line args: %li\n", src_line.args);
            #endif
            
			if (strcmp (src_line.arg[0], COMP_PULL_SB) == 0)
			{
				/* pull register into variable, if variable is stored in register */
				
				var = getvarind_comp (src_line.arg[1]);
				if (var == NOTDEF)
				{
					printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[0], plist_ind);
					return (FALSE);
				}
				
				if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
				{
					type = pvarlist_obj[var].type;
					private_variable = TRUE;
				}
				else
				{
					type = varlist[var].type;
					private_variable = FALSE;
				}
				
				switch (type)
				{
					case INT_VAR:
						reg1 = get_vmreg_var_l (var);
						if (reg1 != EMPTY)
						{
							if (private_variable == TRUE)
							{
								if (! set2 (PPULL_I, reg1, var))
								{
									return (MEMORY);
								}
							}
							else
							{
								if (! set2 (PULL_I, reg1, var))
								{
									return (MEMORY);
								}
							}
							unset_vmreg_l (var);
						}
						else
						{
							printf ("pull: warning variable %s not in register!\n", src_line.arg[1]);
						}
						break;
						
					case LINT_VAR:
						reg1 = get_vmreg_var_l (var);
						if (reg1 != EMPTY)
						{
							if (private_variable == TRUE)
							{
								if (! set2 (PPULL_L, reg1, var))
								{
									return (MEMORY);
								}
							}
							else
							{
								if (! set2 (PULL_L, reg1, var))
								{
									return (MEMORY);
								}
							}
							unset_vmreg_l (var);
						}
						else
						{
							printf ("pull: warning variable %s not in register!\n", src_line.arg[1]);
						}
						break;
						
					case QINT_VAR:
						reg1 = get_vmreg_var_l (var);
						if (reg1 != EMPTY)
						{
							if (private_variable == TRUE)
							{
								if (! set2 (PPULL_Q, reg1, var))
								{
									return (MEMORY);
								}
							}
							else
							{
								if (! set2 (PULL_Q, reg1, var))
								{
									return (MEMORY);
								}
							}
							unset_vmreg_l (var);
						}
						else
						{
							printf ("pull: warning variable %s not in register!\n", src_line.arg[1]);
						}
						break;
					
					case DOUBLE_VAR:
						reg1 = get_vmreg_var_d (var);
						if (reg1 != EMPTY)
						{
							if (private_variable == TRUE)
							{
								if (! set2 (PPULL_D, reg1, var))
								{
									return (MEMORY);
								}
							}
							else
							{
								if (! set2 (PULL_D, reg1, var))
								{
									return (MEMORY);
								}
							}
							unset_vmreg_d (var);
						}
						else
						{
							printf ("pull: warning variable %s not in register!\n", src_line.arg[1]);
						}
						break;
						
					case STRING_VAR:
						reg1 = get_vmreg_var_s (var);
						if (reg1 != EMPTY)
						{
							if (private_variable == TRUE)
							{
								if (! set2 (PPULL_S, reg1, var))
								{
									return (MEMORY);
								}
							}
							else
							{
								if (! set2 (PULL_S, reg1, var))
								{
									return (MEMORY);
								}
							}
							unset_vmreg_s (var);
						}
						else
						{
							printf ("pull: warning variable %s not in register!\n", src_line.arg[1]);
						}
						break;
						
					case BYTE_VAR:
						reg1 = get_vmreg_var_l (var);
						if (reg1 != EMPTY)
						{
							if (private_variable == TRUE)
							{
								if (! set2 (PPULL_B, reg1, var))
								{
									return (MEMORY);
								}
							}
							else
							{
								if (! set2 (PULL_B, reg1, var))
								{
									return (MEMORY);
								}
							}
							unset_vmreg_l (var);
						}
						else
						{
							printf ("pull: warning variable %s not in register!\n", src_line.arg[1]);
						}
						break;
				}
				return (TRUE);
			}
				
						
				
			/* parse expressons like: i = 1 or i = i + 1 or i = x + 1 * y... etc. */
            if (strcmp (src_line.arg[1], EQUAL_SB) != 0)
            {
                printf ("compile: error: missing = in expression %s, line: %li\n", src_line.arg[1], plist_ind);
				printf ("%s\n\n", plist[plist_ind].memptr);
                return (FALSE);
            }
            /* found = */

            var = getvarind_comp (src_line.arg[0]);
            if (var == NOTDEF)
            {
                printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[0], plist_ind);
                return (FALSE);
            }

            if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
            {
                type = pvarlist_obj[var].type;
                private_variable = TRUE;
            }
            else
            {
                type = varlist[var].type;
                private_variable = FALSE;
            }

            
            /* check if array list */
			
			if (src_line.arg[2][0] == AROPEN_SB)
			{
				#if DEBUG
					printf ("DEBUG: found list.\n");
				#endif
				
				/* found x = [ 1, 2, 3, 4 ] 0;  expression */
				
				/* get index offset */
				if (checkdigit (src_line.arg[src_line.args]) == TRUE && src_line.arg[src_line.args][0] != ARCLOSE_SB)
				{
					if (! make_val (INT_VAR, varlist, NORMAL_VAR))
					{
						printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
						return (FALSE);
					}
					var3 = t_var.varlist_ind;
					type3 = t_var.type;
				}
				else
				{
					printf ("compile: error index offset not defined: %s, line: %li\n", src_line.arg[0], plist_ind);
					return (FALSE);
				}
				
				/* do sane check... */
				if (type3 == DOUBLE_VAR || type3 == STRING_VAR)
				{
					printf ("compile: error index offset not integer number: %s, line: %li\n", src_line.arg[src_line.args], plist_ind);
					return (FALSE);
				}
				
				switch (type3)
				{
					case INT_VAR:
						reg3 = get_vmreg_var_l (var3);
						if (reg3 == EMPTY)
						{
							reg3 = get_vmreg_l ();
							if (reg3 != FULL)
							{
								/* set assign code, using free register */

								if (! set2 (PUSH_I, var3, reg3))
								{
									return (MEMORY);
								}
							}
						}
						set_vmreg_l (reg3, var3, NORMAL);
						break;
						
					case LINT_VAR:
						reg3 = get_vmreg_var_l (var3);
						if (reg3 == EMPTY)
						{
							reg3 = get_vmreg_l ();
							if (reg3 != FULL)
							{
								/* set assign code, using free register */

								if (! set2 (PUSH_L, var3, reg3))
								{
									return (MEMORY);
								}
							}
						}
						set_vmreg_l (reg3, var3, NORMAL);
						break;
						
					case QINT_VAR:
						reg3 = get_vmreg_var_l (var3);
						if (reg3 == EMPTY)
						{
							reg3 = get_vmreg_l ();
							if (reg3 != FULL)
							{
								/* set assign code, using free register */

								if (! set2 (PUSH_Q, var3, reg3))
								{
									return (MEMORY);
								}
							}
						}
						set_vmreg_l (reg3, var3, NORMAL);
						break;
						
					case BYTE_VAR:
						reg3 = get_vmreg_var_l (var3);
						if (reg3 == EMPTY)
						{
							reg3 = get_vmreg_l ();
							if (reg3 != FULL)
							{
								/* set assign code, using free register */

								if (! set2 (PUSH_B, var3, reg3))
								{
									return (MEMORY);
								}
							}
						}
						set_vmreg_l (reg3, var3, NORMAL);
						break;
				}
				
				for (list = 3; list <= src_line.args - 2; list++)
				{
					var2 = -1;
					if (! checkstring (src_line.arg[list]))
					{
						if (checkdigit (src_line.arg[list]) == TRUE)
						{
							if (! make_val (INT_VAR, varlist, NORMAL_VAR))
							{
								printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
								return (FALSE);
							}
							var2 = t_var.varlist_ind;
							type2 = t_var.type;
						}
					}
					else
					{
						if (! make_string ())
						{
							printf ("compile: error: can't create string, line %li\n", plist_ind);
							return (FALSE);
						}
						var2 = t_var.varlist_ind;
						type2 = t_var.type;
					}

					if (var2 == -1)
					{
						/* it's a already defined var */

						#if DEBUG
							printf ("DEBUG: checking variable...\n");
						#endif

						var2 = getvarind_comp (src_line.arg[list]);
						if (var2 == NOTDEF)
						{
							printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[list], plist_ind);
							return (FALSE);
						}

						#if DEBUG
							printf ("found var2: %li\n", var2);
						#endif

                  
						type2 = varlist[var2].type;
					}

					#if DEBUG
						printf ("DEBUG: var2: %li\n", var2);
					#endif
						
						
					/* do sane check... */
					switch (varlist[var].type)
					{
						case INT_VAR:
						case LINT_VAR:
						case QINT_VAR:
						case BYTE_VAR:
							if (type2 == STRING_VAR)
							{
								/* can't assign string to number var */
								printf ("compile: error: variable not of number type: %s, line: %li\n", src_line.arg[list], plist_ind);
								return (FALSE);
							}
							
							if (type2 == DOUBLE_VAR)
							{
								/* can't assign double to integer number var */
								printf ("compile: error: variable not of integer number type: %s, line: %li\n", src_line.arg[list], plist_ind);
								return (FALSE);
							}
							break;
							
						case DOUBLE_VAR:
							if (type2 != DOUBLE_VAR)
							{
								/* can't assign not double to double var */
								printf ("compile: error: variable not of double type: %s, line: %li\n", src_line.arg[list], plist_ind);
								return (FALSE);
							}
							break;
							
						case STRING_VAR:
							if (type2 != STRING_VAR)
							{
								/* can't assign string to not string var */
								printf ("compile: error: variable not of string type: %s, line: %li\n", src_line.arg[list], plist_ind);
								return (FALSE);
							}
							break;
					}
						
					switch (type2)
					{
						case INT_VAR:
							reg1 = get_vmreg_var_l (var2);
							if (reg1 == EMPTY)
							{
								reg1 = get_vmreg_l ();
								if (reg1 != FULL)
								{
									/* set assign code, using free register */

                                    if (! set2 (PUSH_I, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                            break;
							
						case LINT_VAR:
							reg1 = get_vmreg_var_l (var2);
							if (reg1 == EMPTY)
							{
								reg1 = get_vmreg_l ();
								if (reg1 != FULL)
								{
									/* set assign code, using free register */

                                    if (! set2 (PUSH_L, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                            break;
							
						case QINT_VAR:
							reg1 = get_vmreg_var_l (var2);
							if (reg1 == EMPTY)
							{
								reg1 = get_vmreg_l ();
								if (reg1 != FULL)
								{
									/* set assign code, using free register */

                                    if (! set2 (PUSH_Q, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                            break;
							
						case DOUBLE_VAR:
							reg1 = get_vmreg_var_d (var2);
							if (reg1 == EMPTY)
							{
								reg1 = get_vmreg_d ();
								if (reg1 != FULL)
								{
									/* set assign code, using free register */

                                    if (! set2 (PUSH_D, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                            break;
							
						case STRING_VAR:
							reg1 = get_vmreg_var_s (var2);
							if (reg1 == EMPTY)
							{
								reg1 = get_vmreg_s ();
								if (reg1 != FULL)
								{
									/* set assign code, using free register */

                                    if (! set2 (PUSH_S, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                            break;
							
						case BYTE_VAR:
							reg1 = get_vmreg_var_l (var2);
							if (reg1 == EMPTY)
							{
								reg1 = get_vmreg_l ();
								if (reg1 != FULL)
								{
									/* set assign code, using free register */

                                    if (! set2 (PUSH_B, var2, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                            }
                            break;
					}
					
					switch (type)
					{
						case INT_VAR:
							if (! set3 (LET2ARRAY_I, reg1, var, reg3))
                            {
                                 return (MEMORY);
                            }
                            break;
							
						case LINT_VAR:
							if (! set3 (LET2ARRAY_L, reg1, var, reg3))
                            {
                                 return (MEMORY);
                            }
                            break;
							
						case QINT_VAR:
							if (! set3 (LET2ARRAY_Q, reg1, var, reg3))
                            {
                                 return (MEMORY);
                            }
                            break;
							
						case DOUBLE_VAR:
							if (! set3 (LET2ARRAY_D, reg1, var, reg3))
                            {
                                 return (MEMORY);
                            }
                            break;
							
						case STRING_VAR:
							if (! set3 (LET2ARRAY_S, reg1, var, reg3))
                            {
                                 return (MEMORY);
                            }
                            break;
							
						case BYTE_VAR:
							if (! set3 (LET2ARRAY_B, reg1, var, reg3))
                            {
                                 return (MEMORY);
                            }
                            break;
							
						case DYNAMIC_VAR:
							switch (type2)
							{
								case QINT_VAR:
									if (! set3 ( MOVE_Q_ADYN, reg1, var, reg3))
									{
										return (MEMORY);
									}
									break;
									
								case DOUBLE_VAR:
									if (! set3 ( MOVE_D_ADYN, reg1, var, reg3))
									{
										return (MEMORY);
									}
									break;
									
								case STRING_VAR:
									if (! set3 ( MOVE_S_ADYN, reg1, var, reg3))
									{
										return (MEMORY);
									}
									break;
							}
							break;
					}
					
					/* increase index */
					if (! set1 (INC_L, reg3))
                    {
						return (MEMORY);
                    }
				}
				unset_vmreg_l (var3);
				return (TRUE);
			}
                            
                            
            /* check if ++ or -- */
            op_found = FALSE;
            if (strcmp (src_line.arg[2], COMP_OP_INC_SB) == 0)
            {
                opcode = INC_L;
                op_found = TRUE;
            }

            if (strcmp (src_line.arg[2], COMP_OP_DEC_SB) == 0)
            {
                opcode = DEC_L;
                op_found = TRUE;
            }

            if (op_found == TRUE)
            {
                switch (type)
                {
                    case INT_VAR:
                        /* get free L register */

                        reg1 = get_vmreg_var_l (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */

                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_I, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_I, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }
                            }
                        }
                        break;

                    case LINT_VAR:
                        /* get free L register */

                        reg1 = get_vmreg_var_l (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */

                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_L, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_L, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }
                            }
                        }
                        break;

                    case QINT_VAR:
                        /* get free L register */

                        reg1 = get_vmreg_var_l (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */

                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_Q, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_Q, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }
                            }
                        }
                        break;

                    case DOUBLE_VAR:
                        /* get free D register */

                        reg1 = get_vmreg_var_d (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_d ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */

                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_D, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_d (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_D, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_d (reg1, var, NORMAL);
                                }
                            }
                        }
                        break;

                    case BYTE_VAR:
                        /* get free L register */

                        reg1 = get_vmreg_var_l (var);
                        if (reg1 == EMPTY)
                        {
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                /* set assign code, using free register */

                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_B, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_B, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg1, var, NORMAL);
                                }
                            }
                        }
                        break;
                }

                if (! set1 (opcode, reg1))
                {
                    return (MEMORY);
                }

                switch (type)
                {
                    case INT_VAR:
                        if (private_variable == TRUE)
                        {
                            if (! set2 (PPULL_I, reg1, var))
                            {
                                return (MEMORY);
                            }
                        }
                        else
                        {
                            if (! set2 (PULL_I, reg1, var))
                            {
                                return (MEMORY);
                            }
                        }
                        break;

                    case LINT_VAR:
                        if (private_variable == TRUE)
                        {
                            if (! set2 (PPULL_L, reg1, var))
                            {
                                return (MEMORY);
                            }
                        }
                        else
                        {
                            if (! set2 (PULL_L, reg1, var))
                            {
                                return (MEMORY);
                            }
                        }
                        break;

                    case QINT_VAR:
                        if (private_variable == TRUE)
                        {
                            if (! set2 (PPULL_Q, reg1, var))
                            {
                                return (MEMORY);
                            }
                        }
                        else
                        {
                            if (! set2 (PULL_Q, reg1, var))
                            {
                                return (MEMORY);
                            }
                        }
                        break;

                    case DOUBLE_VAR:
                        if (private_variable == TRUE)
                        {
                            if (! set2 (PPULL_D, reg1, var))
                            {
                                return (MEMORY);
                            }
                        }
                        else
                        {
                            if (! set2 (PULL_D, reg1, var))
                            {
                                return (MEMORY);
                            }
                        }
                        break;

                    case BYTE_VAR:
                        if (private_variable == TRUE)
                        {
                            if (! set2 (PPULL_B, reg1, var))
                            {
                                return (MEMORY);
                            }
                        }
                        else
                        {
                            if (! set2 (PULL_B, reg1, var))
                            {
                                return (MEMORY);
                            }
                        }
                        break;
                }

                return (TRUE);
            }

            if (src_line.args == 3)
            {
                if (strcmp (src_line.arg[2], NOT_SB) == 0)
                {
                    /* not: i = ! x */
					
                    var2 = -1; private_variable = FALSE;
                    if (checkdigit (src_line.arg[3]) == TRUE)
                    {
                         if (! make_val (INT_VAR, varlist, NORMAL_VAR))
                         {
                             printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
                             return (FALSE);
                         }
                         var2 = t_var.varlist_ind;
                         type2 = t_var.type;
                    }

                    if (var2 == -1)
                    {
                        /* it's a already defined var */

                        var2 = getvarind_comp (src_line.arg[3]);
                        if (var == NOTDEF)
                        {
                            printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[3], plist_ind);
                            return (FALSE);
                        }

                        if (src_line.arg[3][0] == COMP_PRIVATE_VAR_SB)
                        {
                            type2 = pvarlist_obj[var2].type;
                            private_variable = TRUE;
                        }
                        else
                        {
                            type2 = varlist[var2].type;
                        }
                    }

                    /* set assign not code */

                    switch (type)
                    {
                        case INT_VAR:
                            /* get free L register */
                            
                            reg1 = get_vmreg_var_l (var2);
                            if (reg1 == EMPTY)
                            {
                                reg1 = get_vmreg_l ();
                                if (reg1 != FULL)
                                {
                                    /* set assign code, using free register */

                                    if (private_variable == TRUE)
                                    {
                                        if (! set2 (PPUSH_I, var2, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_I, var2, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                    }
                                }

                                reg2 = get_vmreg_l ();
                                if (reg2 == FULL)
                                {
                                    return (FALSE);
                                }
							}
                                
                            if (! set2 (NOT_L, reg1, reg2))
                            {
                                return (MEMORY);
                            }

                            if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPULL_I, reg2, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg2, var, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PULL_I, reg2, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg2, var, NORMAL); 
                            }
                            break;

                        case LINT_VAR:
                            /* get free L register */

                            reg1 = get_vmreg_var_l (var2);
                            if (reg1 == EMPTY)
                            {
                                reg1 = get_vmreg_l ();
                                if (reg1 != FULL)
                                {
                                    /* set assign code, using free register */

                                    if (private_variable == TRUE)
                                    {
                                        if (! set2 (PPUSH_L, var2, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_L, var2, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                    }
                                }

                                reg2 = get_vmreg_l ();
                                if (reg2 == FULL)
                                {
                                    return (FALSE);
                                }
							}
                            if (! set2 (NOT_L, reg1, reg2))
                            {
                                 return (MEMORY);
                            }

                            if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPULL_L, reg2, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg2, var, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PULL_L, reg2, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg2, var, NORMAL);
                            }
                            break;

                       case QINT_VAR:
                            /* get free L register */

                            reg1 = get_vmreg_var_l (var2);
                            if (reg1 == EMPTY)
                            {
                                reg1 = get_vmreg_l ();
                                if (reg1 != FULL)
                                {
                                    /* set assign code, using free register */

                                    if (private_variable == TRUE)
                                    {
                                        if (! set2 (PPUSH_Q, var2, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_Q, var2, reg1))
                                        {
                                            return (MEMORY);
                                        }
                                    }
                                }

                                reg2 = get_vmreg_l ();
                                if (reg2 == FULL)
                                {
                                    return (FALSE);
                                }
							}
                            if (! set2 (NOT_L, reg1, reg2))
                            {
                                return (MEMORY);
                            }

                            if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPULL_L, reg2, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg2, var, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PULL_L, reg2, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg2, var, NORMAL);
							}
                            break;
                    }
                    return (TRUE);
                }
            }

            if (src_line.args >= 2)
            {
                /* expression: i = 1 */

                var2 = -1; private_variable = FALSE;
                /* check if string => save string */

                #if DEBUG
                    printf ("src_line arg2: '%s', var2: %li\n", src_line.arg[2], var2);
                #endif

                if (checkstring (src_line.arg[2]))
                {
                    #if DEBUG
                        printf ("checkstring OK!\n");
                    #endif

                    if (! make_string ())
                    {
                        printf ("compile: error: can't create string, line %li\n", plist_ind);
                        return (FALSE);
                    }
                    var2 = t_var.varlist_ind;
                    type2 = t_var.type;
                }
                else
                {
                    /* number variable */

                    if (checkdigit (src_line.arg[2]) == TRUE)
                    {
                         #if DEBUG
                            printf ("checkdigit OK!\n");
                         #endif

                         if (! make_val (INT_VAR, varlist, NORMAL_VAR))
                         {
                             printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
                             return (FALSE);
                         }
                         var2 = t_var.varlist_ind;
                         type2 = t_var.type;
                    }
                }

                #if DEBUG
                    printf ("var2: %li\n", var2);
                #endif

				array_type = FALSE;
					
                if (var2 == -1)
                {
                    /* it's a already defined var */

                    var2 = getvarind_comp (src_line.arg[2]);
                    if (var2 == NOTDEF)
                    {
                        printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[2], plist_ind);
                        return (FALSE);
                    }

                    #if DEBUG
                        printf ("var2: %li\n", var2);
                    #endif

                    if (src_line.arg[2][0] == COMP_PRIVATE_VAR_SB)
                    {
                        type2 = pvarlist_obj[var2].type;
                        private_variable = TRUE;
						
						 if (pvarlist_obj[var2].dims != NODIMS)
						 {
							 if (type2 != STRING_VAR)
							 {
								array_type = TRUE;			/* except strings, they are always arrays! */
							 }
						 }
                    }
                    else
                    {
                        type2 = varlist[var2].type;
						
						 if (varlist[var2].dims != NODIMS)
						 {
							 if (type2 != STRING_VAR)
							 {
								array_type = TRUE;			/* except strings, they are always arrays! */
							 }
						 }
                    }
                }

                if (array_type == FALSE)
				{
					switch (type2)
					{
						case INT_VAR:
							reg1 = get_vmreg_var_l (var2);
							if (reg1 == EMPTY)
							{
								reg1 = get_vmreg_l ();
								if (reg1 != FULL)
								{
									/* set assign code, using free register */

									if (private_variable == TRUE)
									{
										if (! set2 (PPUSH_I, var2, reg1))
										{
											return (MEMORY);
										}
										set_vmreg_l (reg1, var2, PRIVATE);
									}
									else
									{
										if (! set2 (PUSH_I, var2, reg1))
										{
											return (MEMORY);
										}
										set_vmreg_l (reg1, var2, NORMAL);
									}
								}
							}
							break;

						case LINT_VAR:
							reg1 = get_vmreg_var_l (var2);
							if (reg1 == EMPTY)
							{
								reg1 = get_vmreg_l ();
								if (reg1 != FULL)
								{
									/* set assign code, using free register */

									if (private_variable == TRUE)
									{
										if (! set2 (PPUSH_L, var2, reg1))
										{
											return (MEMORY);
										}
										set_vmreg_l (reg1, var2, PRIVATE);
									}
									else
									{
										if (! set2 (PUSH_L, var2, reg1))
										{
											return (MEMORY);
										}
										set_vmreg_l (reg1, var2, NORMAL);
									}
								}
							}
							break;

						case QINT_VAR:
							reg1 = get_vmreg_var_l (var2);
							if (reg1 == EMPTY)
							{ 
								reg1 = get_vmreg_l ();
								if (reg1 != FULL)
								{
									/* set assign code, using free register */

									if (private_variable == TRUE)
									{
										if (! set2 (PPUSH_Q, var2, reg1))
										{
											return (MEMORY);
										}
										set_vmreg_l (reg1, var2, PRIVATE);
									}
									else
									{
										if (! set2 (PUSH_Q, var2, reg1))
										{
											return (MEMORY);
										}
										set_vmreg_l (reg1, var2, NORMAL);
									}
								}
							}
							break;

						case DOUBLE_VAR:
							reg1 = get_vmreg_var_d (var2);
							if (reg1 == EMPTY)
							{
								reg1 = get_vmreg_d ();
								if (reg1 != FULL)
								{
									/* set assign code, using free register */
    
									if (private_variable == TRUE)
									{
										if (! set2 (PPUSH_D, var2, reg1))
										{
											return (MEMORY);
										}
										set_vmreg_d (reg1, var2, PRIVATE);
									}
									else
									{
										if (! set2 (PUSH_D, var2, reg1))
										{
											return (MEMORY);
										}
										set_vmreg_d (reg1, var2, NORMAL);
									}
								}
							}
							break;

						case STRING_VAR:
							reg1 = get_vmreg_var_s (var2);
							if (reg1 == EMPTY)
							{
								reg1 = get_vmreg_s ();
								if (reg1 != FULL)
								{
									/* set assign code, using free register */

									if (private_variable == TRUE)
									{
										if (! set2 (PPUSH_S, var2, reg1))
										{
											return (MEMORY);
										}
										set_vmreg_s (reg1, var2, PRIVATE);
									}
									else
									{
										if (! set2 (PUSH_S, var2, reg1))
										{
											return (MEMORY);
										}   
										set_vmreg_s (reg1, var2, NORMAL);
									}
								}
							}
							break;

						case BYTE_VAR:
							reg1 = get_vmreg_var_l (var2);
							if (reg1 == EMPTY)
							{
								reg1 = get_vmreg_l ();
								if (reg1 != FULL)
								{
									/* set assign code, using free register */
	
									if (private_variable == TRUE)
									{
										if (! set2 (PPUSH_B, var2, reg1))
										{
											return (MEMORY);
										}
										set_vmreg_l (reg1, var2, PRIVATE);
									}
									else
									{
										if (! set2 (PUSH_B, var2, reg1))
										{
											return (MEMORY);
										}
										set_vmreg_l (reg1, var2, NORMAL);
									}
								}
							}
							break;
					}
				}
                /* set assign code */

                switch (type)
                {
                    case INT_VAR:
                        /* get free L register */

                        if (src_line.args == 2)
                        {
                            if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPULL_I, reg1, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PULL_I, reg1, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var, NORMAL);
                            }
                        }
                        break;

                    case LINT_VAR:
                        /* get free L register */

                        if (src_line.args == 2)
                        {
                            if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPULL_L, reg1, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PULL_L, reg1, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var, NORMAL);
                            }
                        }
                        break;

                    case QINT_VAR:
                        /* get free L register */

                        if (src_line.args == 2)
                        {
                            if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPULL_Q, reg1, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PULL_Q, reg1, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var, NORMAL);
                            }
                        }
                        break;

                    case DOUBLE_VAR:
                        /* get free D register */

                        if (src_line.args == 2)
                        {
                            if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPULL_D, reg1, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PULL_D, reg1, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var, NORMAL);
                            }
                        }
                        break;

                    case STRING_VAR:
                        /* get free S register */

                        if (src_line.args == 2)
                        {
                            if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPULL_S, reg1, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PULL_S, reg1, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var, NORMAL);
                            }
                        }
                        break;

                    case BYTE_VAR:
                        /* get free L register */

                        if (src_line.args == 2)
                        {
                            if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                            {
                                if (! set2 (PPULL_B, reg1, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var, PRIVATE);
                            }
                            else
                            {
                                if (! set2 (PULL_B, reg1, var))
                                {
                                    return (MEMORY);
                                }
                                set_vmreg_l (reg1, var, NORMAL);
                            }
                        }
                        break;

                }

            }
            if (src_line.args == 4)
            {
                /* expression: n = x + y */

                /* check if target variable is array */
				
				i = FALSE;
				if (private_variable == TRUE)
				{
					if (pvarlist_obj[var].dims != NODIMS && (pvarlist_obj[var].type == QINT_VAR || pvarlist_obj[var].type == DOUBLE_VAR))
					{
						i = TRUE; /* is array */
					}
				}
				else
				{
					if (varlist[var].dims != NODIMS && (varlist[var].type == QINT_VAR || varlist[var].type == DOUBLE_VAR))
					{
						i = TRUE; /* is array */
					}
				}
				
				if (i == TRUE)
				{
					/* left value is array */
					
					/* check 5th argument: if it's a variable or array */
					
					var3 = -1; private_variable = FALSE;
                
					/* number variable */
					array_type = FALSE;
					if (checkdigit (src_line.arg[4]) == TRUE)
                   {
						if (! make_val (INT_VAR, varlist, NORMAL_VAR))
                       {
                            printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
                            return (FALSE);
                       }
                       var3 = t_var.varlist_ind;
                       type3 = t_var.type;
					}

					if (var3 == -1)
					{
						/* it's a already defined var */

						var3 = getvarind_comp (src_line.arg[4]);
						if (var3 == NOTDEF)
						{
							printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[4], plist_ind);
							return (FALSE);
						}

						if (src_line.arg[4][0] == COMP_PRIVATE_VAR_SB)
						{
							type3 = pvarlist_obj[var3].type;
							private_variable = TRUE;
							
							if (pvarlist_obj[var3].dims != NODIMS)
							{
								if (pvarlist_obj[var3].type != QINT_VAR && pvarlist_obj[var3].type != DOUBLE_VAR)
								{
									printf ("compile: error: variable must be qint or double variable: %s, line: %li\n", src_line.arg[4], plist_ind);
									return (FALSE);
								}
								
								array_type = TRUE; /* is array */
							}
						}
						else
						{
							type3 = varlist[var3].type;
							
							if (varlist[var3].dims != NODIMS)
							{
								if (varlist[var3].type != QINT_VAR && varlist[var3].type != DOUBLE_VAR)
								{
									printf ("compile: error: variable must be qint or double variable: %s, line: %li\n", src_line.arg[4], plist_ind);
									return (FALSE);
								}
								
								array_type = TRUE; /* is array */
							}
						}
					}

					if (array_type == FALSE)
					{
						/* 5th argument is not array, push it */
						
						switch (type3)
						{
							case INT_VAR:
							/* get free L register */

								reg3 = get_vmreg_var_l (var3);
								if (reg3 == EMPTY)
								{
									reg3 = get_vmreg_l ();
									if (reg3 != FULL)
									{
										/* set opcode using free register */
										if (private_variable == TRUE)
										{
											if (! set2 (PPUSH_I, var3, reg3))
											{
												return (MEMORY);
											}
											set_vmreg_l (reg3, var3, PRIVATE);
										}
										else
										{
											if (! set2 (PUSH_I, var3, reg3))
											{
												return (MEMORY);
											}
											set_vmreg_l (reg3, var3, NORMAL);
										}
									}
								}
								break;

							case LINT_VAR:
								reg3 = get_vmreg_var_l (var3);
								if (reg3 == EMPTY)
								{
									reg3 = get_vmreg_l ();
									if (reg3 != FULL)
									{
										/* set opcode using free register */
										if (private_variable == TRUE)
										{
											if (! set2 (PPUSH_L, var3, reg3))
											{
												return (MEMORY);
											}
											set_vmreg_l (reg3, var3, PRIVATE);
										}
										else
										{
											if (! set2 (PUSH_L, var3, reg3))
											{
												return (MEMORY);
											}
											set_vmreg_l (reg3, var3, NORMAL);
										}
									}
								}
								break;

							case QINT_VAR:
								reg3 = get_vmreg_var_l (var3);
								if (reg3 == EMPTY)
								{
									reg3 = get_vmreg_l ();
									if (reg3 != FULL)
									{
										/* set opcode using free register */
										if (private_variable == TRUE)
										{
											if (! set2 (PPUSH_Q, var3, reg3))
											{
												return (MEMORY);
											}
											set_vmreg_l (reg3, var3, PRIVATE);
										}	
										else
										{
											if (! set2 (PUSH_Q, var3, reg3))
											{
												return (MEMORY);
											}
											set_vmreg_l (reg3, var3, NORMAL);
										}
									}
								}
								break;

							case DOUBLE_VAR:
								reg3 = get_vmreg_var_d (var3);
								if (reg3 == EMPTY)
								{
									reg3 = get_vmreg_d ();
									if (reg3 != FULL)
									{
										/* set opcode using free register */
										if (private_variable == TRUE)
										{
											if (! set2 (PPUSH_D, var3, reg3))
											{
												return (MEMORY);
											}
											set_vmreg_d (reg3, var3, PRIVATE);
										}
										else
										{
											if (! set2 (PUSH_D, var3, reg3))
											{
												return (MEMORY);
											}
											set_vmreg_d (reg3, var3, NORMAL);
										}
									}
								}
								break;

							case STRING_VAR:
								printf ("compile: error: variable type must be a number: %s, line %li\n", src_line.arg[4], plist_ind);
								return (FALSE);
								break;

							case BYTE_VAR:
								reg3 = get_vmreg_var_l (var3);
								if (reg3 == EMPTY)
								{
									reg3 = get_vmreg_l ();
									if (reg3 != FULL)
									{
										/* set opcode using free register */
										if (private_variable == TRUE)
										{
											if (! set2 (PPUSH_B, var3, reg3))
											{
												return (MEMORY);
											}
											set_vmreg_l (reg3, var3, PRIVATE);
										}
										else
										{
											if (! set2 (PUSH_B, var3, reg3))
											{
												return (MEMORY);
											}
											set_vmreg_l (reg3, var3, NORMAL);
										}
									}
								}
								break;
						}
					}
					else
					{
						reg3 = var3;
					}
					

					i = FALSE;
					var2 = getvarind_comp (src_line.arg[2]);
					if (var2 == NOTDEF)
					{
						printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[2], plist_ind);
						return (FALSE);
					}

					if (src_line.arg[2][0] == COMP_PRIVATE_VAR_SB)
					{
						type2 = pvarlist_obj[var2].type;
						private_variable = TRUE;
							
						if (pvarlist_obj[var2].dims != NODIMS)
						{
							if (pvarlist_obj[var2].type != QINT_VAR && pvarlist_obj[var2].type != DOUBLE_VAR)
							{
								printf ("compile: error: variable must be qint or double variable: %s, line: %li\n", src_line.arg[2], plist_ind);
								return (FALSE);
							}
								
							i = TRUE; /* is array */
						}
					}
					else
					{
						type2 = varlist[var2].type;
							
						if (varlist[var2].dims != NODIMS)
						{
							if (varlist[var2].type != QINT_VAR && varlist[var2].type != DOUBLE_VAR)
							{
								printf ("compile: error: variable must be qint or double variable: %s, line: %li\n", src_line.arg[2], plist_ind);
								return (FALSE);
							}
							
							i = TRUE; /* is array */
						}
					}
				
					if (i == TRUE)
					{
						op_found = FALSE; i = 3;

						if (strcmp (src_line.arg[i], COMP_OP_ADD_SB) == 0)
						{
							switch (type)
							{	
								case QINT_VAR:
									if (array_type)
									{
										opcode = VADD2_L;
									}
									else
									{
										opcode = VADD_L;
									}
									break;
									
								case DOUBLE_VAR:
									if (array_type)
									{
										opcode = VADD2_D;
									}
									else
									{
										opcode = VADD_D;
									}
									break;
							}
							op_found = TRUE;
						}
						
						if (strcmp (src_line.arg[i], COMP_OP_SUB_SB) == 0)
						{
							switch (type)
							{	
								case QINT_VAR:
									if (array_type)
									{
										opcode = VSUB2_L;
									}
									else
									{
										opcode = VSUB_L;
									}
									break;
									
								case DOUBLE_VAR:
									if (array_type)
									{
										opcode = VSUB2_D;
									}
									else
									{
										opcode = VSUB_D;
									}
									break;
							}
							op_found = TRUE;
						}
						
						if (strcmp (src_line.arg[i], COMP_OP_MUL_SB) == 0)
						{
							switch (type)
							{	
								case QINT_VAR:
									if (array_type)
									{
										opcode = VMUL2_L;
									}
									else
									{
										opcode = VMUL_L;
									}
									break;
									
								case DOUBLE_VAR:
									if (array_type)
									{
										opcode = VMUL2_D;
									}
									else
									{
										opcode = VMUL_D;
									}
									break;
							}
							op_found = TRUE;
						}
						
						if (strcmp (src_line.arg[i], COMP_OP_DIV_SB) == 0)
						{
							switch (type)
							{	
								case QINT_VAR:
									if (array_type)
									{
										opcode = VDIV2_L;
									}
									else
									{
										opcode = VDIV_L;
									}
									break;
									
								case DOUBLE_VAR:
									if (array_type)
									{
										opcode = VDIV2_D;
									}
									else
									{
										opcode = VDIV_D;
									}
									break;
							}
							op_found = TRUE;
						}
						
						if (op_found == FALSE)
						{
							printf ("compile: error: unknown operator: %s, line %li\n", src_line.arg[i], plist_ind);
							return (FALSE);
						}
						
						if (! set3 (opcode, var2, reg3, var))
                       {
                            return (MEMORY);
                       }
                       else
					   {
						   return (TRUE);
					   }
					}
				}
                       
				
				/* check normal variable expressions */
				
				op_found = FALSE; i = 3;

                if (strcmp (src_line.arg[i], COMP_OP_ADD_SB) == 0)
                {
                    switch (type)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = ADD_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = ADD_D;
                            break;

                        case STRING_VAR:
                            opcode = ADD_S;
                            break;

                        case BYTE_VAR:
                            opcode = ADD_L;
                            break;
                    }
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_SUB_SB) == 0)
                {
                    switch (type)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = SUB_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = SUB_D;
                            break;

                        case BYTE_VAR:
                            opcode = SUB_L;
                            break;
                    }
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_MUL_SB) == 0)
                {
                    switch (type)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = MUL_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = MUL_D;
                            break;

                        case BYTE_VAR:
                             opcode = MUL_L;
                             break;
                    }
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_DIV_SB) == 0)
                {
                    switch (type)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = DIV_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = DIV_D;
                            break;

                        case BYTE_VAR:
                            opcode = DIV_L;
                            break;
                    }
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_INC_SB) == 0)
                {
                    switch (type)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = INC_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = INC_D;
                            break;

                        case BYTE_VAR:
                            opcode = INC_L;
                            break;
                    }
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_DEC_SB) == 0)
                {
                    switch (type)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = DEC_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = DEC_D;
                            break;

                        case BYTE_VAR:
                            opcode = DEC_L;
                            break;
                    }
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_SMUL_SB) == 0)
                {
                    opcode = SMUL_L;
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_SDIV_SB) == 0)
                {
                    opcode = SDIV_L;
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_AND_SB) == 0)
                {
                    opcode = AND_L;
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_OR_SB) == 0)
                {
                    opcode = OR_L;
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_BAND_SB) == 0)
                {
                    opcode = BAND_L;
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_BOR_SB) == 0)
                {
                    opcode = BOR_L;
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_BXOR_SB) == 0)
                {
                    opcode = BXOR_L;
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_MOD_SB) == 0)
                {
                    switch (type)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = MOD_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = MOD_D;
                            break;

                        case BYTE_VAR:
                            opcode = MOD_L;
                            break;
                    }
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], EQUAL_LOG_SB) == 0)
                {
                    switch (type)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = EQUAL_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = EQUAL_D;
                            break;

                        case STRING_VAR:
                            opcode = EQUALSTR_L;
                            break;
                            
                        case BYTE_VAR:
                            opcode = EQUAL_L;
                            break;
                    }
                    op_found = TRUE;
					
                    var_i = getvarind_comp (src_line.arg[i - 1]);
                    if (var_i != NOTDEF)
                    {
                        if (src_line.arg[i - 1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            type_i = pvarlist_obj[var_i].type;
                        }
                        else
                        {
                            type_i = varlist[var_i].type;
                        }   
                       
                        if (type_i == STRING_VAR)
                        {
                            opcode = EQUALSTR_L;
                        }
                    }
                }

                if (strcmp (src_line.arg[i], NOT_EQUAL_SB) == 0)
                {
                    switch (type)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = NOT_EQUAL_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = NOT_EQUAL_D;
                            break;

                       case STRING_VAR:
                            opcode = NOT_EQUALSTR_L;
                            break; 
                            
                        case BYTE_VAR:
                            opcode = NOT_EQUAL_L;
                            break;
                    }
                    op_found = TRUE;
					
                    var_i = getvarind_comp (src_line.arg[i - 1]);
                    if (var_i != NOTDEF)
                    {
                        if (src_line.arg[i - 1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            type_i = pvarlist_obj[var_i].type;
                        }
                        else
                        {
                            type_i = varlist[var_i].type;
                        }   
                       
                        if (type_i == STRING_VAR)
                        {
                            opcode = NOT_EQUALSTR_L;
                        }
                    }
                }

                if (strcmp (src_line.arg[i], COMP_OP_GREATER_SB) == 0)
                {
                    switch (type2)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = GREATER_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = GREATER_D;
                            break;

                        case BYTE_VAR:
                            opcode = GREATER_L;
                            break;
                    }
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_LESS_SB) == 0)
                {
                    switch (type2)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = LESS_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = LESS_D;
                            break;

                        case BYTE_VAR:
                            opcode = LESS_L;
                            break;
                    }
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_GREATER_OR_EQ_SB) == 0)
                {
                    switch (type2)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = GREATER_OR_EQ_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = GREATER_OR_EQ_D;
                            break;

                        case BYTE_VAR:
                            opcode = GREATER_OR_EQ_L;
                            break;
                    }
                    op_found = TRUE;
                }

                if (strcmp (src_line.arg[i], COMP_OP_LESS_OR_EQ_SB) == 0)
                {
                    switch (type2)
                    {
                        case INT_VAR:
                        case LINT_VAR:
                        case QINT_VAR:
                            opcode = LESS_OR_EQ_L;
                            break;

                        case DOUBLE_VAR:
                            opcode = LESS_OR_EQ_D;
                            break;

                        case BYTE_VAR:
                            opcode = LESS_OR_EQ_L;
                            break;
                    }
                    op_found = TRUE;
                }

                if (op_found == FALSE)
                {
                    printf ("compile: error: unknown operator: %s, line %li\n", src_line.arg[i], plist_ind);
                    return (FALSE);
                }

                var3 = -1; private_variable = FALSE;
                /* check if string => save string */
                if (checkstring (src_line.arg[4]))
                {
                    if (! make_string ())
                    {
                        printf ("compile: error: can't create string, line %li\n", plist_ind);
                        return (FALSE);
                    }
                    var3 = t_var.varlist_ind;
                    type3 = t_var.type;
                }
                else
                {
                    /* number variable */

                    if (checkdigit (src_line.arg[4]) == TRUE)
                    {
                        if (! make_val (INT_VAR, varlist, NORMAL_VAR))
                        {
                            printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
                            return (FALSE);
                        }
                        var3 = t_var.varlist_ind;
                        type3 = t_var.type;
                    }
                }

                if (var3 == -1)
                {
                    /* it's a already defined var */

                    var3 = getvarind_comp (src_line.arg[4]);
                    if (var3 == NOTDEF)
                    {
                        printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[4], plist_ind);
                        return (FALSE);
                    }

                    if (src_line.arg[4][0] == COMP_PRIVATE_VAR_SB)
                    {
                        type3 = pvarlist_obj[var3].type;
                        private_variable = TRUE;
                    }
                    else
                    {
                        type3 = varlist[var3].type;
                    }
                }

                switch (type3)
                {
                    case INT_VAR:
                        /* get free L register */

                        reg3 = get_vmreg_var_l (var3);
                        if (reg3 == EMPTY)
                        {
                            reg3 = get_vmreg_l ();
                            if (reg3 != FULL)
                            {
                                /* set opcode using free register */
                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_I, var3, reg3))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg3, var3, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_I, var3, reg3))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg3, var3, NORMAL);
                                }
                            }
                        }
                        break;

                    case LINT_VAR:
                        reg3 = get_vmreg_var_l (var3);
                        if (reg3 == EMPTY)
                        {
                            reg3 = get_vmreg_l ();
                            if (reg3 != FULL)
                            {
                                /* set opcode using free register */
                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_L, var3, reg3))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg3, var3, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_L, var3, reg3))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg3, var3, NORMAL);
                                }
                            }
                        }
                        break;

                    case QINT_VAR:
                        reg3 = get_vmreg_var_l (var3);
                        if (reg3 == EMPTY)
                        {
                            reg3 = get_vmreg_l ();
                            if (reg3 != FULL)
                            {
                                /* set opcode using free register */
                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_Q, var3, reg3))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg3, var3, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_Q, var3, reg3))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg3, var3, NORMAL);
                                }
                            }
                        }
                        break;

                    case DOUBLE_VAR:
                        reg3 = get_vmreg_var_d (var3);
                        if (reg3 == EMPTY)
                        {
                            reg3 = get_vmreg_d ();
                            if (reg3 != FULL)
                            {
                                /* set opcode using free register */
                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_D, var3, reg3))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_d (reg3, var3, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_D, var3, reg3))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_d (reg3, var3, NORMAL);
                                }
                            }
                        }
                        break;

                    case STRING_VAR:
                        reg3 = get_vmreg_var_s (var3);
                        if (reg3 == EMPTY)
                        {
                            reg3 = get_vmreg_s ();
                            if (reg3 != FULL)
                            {
                                /* set opcode using free register */
                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_S, var3, reg3))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_s (reg3, var3, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_S, var3, reg3))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_s (reg3, var3, NORMAL);
                                }
                            }
                        }
                        break;

                    case BYTE_VAR:
                        reg3 = get_vmreg_var_l (var3);
                        if (reg3 == EMPTY)
                        {
                            reg3 = get_vmreg_l ();
                            if (reg3 != FULL)
                            {
                                /* set opcode using free register */
                                if (private_variable == TRUE)
                                {
                                    if (! set2 (PPUSH_B, var3, reg3))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg3, var3, PRIVATE);
                                }
                                else
                                {
                                    if (! set2 (PUSH_B, var3, reg3))
                                    {
                                        return (MEMORY);
                                    }
                                    set_vmreg_l (reg3, var3, NORMAL);
                                }
                            }
                        }
                        break;
                }

                switch (type)
                {
                    case INT_VAR:
                        /* set pull opcode */

                        reg2 = get_vmreg_l ();
                        if (reg2 != FULL)
                        {
                            if (! set3 (opcode, reg1, reg3, reg2))
                            {
                                return (MEMORY);
                            }
                        }

                        if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_I, reg2, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg2, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_I, reg2, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg2, var, NORMAL);
                        }
                        break;

                    case LINT_VAR:
                        reg2 = get_vmreg_l ();
                        if (reg2 != FULL)
                        {
                            if (! set3 (opcode, reg1, reg3, reg2))
                            {
                                return (MEMORY);
                            }
                        }

                        if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_L, reg2, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg2, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_L, reg2, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg2, var, NORMAL);
                        }
                        break;

                    case QINT_VAR:
                        reg2 = get_vmreg_l ();
                        if (reg2 != FULL)
                        {
                            if (! set3 (opcode, reg1, reg3, reg2))
                            {
                                return (MEMORY);
                            }
                        }

                        if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_Q, reg2, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg2, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_Q, reg2, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg2, var, NORMAL);
                        }
                        break;

                    case DOUBLE_VAR:
                        reg2 = get_vmreg_d ();
                        if (reg2 != FULL)
                        {
                            if (! set3 (opcode, reg1, reg3, reg2))
                            {
                                return (MEMORY);
                            }
                        }

                        if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_D, reg2, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_d (reg2, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_D, reg2, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_d (reg2, var, NORMAL);
                        }
                        break;

                    case STRING_VAR:
                        reg2 = get_vmreg_s ();
                        if (reg2 != FULL)
                        {
                            if (! set3 (opcode, reg1, reg3, reg2))
                            {
                                return (MEMORY);
                            }
                        }

                        if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_S, reg2, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_s (reg2, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_S, reg2, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_s (reg2, var, NORMAL);
                        }
                        break;

                    case BYTE_VAR:
                        reg2 = get_vmreg_l ();
                        if (reg2 != FULL)
                        {
                            if (! set3 (opcode, reg1, reg3, reg2))
                            {
                                return (MEMORY);
                            }
                        }

                        if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_B, reg2, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg2, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_B, reg2, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg2, var, NORMAL);
                        }
                        break;
                }
            }

            if (src_line.args > 4)
            {
                /* expression: "n = 2 * 6 + x */

                for (i = 3; i <= src_line.args; i += 2)
                {
                    op_found = FALSE;

                    if (strcmp (src_line.arg[i], COMP_OP_ADD_SB) == 0)
                    {
                        switch (type)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = ADD_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = ADD_D;
                                break;

                            case STRING_VAR:
                                opcode = ADD_S;
                                break;

                            case BYTE_VAR:
                                opcode = ADD_L;
                                break;
                        }
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_SUB_SB) == 0)
                    {
                        switch (type)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = SUB_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = SUB_D;
                                break;

                            case BYTE_VAR:
                                opcode = SUB_L;
                                break;
                        }
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_MUL_SB) == 0)
                    {
                        switch (type)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = MUL_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = MUL_D;
                                break;

                            case BYTE_VAR:
                                opcode = MUL_L;
                                break;
                        }
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_DIV_SB) == 0)
                    {
                        switch (type)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = DIV_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = DIV_D;
                                break;

                            case BYTE_VAR:
                                opcode = DIV_L;
                                break;
                        }
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_INC_SB) == 0)
                    {
                        switch (type)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = INC_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = INC_D;
                                break;

                            case BYTE_VAR:
                                opcode = INC_L;
                                break;
                        }
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_DEC_SB) == 0)
                    {
                        switch (type)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = DEC_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = DEC_D;
                                break;

                            case BYTE_VAR:
                                opcode = DEC_L;
                                break;
                        }
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_SMUL_SB) == 0)
                    {
                        opcode = SMUL_L;
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_SDIV_SB) == 0)
                    {
                        opcode = SDIV_L;
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_AND_SB) == 0)
                    {
                        opcode = AND_L;
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_OR_SB) == 0)
                    {
                        opcode = OR_L;
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_BAND_SB) == 0)
                    {
                        opcode = BAND_L;
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_BOR_SB) == 0)
                    {
                        opcode = BOR_L;
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_BXOR_SB) == 0)
                    {
                        opcode = BXOR_L;
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_MOD_SB) == 0)
                    {
                        switch (type)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = MOD_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = MOD_D;
                                break;

                            case BYTE_VAR:
                                opcode = MOD_L;
                                break;
                        }
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], EQUAL_LOG_SB) == 0)
                    {
                        switch (type2)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = EQUAL_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = EQUAL_D;
                                break;

                            case STRING_VAR:
                                opcode = EQUALSTR_L;
                                break;
                                
                            case BYTE_VAR:
                                opcode = EQUAL_L;
                                break;
                        }
                        op_found = TRUE;
                        
                        var_i = getvarind_comp (src_line.arg[i - 1]);
                        if (var_i != NOTDEF)
                        {
                            if (src_line.arg[i - 1][0] == COMP_PRIVATE_VAR_SB)
                            {
                                type_i = pvarlist_obj[var_i].type;
                            }
                            else
                            {
                                type_i = varlist[var_i].type;
                            }   
                        
                            if (type_i == STRING_VAR)
                            {
                                opcode = EQUALSTR_L;
                            }
                        }
                    }

                    if (strcmp (src_line.arg[i], NOT_EQUAL_SB) == 0)
                    {
                        switch (type2)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = NOT_EQUAL_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = NOT_EQUAL_D;
                                break;

                            case STRING_VAR:
                                opcode = NOT_EQUALSTR_L;
                                break;
                                
                            case BYTE_VAR:
                                opcode = NOT_EQUAL_L;
                                break;
                        }
                        op_found = TRUE;
                        
                        var_i = getvarind_comp (src_line.arg[i - 1]);
                        if (var_i != NOTDEF)
                        {
                            if (src_line.arg[i - 1][0] == COMP_PRIVATE_VAR_SB)
                            {
                                type_i = pvarlist_obj[var_i].type;
                            }
                            else
                            {
                                type_i = varlist[var_i].type;
                            }   
                        
                            if (type_i == STRING_VAR)
                            {
                                opcode = NOT_EQUALSTR_L;
                            }
                        }
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_GREATER_SB) == 0)
                    {
                        switch (type2)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = GREATER_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = GREATER_D;
                                break;

                            case BYTE_VAR:
                                opcode = GREATER_L;
                                break;
                        }
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_LESS_SB) == 0)
                    {
                        switch (type2)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = LESS_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = LESS_D;
                                break;

                            case BYTE_VAR:
                                opcode = LESS_L;
                                break;
                        }
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_GREATER_OR_EQ_SB) == 0)
                    {
                        switch (type)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = GREATER_OR_EQ_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = GREATER_OR_EQ_D;
                                break;

                            case BYTE_VAR:
                                opcode = GREATER_OR_EQ_L;
                                break;
                        }
                        op_found = TRUE;
                    }

                    if (strcmp (src_line.arg[i], COMP_OP_LESS_OR_EQ_SB) == 0)
                    {
                        switch (type)
                        {
                            case INT_VAR:
                            case LINT_VAR:
                            case QINT_VAR:
                                opcode = LESS_OR_EQ_L;
                                break;

                            case DOUBLE_VAR:
                                opcode = LESS_OR_EQ_D;
                                break;

                            case BYTE_VAR:
                                opcode = LESS_OR_EQ_L;
                                break;
                        }
                        op_found = TRUE;
                    }

                    if (op_found == FALSE)
                    {
                        printf ("compile: error: unknown operator: %s, line %li\n", src_line.arg[i], plist_ind);
                        return (FALSE);
                    }

                    var2 = -1; private_variable = FALSE;
                    /* check if string => save string */
                    if (checkstring (src_line.arg[i + 1]))
                    {
                        if (! make_string ())
                        {
                            printf ("compile: error: can't create string, line %li\n", plist_ind);
                            return (FALSE);
                        }
                        var2 = t_var.varlist_ind;
                        type2 = t_var.type;
                    }
                    else
                    {
                        /* number variable */

                        if (checkdigit (src_line.arg[i + 1]) == TRUE)
                        {
                            if (! make_val (INT_VAR, varlist, NORMAL_VAR))
                            {
                                printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
                                return (FALSE);
                            }
                            var2 = t_var.varlist_ind;
                            type2 = t_var.type;
                        }
                    }

                    if (var2 == -1)
                    {
                        /* it's a already defined var */

                        var2 = getvarind_comp (src_line.arg[i + 1]);
                        if (var2 == NOTDEF)
                        {
                            printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[i + 1], plist_ind);
                            return (FALSE);
                        }

                        if (src_line.arg[i + 1][0] == COMP_PRIVATE_VAR_SB)
                        {
                            type2 = pvarlist_obj[var2].type;
                            private_variable = TRUE;
                        }
                        else
                        {
                            type2 = varlist[var2].type;
                        }
                    }

                    switch (type2)
                    {
                        case INT_VAR:
                            /* get free L register */
                            
                            reg2 = get_vmreg_var_l (var2);
                            if (reg2 == EMPTY)
                            {
                                reg2 = get_vmreg_l ();
                                if (reg2 != FULL)
                                {
                                    /* set opcode using free register */

                                    if (private_variable == TRUE)
                                    {
                                        if (! set2 (PPUSH_I, var2, reg2))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg2, var2, PRIVATE);
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_I, var2, reg2))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg2, var2, NORMAL);
                                    }
                                }
                            }

                            if (! set3 (opcode, reg1, reg2, reg1))
                            {
                                return (MEMORY);
                            }
                            break;

                        case LINT_VAR:
                            reg2 = get_vmreg_var_l (var2);
                            if (reg2 == EMPTY)
                            {
                                reg2 = get_vmreg_l ();
                                if (reg2 != FULL)
                                {
                                    /* set opcode using free register */

                                    if (private_variable == TRUE)
                                    {
                                        if (! set2 (PPUSH_L, var2, reg2))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg2, var2, PRIVATE);
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_L, var2, reg2))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg2, var2, NORMAL);
                                    }
                                }
                            }

                            if (! set3 (opcode, reg1, reg2, reg1))
                            {
                                return (MEMORY);
                            }
                             
                            break;

                        case QINT_VAR:
                            reg2 = get_vmreg_var_l (var2);
                            if (reg2 == EMPTY)
                            {
                                reg2 = get_vmreg_l ();
                                if (reg2 != FULL)
                                {
                                    /* set opcode using free register */

                                    if (private_variable == TRUE)
                                    {
                                        if (! set2 (PPUSH_Q, var2, reg2))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg2, var2, PRIVATE); 
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_Q, var2, reg2))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg2, var2, NORMAL);
                                    }
                                }
                            }

                            if (! set3 (opcode, reg1, reg2, reg1))
                            {
                                return (MEMORY);
                            }
                            break;

                        case DOUBLE_VAR:
                            reg2 = get_vmreg_var_d (var2);
                            if (reg2 == EMPTY)
                            {
                                reg2 = get_vmreg_d ();
                                if (reg2 != FULL)
                                {
                                    /* set opcode using free register */

                                    if (private_variable == TRUE)
                                    {
                                        if (! set2 (PPUSH_D, var2, reg2))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_d (reg2, var2, PRIVATE);
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_D, var2, reg2))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_d (reg2, var2, NORMAL);
                                    }
                                }
                            }

                            if (! set3 (opcode, reg1, reg2, reg1))
                            {
                                return (MEMORY);
                            }
                            break;

                        case STRING_VAR:
                            reg2 = get_vmreg_var_s (var2);
                            if (reg2 == EMPTY)
                            {
                                reg2 = get_vmreg_s ();
                                if (reg2 != FULL)
                                {
                                    /* set opcode using free register */

                                    if (private_variable == TRUE)
                                    {
                                        if (! set2 (PPUSH_S, var2, reg2))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_s (reg2, var2, PRIVATE);
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_S, var2, reg2))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_s (reg2, var2, NORMAL);
                                    }
                                }
                            }

                            if (! set3 (opcode, reg1, reg2, reg1))
                            {
                                return (MEMORY);
                            }
                            break;

                        case BYTE_VAR:
                            reg2 = get_vmreg_var_l (var2);
                            if (reg2 == EMPTY)
                            {
                                reg2 = get_vmreg_l ();
                                if (reg2 != FULL)
                                {
                                    /* set opcode using free register */

                                    if (private_variable == TRUE)
                                    {
                                        if (! set2 (PPUSH_B, var2, reg2))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg2, var2, PRIVATE);
                                    }
                                    else
                                    {
                                        if (! set2 (PUSH_B, var2, reg2))
                                        {
                                            return (MEMORY);
                                        }
                                        set_vmreg_l (reg2, var2, NORMAL);
                                    }
                                }
                            }

                            if (! set3 (opcode, reg1, reg2, reg1))
                            {
                                return (MEMORY);
                            }
                            break;
                    }
                }

                switch (type)
                {
                    case INT_VAR:
                        /* set pull opocde */

                        if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_I, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_I, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                        }
                        break;

                    case LINT_VAR:
                        if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_L, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_L, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                        }
                        break;

                    case QINT_VAR:
                        if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_Q, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE); 
                        }
                        else
                        {
                            if (! set2 (PULL_Q, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                        }
                        break;

                    case DOUBLE_VAR:
                        if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_D, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_d (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_D, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_d (reg1, var, NORMAL);
                        }
                        break;

                    case STRING_VAR:
                        if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_S, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_s (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_S, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_s (reg1, var, NORMAL);
                        }
                        break;

                    case BYTE_VAR:
                        if (src_line.arg[0][0] == COMP_PRIVATE_VAR_SB)
                        {
                            if (! set2 (PPULL_B, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PULL_B, reg1, var))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                        }
                        break;
                }
            }
            break;

        case COMP_IF:
            if (nested_code_global_off == FALSE) nested_code = TRUE;
            if (src_line.args != 1)
            {
                 printf ("compile: error: if expression too complex: line %li\n", plist_ind);
                 return (FALSE);
            }

            var = getvarind_comp (src_line.arg[1]);
            if (var == NOTDEF)
            {
                printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[1], plist_ind);
                return (FALSE);
            }

            if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
            {
                type = pvarlist_obj[var].type;
                private_variable = TRUE;
            }
            else
            {
                type = varlist[var].type;
                private_variable = FALSE;
            }

            switch (type)
            {
                /* get free L register */
                case INT_VAR:
                    reg1 = get_vmreg_l ();
                    if (reg1 != FULL)
                    {
                        /* set opcode using free register */

                        if (private_variable == TRUE)
                        {
                            if (! set2 (PPUSH_I, var, reg1))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PUSH_I, var, reg1))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                        }
                    }
                    break;

                case LINT_VAR:
                    reg1 = get_vmreg_l ();
                    if (reg1 != FULL)
                    {
                        /* set opcode using free register */

                        if (private_variable == TRUE)
                        {
                            if (! set2 (PPUSH_L, var, reg1))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PUSH_L, var, reg1))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                        }
                    }
                    break;

                case QINT_VAR:
                    reg1 = get_vmreg_l ();
                    if (reg1 != FULL)
                    {
                        /* set opcode using free register */

                        if (private_variable == TRUE)
                        {
                            if (! set2 (PPUSH_Q, var, reg1))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PUSH_Q, var, reg1))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                        }
                    }
                    break;

                default:
                    printf ("compile: error: not a int or lint or qint type at if: line: %li\n", plist_ind);
                    return (FALSE);
                    break;
            }

            reg2 = get_vmreg_l ();
            if (reg1 != FULL)
            {
                /* set NOT opcode (for if) */

                if (! set2 (NOT_L, reg1, reg2))
                {
                    return (MEMORY);
                }

                if_pos = get_if_pos ();
                if (if_pos == -1)
                {
                    printf ("compile: error: if: out of memory if-list line: %li\n", plist_ind);
                    return (FALSE);
                }

                set_else_jmp (if_pos);

                if (! set2 (JMP_L, reg2, get_else_lab (if_pos)))
                {
                    return (MEMORY);
                }
            }
            break;

        case COMP_ELSE:
            if_pos = get_act_if ();

            set_endif_jmp (if_pos);

            if (! set1 (JMP, get_endif_lab (if_pos)))
            {
                return (MEMORY);
            }

            if (! set_else_label (if_pos))
            {
                printf ("compile: error: else: out of memory line: %li\n", plist_ind);
                return (FALSE);
            }
            break;

        case COMP_ENDIF:
            if_pos = get_act_if ();

            if (get_else_set (if_pos) == NOTDEF)
            {
                /* no else defined: set else label */

                if (! set_else_label (if_pos))
                {
                    printf ("compile: error: endif: out of memory line: %li\n", plist_ind);
                    return (FALSE);
                }
                set_endif_finished (if_pos); /* new  ======================================================== */
            }
            else
            {
                if (! set_endif_label (if_pos))
                {
                    printf ("compile: error: endif: out of memory line: %li\n", plist_ind);
                    return (FALSE);
                }
            }
            break;

        case COMP_FOR:
            if (nested_code_global_off == FALSE) nested_code = TRUE;
            for_pos = get_for_pos ();
            if (for_pos == -1)
            {
                printf ("compile: error: for: out of memory for-list line: %li\n", plist_ind);
                return (FALSE);
            }

            if (! set_for_label (for_pos)) /* if_pos */
            {
                printf ("compile: error: else: out of memory line: %li\n", plist_ind);
                return (FALSE);
            }
            break;

        case COMP_NEXT:
            if (src_line.args != 1)
            {
                 printf ("compile: error: next expression too complex: line %li\n", plist_ind);
                 return (FALSE);
            }

            for_pos = get_act_for ();

            var = getvarind_comp (src_line.arg[1]);
            if (var == NOTDEF)
            {
                printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[1], plist_ind);
                return (FALSE);
            }

            set_next (for_pos);

            if (src_line.arg[1][0] == COMP_PRIVATE_VAR_SB)
            {
                type = pvarlist_obj[var].type;
                private_variable = TRUE;
            }
            else
            {
                type = varlist[var].type;
                private_variable = FALSE;
            }

            switch (type)
            {
                /* get free L register */
                case INT_VAR:
                    reg1 = get_vmreg_l ();
                    if (reg1 != FULL)
                    {
                        /* set opcode using free register */

                        if (private_variable == TRUE)
                        {
                            if (! set2 (PPUSH_I, var, reg1))
                            {
                                return (MEMORY);
                            }
                            
                        }
                        else
                        {
                            if (! set2 (PUSH_I, var, reg1))
                            {
                                return (MEMORY);
                            }
                        }
                    }
                    break;

                case LINT_VAR:
                    reg1 = get_vmreg_l ();
                    if (reg1 != FULL)
                    {
                        /* set opcode using free register */

                        if (private_variable == TRUE)
                        {
                            if (! set2 (PPUSH_L, var, reg1))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PUSH_L, var, reg1))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                        }
                    }
                    break;

                case QINT_VAR:
                    reg1 = get_vmreg_l ();
                    if (reg1 != FULL)
                    {
                        /* set opcode using free register */

                        if (private_variable == TRUE)
                        {
                            if (! set2 (PPUSH_Q, var, reg1))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, PRIVATE);
                        }
                        else
                        {
                            if (! set2 (PUSH_Q, var, reg1))
                            {
                                return (MEMORY);
                            }
                            set_vmreg_l (reg1, var, NORMAL);
                        }
                    }
                    break;

                default:
                    printf ("compile: error: not a int or lint or qint type at next: line: %li\n", plist_ind);
                    return (FALSE);
                    break;
            }

            if (! set2 (JMP_L, reg1, get_for_lab (for_pos)))
            {
                return (MEMORY);
            }
            break;

        case COMP_LAB:
            if (! compile_set_label (src_line.arg[1])) return (FALSE); /* set function name as label = PSEUDO opcode for emitter */
            if (nested_code_global_off == FALSE) nested_code = TRUE;
            break;

        case COMP_GOTO:
            if (nested_code_global_off == FALSE) nested_code = TRUE;         /* goto used, set flag (opcode.c) */
            reg1 = getjumpind (src_line.arg[1]);
            if (reg1 == NOTDEF)
            {
                /* LAB not defined, so we define it now.
                 * and set the 'pos' in jumplist when we reach
                 * the LAB definition.
                 */

                if (jumplist_gonext () == FALSE)
                {
                    return (FALSE);
                }

                strcpy (jumplist[jumplist_ind].lab, src_line.arg[1]);
                jumplist[jumplist_ind].pos = NOTDEF;
                jumplist[jumplist_ind].islabel = FALSE;
                reg1 = jumplist_ind;
            }
            if (! set1 (JMP, reg1))
            {
                return (MEMORY);
            }
            break;

        case COMP_GET_MULTI:
            getmulti:
            if (src_line.args > 0)
            {
                /* pull arguments from stack */
                i = src_line.args;      /* beginn from back of list */
                ok = TRUE;
                while (ok)
                {
                    var = getvarind_comp (src_line.arg[i]);
                    if (var == NOTDEF)
                    {
                        printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[i], plist_ind);
                        return (FALSE);
                    }

                    if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                    {
                        type = pvarlist_obj[var].type;
                    }
                    else
                    {
                        type = varlist[var].type;
                    }

                    switch (type)
                    {
                        case INT_VAR:
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_L, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_I, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PULL_I, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    unset_vmreg_l (var);
                                }
                            }
                            break;

                        case LINT_VAR:
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_L, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_L, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PULL_L, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    unset_vmreg_l (var);
                                }
                            }
                            break;

                        case QINT_VAR:
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_L, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_Q, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PULL_Q, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    unset_vmreg_l (var);
                                }
                            }
                            break;

                        case DOUBLE_VAR:
                            reg1 = get_vmreg_d();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_D, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_D, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PULL_D, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    unset_vmreg_d (var);
                                }
                            }
                            break;

                        case STRING_VAR:
                            reg1 = get_vmreg_s ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_S, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_S, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PULL_S, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    unset_vmreg_s (var);
                                }
                            }
                            break;

                        case BYTE_VAR:
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (! set1 (STPULL_L, reg1))
                                {
                                    return (MEMORY);
                                }

                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPULL_B, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PULL_B, reg1, var))
                                    {
                                        return (MEMORY);
                                    }
                                    unset_vmreg_l (var);
                                }
                            }
                            break;
                    }
                    if (i > 1)
                    {
                        i = i - 1;
                    }
                    else
                    {
                        ok = FALSE;
                    }
                }
            }
            break;
        
        case COMP_RETURN_MULTI:
            returnmulti:
            if (src_line.args > 0)
            {
                i = 1;
                ok = TRUE;
                while (ok)
                {
                    var = getvarind_comp (src_line.arg[i]);
                    if (var == NOTDEF)
                    {
                        printf ("compile: error: variable not defined: %s, line: %li\n", src_line.arg[i], plist_ind);
                        return (FALSE);
                    }

                    if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                    {
                        type = pvarlist_obj[var].type;
                    }
                    else
                    {
                        type = varlist[var].type;
                    }

                    switch (type)
                    {
                        case INT_VAR:
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_I, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_I, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                
                                if (! set1 (STPUSH_L, reg1))
                                {
                                    return (MEMORY);
                                }
                            }
                            break;
                            
                        case LINT_VAR:
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_L, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_L, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                
                                if (! set1 (STPUSH_L, reg1))
                                {
                                    return (MEMORY);
                                }
                            }
                            break;
                            
                        case QINT_VAR:
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_Q, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_Q, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                
                                if (! set1 (STPUSH_L, reg1))
                                {
                                    return (MEMORY);
                                }
                            }
                            break;
                            
                        case DOUBLE_VAR:
                            reg1 = get_vmreg_d ();
                            if (reg1 != FULL)
                            {
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_D, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_D, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                
                                if (! set1 (STPUSH_D, reg1))
                                {
                                    return (MEMORY);
                                }
                            }
                            break;
            
                        case STRING_VAR:
                            reg1 = get_vmreg_s ();
                            if (reg1 != FULL)
                            {
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_S, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_S, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                
                                if (! set1 (STPUSH_S, reg1))
                                {
                                    return (MEMORY);
                                }
                            }
                            break;
            
                        case BYTE_VAR:
                            reg1 = get_vmreg_l ();
                            if (reg1 != FULL)
                            {
                                if (src_line.arg[i][0] == COMP_PRIVATE_VAR_SB)
                                {
                                    if (! set2 (PPUSH_B, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                else
                                {
                                    if (! set2 (PUSH_B, var, reg1))
                                    {
                                        return (MEMORY);
                                    }
                                }
                                
                                if (! set1 (STPUSH_L, reg1))
                                {
                                    return (MEMORY);
                                }
                            }
                            break;
                    }
                    if (i < src_line.args)
                    {
                        i = i + 1;
                    }
                    else
                    {
                        ok = FALSE;
                    }
                }
            }
            break;
            
        case COMP_GET_MULTI_END:
            getmultiend:
            /* restore all registers */
            if (! set0 (STPULL_ALL_S))
            {
                return (MEMORY);
            }
            if (! set0 (STPULL_ALL_D))
            {
                return (MEMORY);
            }
            if (! set0 (STPULL_ALL_L))
            {
               return (MEMORY);
            }
            break;

        case ASSEMB_INLINE:
            if (! set0 (ASSEMB_INLINE))
            {
                return (MEMORY);
            }
            break;
            
        case COMP_NESTED_CODE_OFF:
            nested_code = FALSE;
            
            printf ("compile: NESTED CODE OFF!\n");
            break;
            
        case COMP_NESTED_CODE_ON:
            nested_code = TRUE;
            
            printf ("compile: NESTED CODE ON!\n");
            break;
			
		case COMP_NESTED_CODE_GLOBAL_OFF:
			nested_code_global_off = TRUE;
			nested_code = FALSE;
			
			printf ("compile: NESTED CODE GLOBAL OFF!\n");
            break;
			
		case COMP_ATOMIC_START:
			atomic = TRUE;
			
			printf ("compile: ATOMIC START!\n");
			break;
			
		case COMP_ATOMIC_END:
			atomic = FALSE;
			
			if (! set0 (STPULL_ALL_ALL))
            {
                return (MEMORY);
            }
			printf ("compile: ATOMIC END!\n");
			break;
    }
    return (TRUE);
}

