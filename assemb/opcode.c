/****************************************************************************
*
* Filename: opcode.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  opcode parsers
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

extern struct varlist *varlist;
extern struct varlist *pvarlist_obj;                /* thread private varlist */
extern struct varlist_state varlist_state;
extern struct varlist_state pvarlist_state;
extern struct src_line src_line;
extern struct jumplist *jumplist;
extern struct opcode opcode[];
extern struct arg arg;
extern struct t_var t_var;
extern struct vmreg_name vmreg_name;
extern struct assemb_set assemb_set;

extern S4 varlist_ind;
extern S4 plist_ind;
extern S4 cclist_ind;
extern S4 jumplist_ind;

extern U1 print_debug;

void get_opcode (U1 *str)
{
    S2 line_len, start, i, pos;
    U1 ok, comma[2];

    comma[0] = assemb_set.separ;
    comma[1] = BINUL;

    line_len = strlen (str) - 1;
    getstr (str, src_line.opcode, MAXLINELEN, 0, line_len);

    #if DEBUG
    if (print_debug)
    {
        printf ("get_opcode: opcode: '%s'\n", src_line.opcode);
    }
    #endif

    /* read arguments */
    start = strlen (src_line.opcode) + 1;
    src_line.args = 0;

    if (start <= line_len)
    {
        ok = FALSE; i = 0;
        while (! ok)
        {
            pos = searchstr (str, comma, start, line_len, TRUE);
            if (pos != -1)
            {
                getstr (str, src_line.arg[i], MAXLINELEN, start, pos - 1);
                start = pos + 1;
                src_line.args++;
            }
            else
            {
                getstr (str, src_line.arg[i], MAXLINELEN, start, line_len);
                src_line.args++;
                ok = TRUE;
            }

            #if DEBUG
            if (print_debug)
            {
                printf ("get_opcode: arg[%i] '%s'\n", i, src_line.arg[i]);
            }
            #endif

            if (i < MAXCOMMARG - 1)
            {
                i++;
            }
            else
            {
                ok = TRUE;
            }
        }
    }
}

