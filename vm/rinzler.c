/****************************************************************************
*
* Filename: rinzler.c ;name taken from "TRON legacy"
*
* Rinzler: "I FIGHT FOR THE USERS!!!"
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  memory protection for constant variables, debugging only!
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2011
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

#if DEBUG

extern struct varlist *varlist;
extern struct vmreg vmreg;
extern S4 varlist_size;
extern S4 **cclist;

U1 check_code (S4 threadnum, S4 maxcclist, S4 **pcclist)
{
    S4 i, j; U1 ret = TRUE;

    for (i = 0; i <= maxcclist; i++)
    {
        for (j = 0; j < MAXCCOMMARG; j++)
        {
            if (pcclist[i][j] != cclist[i][j])
            {
                ret = FALSE;

                printf ("check_code: FATAL ERROR: thread: %li / opcode: %li, argument: %li, changed to %li !!!\n", threadnum, i, j, pcclist[i][j]);
            }
        }
    }
}

U1 rinzler (void)
{
    static U1 firstrun = 1;
    static S4 hash = 0;
    S4 i, j, str_len, hash_new = 0;

    /* do hashing the constants for "memory protection" */
    /* for debugging only! */

    for (i = LINT_TMP_2 + 1; i <= varlist_size; i++)
    {
        if (strcmp (varlist[i].name, "") == 0)
        {
            /* variable is constant, calculate hash */

            switch (varlist[i].type)
            {
                case BYTE_VAR:
                    hash_new = hash_new ^ (S4) varlist[i].s_m[0];
                    break;

                case INT_VAR:
                    hash_new = hash_new ^ (S4) varlist[i].i_m;
                    break;

                case LINT_VAR:
                    hash_new = hash_new ^ (S4) varlist[i].li_m;
                    break;

                case QINT_VAR:
                    hash_new = hash_new ^ (S8) varlist[i].q_m;
                    break;

                case DOUBLE_VAR:
                    hash_new = hash_new ^ (S4) varlist[i].d_m;
                    break;

                case STRING_VAR:
                    str_len = strlen (varlist[i].s_m);
                    for (j = 0; j < str_len; j++)
                    {
                        hash_new = hash_new ^ varlist[i].s_m[j];
                    }
                    break;
            }
        }
    }

    if (firstrun == 0)
    {
        if (hash != hash_new)
        {
            /* fatal error, should never happen... */

            printf ("SYSTEM FAILURE, RELEASE RINZLER!\nCONSTANT CHANGED!\n\n");

            return (1);
        }
        else
        {
            return (0);
        }
    }
    else
    {
        /* Rinzler running the first time */
        /* print out active message */

        hash = hash_new;
        firstrun = 0;

        printf ("RINZLER: PROTECTING THE SYSTEM!\n");
        return (0);
    }
}

#endif
