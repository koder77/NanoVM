/****************************************************************************
*
* Filename: var.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  variable functions
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

#if OS_AMIGA
    #include <machine/limits.h>
#endif

extern struct varlist *varlist;
extern struct varlist *pvarlist_obj;                /* thread private varlist */
extern struct varlist_state varlist_state;
extern struct varlist_state pvarlist_state;
extern struct t_var t_var;
extern struct arg arg;
extern struct t_op t_op;
extern struct vmreg_name vmreg_name;
extern struct assemb_set assemb_set;

extern S4 plist_ind;

#if DEBUG
    extern U1 print_debug;
#endif

#if OS_AMIGA || OS_ANDROID

#if OS_AMIGA
	#define LONG_LONG_MIN   QUAD_MIN
	#define LONG_LONG_MAX   QUAD_MAX
#endif
	
/* there is no strtoll on Amiga OS gcc!!! */

/*  $NetBSD: strtoll.c,v 1.4 2005/05/16 11:27:58 lukem Exp $    */
/*  from    NetBSD: strtoll.c,v 1.6 2003/10/27 00:12:42 lukem Exp   */

/*-
 * Copyright (c) 1992, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

long long int
strtoll(const char *nptr, char **endptr, int base)
{
    const char *s;
    /* LONGLONG */
    long long int acc, cutoff ALIGN;
    int c;
    int neg, any, cutlim;

    /* endptr may be NULL */

#ifdef __GNUC__
    /* This outrageous construct just to shut up a GCC warning. */
    (void) &acc; (void) &cutoff;