U1 set_opcode (void)
{
    /* return MEMORY, if set fails
     * to break assembling immediatly
     */


    S2 args2_len;
    S4 i = 0, op = -1, vind[3], var;
    U1 args2[MAXLINELEN + 1];
    U1 ok = FALSE, create_variable = FALSE, create_variable_private = FALSE, private_variable = FALSE;

    /* search opcode */
    while (! ok)
    {
        if (strcmp (src_line.opcode, opcode[i].op) == 0)
        {
            #if DEBUG
            if (print_debug)
            {
                printf ("set_opcode: opcode: '%s'\n", opcode[i].op);
            }
            #endif

            op = i;
            ok = TRUE;
        }

        if (i < MAXOPCODE)
        {
            i++;
        }
        else
        {
            ok = TRUE;
        }
    }

    if (op == -1)
    {
        printerr (UNKN_OPCODE, plist_ind, ST_PRE, src_line.opcode);
        return (FALSE);
    }

    if (src_line.args != opcode[op].args)
    {
        /* wrong number of arguments */

        printerr (SYNTAX, plist_ind, ST_PRE, src_line.opcode);
        return (FALSE);
    }

    switch (op)
    {
        case VM_INT:
        case VM_LINT:
        case VM_QINT:
        case VM_DOUBLE:
        case VM_STRING:
        case VM_BYTE:
        case VM_DYNAMIC:
            if (! getvarstr (src_line.arg[0], 0))
            {
                return (FALSE);
            }

            #if DEBUG
                if (print_debug)
                {
                    printf ("set_opcode: t_var.varname: '%s'\n", t_var.varname);
                }
            #endif

            if (t_var.varname[0] == PRIVATE_VAR_SB)
            {
                private_variable = TRUE;

                var = getvarind (t_var.varname);
                if (var == NOTDEF) create_variable_private = TRUE;
            }
            else
            {
                var = getvarind (t_var.varname);
                if (var == NOTDEF) create_variable = TRUE;
            }

            if (create_variable == TRUE || create_variable_private == TRUE)
            {
                /* set new variable */

                switch (op)
                {
                    case VM_INT:
                        t_var.type = INT_VAR;
                        break;

                    case VM_LINT:
                        t_var.type = LINT_VAR;
                        break;

                    case VM_QINT:
                        t_var.type = QINT_VAR;
                        break;

                    case VM_DOUBLE:
                        t_var.type = DOUBLE_VAR;
                        break;

                    case VM_STRING:
                        t_var.type = STRING_VAR;
                        break;

                    case VM_BYTE:
                        t_var.type = BYTE_VAR;
                        break;

                    case VM_DYNAMIC:
                        t_var.type = DYNAMIC_VAR;
                        break;
                }

                if (create_variable == TRUE)
                {
                    if (! init_var (varlist, &varlist_state))
                    {
                        return (FALSE);
                    }
                    var = varlist_state.varlist_ind;
                }
                if (create_variable_private == TRUE)
                {
                    if (! init_var (pvarlist_obj, &pvarlist_state))
                    {
                        return (FALSE);
                    }
                    var = pvarlist_state.varlist_ind;
                }
            }

            if (private_variable == FALSE)
            {
                if (! set1 (ALLOC, var))
                {
                    return (MEMORY);
                }
            }
            else
            {
                if (! set1 (ALLOC_PRIVATE, var))
                {
                    return (MEMORY);
                }
            }
            break;

        case POINTER:
            getstr (src_line.arg[2], args2, MAXJUMPNAME, 0, strlen (src_line.arg[2]));
            args2_len = strlen (args2);

            if (args2_len == 0)
            {
                printerr (NOTDEF_LAB, plist_ind, ST_PRE, "");
                return (FALSE);
            }

            vind[2] = getjumpind (args2);
            if (vind[2] == NOTDEF)
            {
                /* LAB not defined, so we define it now.
                * and set the 'pos' in jumplist when we reach
                * the LAB definition.
                */

                if (jumplist_gonext () == FALSE)
                {
                    return (FALSE);
                }

                strcpy (jumplist[jumplist_ind].lab, args2);
                jumplist[jumplist_ind].pos = NOTDEF;
                jumplist[jumplist_ind].islabel = FALSE;
                vind[2] = jumplist_ind;
            }

            if (! get_arg (src_line.arg[0]))
            {
                return (FALSE);
            }

            if (arg.type != L_REG)
            {
                printerr (WRONG_VARTYPE, plist_ind, ST_PRE, src_line.arg[0]);
                return (FALSE);
            }
            vind[0] = arg.reg;

            if (! get_arg (src_line.arg[1]))
            {
                return (FALSE);
            }

            if (arg.type == L_REG || arg.type == D_REG)
            {
                printerr (WRONG_VARTYPE, plist_ind, ST_PRE, src_line.arg[1]);
                return (FALSE);
            }
            vind[1] = arg.reg;

            if (! set3 (op, vind[0], vind[1], vind[2]))
            {
                return (MEMORY);
            }
            break;

        case SETREG_L:
            if (! get_arg (src_line.arg[0]))
            {
                return (FALSE);
            }

            if (arg.type != L_REG)
            {
                printerr (WRONG_VARTYPE, plist_ind, ST_PRE, src_line.arg[0]);
                return (FALSE);
            }

            getstr (src_line.arg[1], args2, MAXVARNAME, 0, strlen (src_line.arg[1]));

            strcpy (vmreg_name.l[arg.reg], args2);
            break;

        case SETREG_D:
            if (! get_arg (src_line.arg[0]))
            {
                return (FALSE);
            }

            if (arg.type != D_REG)
            {
                printerr (WRONG_VARTYPE, plist_ind, ST_PRE, src_line.arg[0]);
                return (FALSE);
            }

            getstr (src_line.arg[1], args2, MAXVARNAME, 0, strlen (src_line.arg[1]));
            strcpy (vmreg_name.d[arg.reg], args2);
            break;

        case SETREG_S:
            if (! get_arg (src_line.arg[0]))
            {
                return (FALSE);
            }

            if (arg.type != S_REG)
            {
                printerr (WRONG_VARTYPE, plist_ind, ST_PRE, src_line.arg[0]);
                return (FALSE);
            }

            getstr (src_line.arg[1], args2, MAXVARNAME, 0, strlen (src_line.arg[1]));
            strcpy (vmreg_name.s[arg.reg], args2);
            break;

        case UNSETREG_ALL_L:
            /* remove all L register names */

            init_regname_l ();
            break;

        case UNSETREG_ALL_D:
            /* remove all D register names */

            init_regname_d ();
            break;

       case UNSETREG_ALL_S:
            /* remove all S register names */

            init_regname_s ();
            break;

        case SETQUOTE:
            getstr (src_line.arg[0], args2, MAXLINELEN, 0, strlen (src_line.arg[0]));
            args2_len = strlen (args2);

            if (args2_len == 0)
            {
                printerr (SYNTAX, plist_ind, ST_PRE, "");
                return (FALSE);
            }

            if (strcmp (args2, SETDEFAULT_SB) == 0)
            {
                /* set default: quote */

                assemb_set.quote = QUOTE_SB;
            }
            else
            {
                if (args2[0] != assemb_set.separ && args2[0] != assemb_set.semicol)
                {
                    assemb_set.quote = args2[0];
                }
                else
                {
                    printerr (SYNTAX, plist_ind, ST_PRE, "");
                    return (FALSE);
                }
            }
            break;

        case SETSEPAR:
            getstr (src_line.arg[0], args2, MAXLINELEN, 0, strlen (src_line.arg[0]));
            args2_len = strlen (args2);

            if (args2_len == 0)
            {
                printerr (SYNTAX, plist_ind, ST_PRE, "");
                return (FALSE);
            }

            if (strcmp (args2, SETDEFAULT_SB) == 0)
            {
                /* set default: comma */

                assemb_set.separ = COMMA_SB;
            }
            else
            {
                if (args2[0] != assemb_set.quote && args2[0] != assemb_set.semicol)
                {
                    assemb_set.separ = args2[0];
                }
                else
                {
                    printerr (SYNTAX, plist_ind, ST_PRE, "");
                    return (FALSE);
                }
            }
            break;

        case SETSEMICOL:
            getstr (src_line.arg[0], args2, MAXLINELEN, 0, strlen (src_line.arg[0]));
            args2_len = strlen (args2);

            if (args2_len == 0)
            {
                printerr (SYNTAX, plist_ind, ST_PRE, "");
                return (FALSE);
            }

            if (strcmp (args2, SETDEFAULT_SB) == 0)
            {
                /* set default: semicol */

                assemb_set.semicol = SEMICOL_SB;
            }
            else
            {
                if (args2[0] != assemb_set.quote && args2[0] != assemb_set.separ)
                {
                    assemb_set.semicol = args2[0];
                }
                else
                {
                    printerr (SYNTAX, plist_ind, ST_PRE, "");
                    return (FALSE);
                }
            }
            break;

        default:
            if (src_line.args > 0)
            {
                ok = TRUE;
                for (i = 0; i <= src_line.args - 1; i++)
                {
                    #if DEBUG
                        if (print_debug)
                        {
                            printf ("set_opcode: arg.type: %li / op.type: %li\n", arg.type, opcode[op].type[i]);
                        }
                    #endif

                    if (opcode[op].type[i] == LABEL || opcode[op].type[i] == LABEL_SET)
                    {
                        #if DEBUG
                            if (print_debug)
                            {
                                printf ("set_opcode: LABEL / LABEL_SET", arg.type);
                            }
                        #endif

                        if (opcode[op].type[i] == LABEL_SET)
                        {
                            /* set label */

                            getstr (src_line.arg[i], args2, MAXJUMPNAME, 0, strlen (src_line.arg[i]));
                            args2_len = strlen (args2);

                            if (args2_len == 0)
                            {
                                printerr (NOTDEF_LAB, plist_ind, ST_PRE, "");
                                return (FALSE);
                            }

                            vind[i] = getjumpind (args2);
                            if (vind[i] != NOTDEF)
                            {
                                if (jumplist[vind[i]].islabel == FALSE)
                                {
                                    /* LAB already defined by a GOTO, so we set 'pos' now. */

                                    jumplist[vind[i]].pos = cclist_ind + 1;
                                    jumplist[vind[i]].source_pos = plist_ind + 1;
                                }
                                else
                                {
                                    printerr (DOUBDEF_LAB, plist_ind, ST_PRE, jumplist[vind[i]].lab);
                                    printf ("already defined in line: %li\n\n", jumplist[vind[i]].source_pos);

                                    return (FALSE);
                                }
                            }
                            else
                            {
                                if (jumplist_gonext () == FALSE)
                                {
                                    return (FALSE);
                                }

                                strcpy (jumplist[jumplist_ind].lab, args2);
                                jumplist[jumplist_ind].pos = cclist_ind + 1;
                                jumplist[jumplist_ind].source_pos = plist_ind + 1;
                                jumplist[jumplist_ind].islabel = TRUE;
                            }
                        }

                        if (opcode[op].type[i] == LABEL)
                        {
                            /* use already set label */

                            getstr (src_line.arg[i], args2, MAXJUMPNAME, 0, strlen (src_line.arg[i]));
                            args2_len = strlen (args2);

                            if (args2_len == 0)
                            {
                                printerr (NOTDEF_LAB, plist_ind, ST_PRE, "");
                                return (FALSE);
                            }

                            vind[i] = getjumpind (args2);
                            if (vind[i] == NOTDEF)
                            {
                               /* LAB not defined, so we define it now.
                                * and set the 'pos' in jumplist when we reach
                                * the LAB definition.
                                */

                                if (jumplist_gonext () == FALSE)
                                {
                                    return (FALSE);
                                }

                                strcpy (jumplist[jumplist_ind].lab, args2);
                                jumplist[jumplist_ind].pos = NOTDEF;
                                jumplist[jumplist_ind].islabel = FALSE;
                                vind[i] = jumplist_ind;
                            }
                        }
                    }
                    else
                    {
                        if (! get_arg (src_line.arg[i]))
                        {
                            ok = FALSE;
                            break;
                        }

                        if (arg.type != opcode[op].type[i] && opcode[op].type[i] != VAR)
                        {
                            printerr (WRONG_VARTYPE, plist_ind, ST_PRE, src_line.arg[i]);
                            ok = FALSE;
                            break;
                        }

                        vind[i] = arg.reg;

                        #if DEBUG
                            printf ("set_opcode: vind[%li]: %li\n", i, arg.reg);
                        #endif
                    }
                }

                if (ok)
                {
                    if (opcode[op].type[0] != LABEL_SET && opcode[op].type[1] != LABEL_SET && opcode[op].type[2] != LABEL_SET)
                    {
                        /* ist's not a "lab foobar;" opcode => store it */

                        switch (src_line.args)
                        {
                            case 1:
                                if (! set1 (op, vind[0]))
                                {
                                    return (MEMORY);
                                }
                                break;

                            case 2:
                                if (! set2 (op, vind[0], vind[1]))
                                {
                                    return (MEMORY);
                                }
                                break;

                            case 3:
                                if (! set3 (op, vind[0], vind[1], vind[2]))
                                {
                                    return (MEMORY);
                                }
                                break;
                        }
                    }
                }
                else
                {
                    return (FALSE);
                }
            }
            else
            {
                if (! set0 (op))
                {
                    printf ("set_op: MEMORY ERROR!\n");

                    return (MEMORY);
                }
            }
            break;
    }
    return (TRUE);
}