#endif

    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */
    s = nptr;
    do {
        c = (unsigned char) *s++;
    } while (isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else {
        neg = 0;
        if (c == '+')
            c = *s++;
    }
    if ((base == 0 || base == 16) &&
        c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;

    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value, divided by the
     * base.  An input number that is greater than this value, if
     * followed by a legal input character, is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance, if the range for long longs is
     * [-9223372036854775808..9223372036854775807] and the input base
     * is 10, cutoff will be set to 922337203685477580 and cutlim to
     * either 7 (neg==0) or 8 (neg==1), meaning that if we have
     * accumulated a value > 922337203685477580, or equal but the
     * next digit is > 7 (or 8), the number is too big, and we will
     * return a range error.
     *
     * Set any if any `digits' consumed; make it negative to indicate
     * overflow.
     */
    cutoff = neg ? LONG_LONG_MIN : LONG_LONG_MAX;
    cutlim = (int)(cutoff % base);
    cutoff /= base;
    if (neg) {
        if (cutlim > 0) {
            cutlim -= base;
            cutoff += 1;
        }
        cutlim = -cutlim;
    }
    for (acc = 0, any = 0;; c = (unsigned char) *s++) {
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0)
            continue;
        if (neg) {
            if (acc < cutoff || (acc == cutoff && c > cutlim)) {
                any = -1;
                acc = LONG_LONG_MIN;
                errno = ERANGE;
            } else {
                any = 1;
                acc *= base;
                acc -= c;
            }
        } else {
            if (acc > cutoff || (acc == cutoff && c > cutlim)) {
                any = -1;
                acc = LONG_LONG_MAX;
                errno = ERANGE;
            } else {
                any = 1;
                acc *= base;
                acc += c;
            }
        }
    }
    if (endptr != 0)
        /* LINTED interface specification */
        *endptr = (char *)(any ? s - 1 : nptr);
    return (acc);
}
#endif

S4 getvarind (U1 *str)
{
    S4 i, ind = NOTDEF;
    U1 ok = FALSE;

    #if DEBUG
        printf ("getvarind: '%s'\n", str);
    #endif

    if (str[0] == PRIVATE_VAR_SB)
    {
        /* private varlist */

        i = 0;

        #if DEBUG
            printf ("PRIVATE VAR\n");
        #endif

        while (! ok)
        {
            if (strcmp (pvarlist_obj[i].name, str) == 0)
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
            if (strcmp (varlist[i].name, str) == 0)
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
        printf ("getvarind: ind: %li\n", ind);
    #endif

    return (ind);
}


U1 checkstring (U1 *str)
{
    U1 quote[2];
    S2 str_len, startpos, endpos;

    quote[0] = assemb_set.quote;
    quote[1] = BINUL;

    str_len = strlen (str);

    startpos = searchstr (str, quote, 0, str_len - 1, TRUE);
    if (startpos == -1)
    {
        /* no quote found, not a string! */
        return (FALSE);
    }

    endpos = searchstr (str, quote, startpos + 1, str_len - 1, TRUE);
    if (endpos == -1)
    {
        return (FALSE);
    }

    /* ok its a string: "bla bla"
    * now get the text
    */

    t_op.pos = endpos + 1;      /* next char behind string */

    partstr (str, t_var.str, startpos + 1, endpos - 1);
    return (TRUE);
}

U1 getvarstr (U1 *argstr, S2 pos_start)
{
    S2 pos, pos_end, endvn, dimstart, dimend;
    S4 varind, dims;
    U1 pstr[MAXLINELEN + 1], comm[2], dimensstr[MAXLINELEN + 1], private_var = NORMAL_VAR;
    U1 ok = FALSE;

    t_var.dims = NODIMS;
    pos_end = strlen (argstr) - 1;

    if (argstr[0] == PRIVATE_VAR_SB)
    {
        /* use pvarlist for dimens */

        private_var = PRIVATE_VAR;

        #if DEBUG
            printf ("getvarstr: PRIVATE_VAR SET\n");
        #endif
    }

    partstr (argstr, pstr, pos_start, pos_end);

    #if DEBUG
        printf ("getvarstr: argstr: '%s'\n", argstr);
    #endif

    comm[0] = AROPEN_SB;
    comm[1] = BINUL;

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

                        varind = getvarind (dimensstr);
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

                        varind = getvarind (dimensstr);
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
    }
    else
    {
        getstr (pstr, t_var.varname, MAXVARNAME, pos_start, pos_end);
    }
    return (TRUE);
}

U1 make_val (U1 type, struct varlist *varlist, U1 private)
{
    /* check if constant type is set,
    * if not, use variable type
    */

    U1 alloc = TRUE, ok;
    S4 i, new_li_var = 0;
    S2 new_i_var = 0;
    S8 new_q_var ALIGN = 0;
    F8 new_d_var ALIGN = 0;
    U1 new_b_var = 0;

	char *endptr;
	
    if (t_var.digitstr_type == UNDEF_VAR)
    {
        t_var.type = type;
    }
    else
    {
        t_var.type = t_var.digitstr_type;
    }

    strcpy (t_var.varname, "");
    t_var.dims = NODIMS;


    switch (t_var.type)
    {
        case INT_VAR:
            new_i_var = (S2) strtoll (t_var.digitstr, &endptr, t_var.base);
			if (*endptr != '\0') printf ("make_val: error: make int!\n");
            break;

        case LINT_VAR:
            new_li_var = (S4) strtoll (t_var.digitstr, &endptr, t_var.base);
			if (*endptr != '\0') printf ("make_val: error: make lint!\n");
            break;

        case QINT_VAR:
            errno = 0;
            new_q_var = (S8) strtoll (t_var.digitstr, &endptr, t_var.base);
            if (*endptr != '\0') printf ("make_val: error: make qint!\n");
            break;

        case DOUBLE_VAR:
            new_d_var = strtod (t_var.digitstr, NULL);
			if (errno != 0) printf ("make_val: error: make double!\n");
            break;

        case BYTE_VAR:
            new_b_var = (U1) strtoll (t_var.digitstr, &endptr, t_var.base);
			if (*endptr != '\0') printf ("make_val: error: make byte!\n");
            break;
    }

    /* search if there is a constant variable which uses the
    * constant readed from program-line
    * to reduce needed memory.
    */

    ok = FALSE, i = VARLIST_START;
    while (! ok)
    {
        if (strcmp (varlist[i].name, "") == 0)
        {
            if (varlist[i].type == t_var.type)
            {
                switch (t_var.type)
                {
                    case INT_VAR:
                        if (*varlist[i].i_m == new_i_var)
                        {
                            /* found matching already allocated constant */
                            t_var.varlist_ind = i;
                            alloc = FALSE, ok = TRUE;
                        }
                        break;

                    case LINT_VAR:
                        if (*varlist[i].li_m == new_li_var)
                        {
                            /* found matching already allocated constant */
                            t_var.varlist_ind = i;
                            alloc = FALSE, ok = TRUE;
                        }
                        break;

                    case QINT_VAR:
                        if (*varlist[i].q_m == new_q_var)
                        {
                            /* found matching already allocated constant */
                            t_var.varlist_ind = i;
                            alloc = FALSE, ok = TRUE;
                        }
                        break;

                     case DOUBLE_VAR:
                        if (*varlist[i].d_m == new_d_var)
                        {
                            /* found matching already allocated constant */

                            t_var.varlist_ind = i;
                            alloc = FALSE, ok = TRUE;
                        }
                        break;

                    case BYTE_VAR:
                        if (varlist[i].s_m[0] == new_b_var)
                        {
                            /* found matching already allocated constant */
                            t_var.varlist_ind = i;
                            alloc = FALSE, ok = TRUE;
                        }
                        break;
                }
            }
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
    
    if (alloc)
    {
        if (private == NORMAL_VAR)
        {
            if (init_var (varlist, &varlist_state) == FALSE)
            {
                return (FALSE);
            }

            if (exe_alloc_var (varlist, varlist_state.varlist_ind, t_var.type) == FALSE)
            {
                return (FALSE);
            }

            switch (t_var.type)
            {
                case INT_VAR:
                    *varlist[varlist_state.varlist_ind].i_m = new_i_var;
                    break;

                case LINT_VAR:
                    *varlist[varlist_state.varlist_ind].li_m = new_li_var;
                    break;

                case QINT_VAR:
                    *varlist[varlist_state.varlist_ind].q_m = new_q_var;
                    break;

                case DOUBLE_VAR:
                    *varlist[varlist_state.varlist_ind].d_m = new_d_var;
                    break;

                case BYTE_VAR:
                    varlist[varlist_state.varlist_ind].s_m[0] = new_b_var;
                    break;
            }
            t_var.varlist_ind = varlist_state.varlist_ind;
        }
        else
        {
            /* private variable */

            if (init_var (varlist, &pvarlist_state) == FALSE)
            {
                return (FALSE);
            }

            if (exe_alloc_var (varlist, pvarlist_state.varlist_ind, t_var.type) == FALSE)
            {
                return (FALSE);
            }

            switch (t_var.type)
            {
                case INT_VAR:
                    *varlist[pvarlist_state.varlist_ind].i_m = new_i_var;
                    break;

                case LINT_VAR:
                    *varlist[pvarlist_state.varlist_ind].li_m = new_li_var;
                    break;

                case QINT_VAR:
                    *varlist[pvarlist_state.varlist_ind].q_m = new_q_var;
                    break;

                case DOUBLE_VAR:
                    *varlist[pvarlist_state.varlist_ind].d_m = new_d_var;
                    break;

                case BYTE_VAR:
                    varlist[pvarlist_state.varlist_ind].s_m[0] = new_b_var;
                    break;
            }
            t_var.varlist_ind = pvarlist_state.varlist_ind;
        }
    }
    return (TRUE);
}

U1 make_string (void)
{
    U1 alloc = TRUE, ok;
    S4 i, dim_ind;

    /* init dimension index-variable */

    t_var.type = INT_VAR;
    t_var.dims = NODIMS;
    strcpy (t_var.varname, "");
    if (! init_var (varlist, &varlist_state)) return (FALSE);
    if (! exe_alloc_var (varlist, varlist_state.varlist_ind, t_var.type)) return (FALSE);

    dim_ind = varlist_state.varlist_ind;
    *varlist[dim_ind].i_m = strlen (t_var.str) + 1;

    t_var.type = STRING_VAR;
    t_var.dims = 0;
    t_var.dimens[0] = dim_ind;

    /* search if there is a constant variable which uses the
    * constant readed from program-line
    * to reduce needed memory.
    */

    ok = FALSE, i = VARLIST_START;
    while (! ok)
    {
        if (strcmp (varlist[i].name, "") == 0)
        {
            if (varlist[i].type == STRING_VAR && varlist[i].dims == 0)
            {
                if (strcmp (varlist[i].s_m, t_var.str) == 0)
                {
                    /* found matching already allocated constant */

                    t_var.varlist_ind = i;
                    alloc = FALSE, ok = TRUE;
                }
            }
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

    if (alloc)
    {
        if (init_var (varlist, &varlist_state) == FALSE)
        {
            return (FALSE);
        }

        if (exe_alloc_var (varlist, varlist_state.varlist_ind, t_var.type) == FALSE)
        {
            return (FALSE);
        }

        #if DEBUG
            if (print_debug)
            {
                printf ("make_string: copy string...\n");
            }
        #endif

        strcpy (varlist[varlist_state.varlist_ind].s_m, t_var.str);
        t_var.varlist_ind = varlist_state.varlist_ind;

        #if DEBUG
            if (print_debug)
            {
                printf ("make_string: copied\n");
            }
        #endif
    }
    return (TRUE);
}

U1 get_arg (U1 *str)
{
    U1 tstr[MAXLINELEN + 1];
    S2 str_len, i;
    U1 private_var = FALSE;

    strcpy (tstr, "");
    str_len = strlen (str) - 1;

    #if DEBUG
        if (print_debug)
        {
            printf ("get_arg: '%s'\n", str);
            printf ("get_arg: tstr: '%s'\n", tstr);
        }
    #endif


    /* check if register name, and use the register-number */

    for (i = 0; i <= MAXVMREG - 1; i++)
    {
        if (strcmp (vmreg_name.l[i], str) == 0)
        {
            arg.reg = i;
            arg.type = L_REG;

            #if DEBUG
                printf ("found REGNAME L: %li\n", arg.reg);
            #endif
            return (TRUE);
        }

        if (strcmp (vmreg_name.d[i], str) == 0)
        {
            arg.reg = i;
            arg.type = D_REG;

            #if DEBUG
                printf ("found REGNAME D: %li\n", arg.reg);
            #endif
            return (TRUE);
        }

        if (strcmp (vmreg_name.s[i], str) == 0)
        {
            arg.reg = i;
            arg.type = S_REG;

            #if DEBUG
                printf ("found REGNAME S: %li\n", arg.reg);
            #endif
            return (TRUE);
        }
    }

    if (str[0] == PRIVATE_VAR_SB) private_var = TRUE;

    if (str[0] == 'L')
    {
        arg.type = L_REG;
        for (i = 1; i <= str_len; i++)
        {
            tstr[i - 1] = str[i];
        }
        tstr[i - 1] = BINUL;

        #if DEBUG
            if (print_debug)
            {
                printf ("get_arg: tstr: '%s'\n", tstr);
            }
        #endif

        if (checkdigit (tstr) == TRUE)
        {
            arg.reg = atol (t_var.digitstr);

            #if DEBUG
                if (print_debug)
                {
                    printf ("get_arg: arg.reg: '%li'\n", arg.reg);
                }
            #endif

            /* check if register is in legal range */
            if (arg.reg >= 0 && arg.reg <= MAXVMREG - 1)
            {
                return (TRUE);
            }
            else
            {
                printerr (OVERFLOW_IND, plist_ind, ST_PRE, str);
                return (FALSE);
            }
        }
/*
        else
        {
            return (FALSE);
        }
*/
    }
    if (str[0] == 'D')
    {
        arg.type = D_REG;
        for (i = 1; i <= str_len; i++)
        {
            tstr[i - 1] = str[i];
        }
        tstr[i - 1] = BINUL;

        #if DEBUG
            if (print_debug)
            {
                printf ("get_arg: tstr: '%s'\n", tstr);
            }
        #endif

        if (checkdigit (tstr) == TRUE)
        {
            arg.reg = atol (t_var.digitstr);

            #if DEBUG
                if (print_debug)
                {
                    printf ("get_arg: arg.reg: '%li'\n", arg.reg);
                }
            #endif

            /* check if register is in legal range */
            if (arg.reg >= 0 && arg.reg <= MAXVMREG - 1)
            {
                return (TRUE);
            }
            else
            {
                printerr (OVERFLOW_IND, plist_ind, ST_PRE, str);
                return (FALSE);
            }
        }
/*
        else
        {
            return (FALSE);
        }
*/
    }
    if (str[0] == 'S')
    {
        arg.type = S_REG;
        for (i = 1; i <= str_len; i++)
        {
            tstr[i - 1] = str[i];
        }
        tstr[i - 1] = BINUL;

        #if DEBUG
            if (print_debug)
            {
                printf ("get_arg: tstr: '%s'\n", tstr);
            }
        #endif

        if (checkdigit (tstr) == TRUE)
        {
            arg.reg = atol (t_var.digitstr);

            #if DEBUG
                if (print_debug)
                {
                    printf ("get_arg: arg.reg: '%li'\n", arg.reg);
                }
            #endif

            /* check if register is in legal range */
            if (arg.reg >= 0 && arg.reg <= MAXVMREG - 1)
            {
                return (TRUE);
            }
            else
            {
                printerr (OVERFLOW_IND, plist_ind, ST_PRE, str);
                return (FALSE);
            }
        }
/*
        else
        {
            return (FALSE);
        }
*/
    }


    if (str[0] == 'B')
    {
        /* binary number: B10001 */

        if (checkdigit (str) == TRUE)
        {
			if (! make_val (INT_VAR, varlist, NORMAL_VAR))
			{
				printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
				return (FALSE);
			}
        }

        arg.type = t_var.type;
        arg.reg = t_var.varlist_ind;

        return (TRUE);
    }

    if (str[0] == '&')
    {
        /* hex number: &FF */

        if (checkdigit (str) == TRUE)
        {
			if (! make_val (INT_VAR, varlist, NORMAL_VAR))
			{
				printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
				return (FALSE);
			}
        }

        arg.type = t_var.type;
        arg.reg = t_var.varlist_ind;

        return (TRUE);
    }

    getstr (str, tstr, MAXLINELEN, 0, str_len);
    if (checkdigit (tstr) == TRUE)
    {
        if (! make_val (INT_VAR, varlist, NORMAL_VAR))
        {
            printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
            return (FALSE);
        }

        arg.type = t_var.type;
        arg.reg = t_var.varlist_ind;
    }
    else
    {
        if (checkstring (tstr) == TRUE)
        {
            /* it's a = "bla bla" , we make same as with a number
            * declare a new variable and copy at runtime
            */

            if (! make_string ())
            {
                printerr (MEMORY, plist_ind, ST_PRE, t_var.varname);
                return (FALSE);
            }

            arg.type = STRING_VAR;
            arg.reg = t_var.varlist_ind;
        }
        else
        {
            if (! getvarstr (tstr, 0))
            {
                return (FALSE);
            }

            arg.reg = getvarind (t_var.varname);
            if (arg.reg == NOTDEF)
            {
                printerr (NOTDEF_VAR, plist_ind, ST_PRE, t_var.varname);
                return (FALSE);
            }
            if (private_var == FALSE)
            {
                arg.type = varlist[arg.reg].type;

                if (varlist[arg.reg].dims != NODIMS)
                {
                    switch (varlist[arg.reg].type)
                    {
                        case INT_VAR:
                            arg.type = INT_A_VAR;
                            break;

                        case LINT_VAR:
                            arg.type = LINT_A_VAR;
                            break;

                        case QINT_VAR:
                            arg.type = QINT_A_VAR;
                            break;

                        case DOUBLE_VAR:
                            arg.type = DOUBLE_A_VAR;
                            break;

                        case BYTE_VAR:
                            arg.type = BYTE_A_VAR;
                            break;

                        case DYNAMIC_VAR:
                            arg.type = DYNAMIC_A_VAR;
                            break;
                    }
                }
            }
            else
            {
                arg.type = pvarlist_obj[arg.reg].type;

                if (pvarlist_obj[arg.reg].dims != NODIMS)
                {
                    switch (pvarlist_obj[arg.reg].type)
                    {
                        case INT_VAR:
                            arg.type = INT_A_VAR;
                            break;

                        case LINT_VAR:
                            arg.type = LINT_A_VAR;
                            break;

                        case QINT_VAR:
                            arg.type = QINT_A_VAR;
                            break;

                        case DOUBLE_VAR:
                            arg.type = DOUBLE_A_VAR;
                            break;

                        case BYTE_VAR:
                            arg.type = BYTE_A_VAR;
                            break;

                        case DYNAMIC_VAR:
                            arg.type = DYNAMIC_A_VAR;
                            break;
                    }
                }
            }
        }
    }
    return (TRUE);
}
