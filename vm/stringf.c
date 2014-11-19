/****************************************************************************
*
* Filename: stringf.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  string functions
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

extern struct t_op t_op;

S2 checksemicol (U1 *str, S2 start, S2 end)
{
    U1 comm[2];

    comm[0] = SEMICOL_SB;
    comm[1] = BINUL;
    t_op.pos = searchstr (str, comm, start, end, TRUE);
    if (t_op.pos != -1)
    {
        t_op.type = SEMICOL;
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}

void skipspaces (U1 *str, U1 *retstr)
{
    S2 i = 0, j = 0, end;
    U1 ok = FALSE;

    end = strlen (str) - 1;

    /* skip leading spaces */
    while (! ok)
    {
        if (str[i] != SPACE_SB)
        {
            ok = TRUE;
        }

        if (i < end)
        {
            i++;
        }
        else
        {
            ok = TRUE;
        }
    }
    if (i <= end)
    {
        for (i = i - 1; i <= end; i++)
        {
            retstr[j] = str[i];
            j++;
        }
        retstr[j] = BINUL;
    }
    else
    {
        retstr[j] = BINUL;
    }
}

void partstr (U1 *str, U1 *retstr, S2 start, S2 end)
{
    S2 i, j = 0;
    for (i = start; i <= end; i++)
    {
        retstr[j] = str[i];
        j++;
    }
    retstr[j] = BINUL;
}

void getstr (U1 *str, U1 *retstr, S2 maxlen, S2 start, S2 end)
{
    S2 i = start, j = -1;
    U1 ok = FALSE, check = FALSE, endch;

    /* skip leading spaces */
    while (! ok)
    {
        if (str[i] != SPACE_SB)
        {
            ok = TRUE;
        }

        if (i < end)
        {
            i++;
        }
        else
        {
            ok = TRUE;
        }
    }
    if (i <= end)
    {
        i--;
        if (str[i] == QUOTE_SB)
        {
            /* first char is a quote, it's a string */

            endch = QUOTE_SB;
        }
        else
        {
            /* it's not a string */

            endch = SPACE_SB;
        }
        j++;
        retstr[j] = str[i];
        i++;

        ok = FALSE;
        while (! ok)
        {
            if (str[i] != endch)
            {
                check = TRUE;
                if (j < maxlen)
                {
                    j++;
                    retstr[j] = str[i];
                }
            }
            else
            {
                if (check)
                {
                    ok = TRUE;
                }
            }
            if (i < end)
            {
                i++;
            }
            else
            {
                ok = TRUE;
            }
        }

        if (endch == QUOTE_SB && j < maxlen)
        {
            j++;
            retstr[j] = QUOTE_SB;
        }
        j++;
        retstr[j] = BINUL;
    }
}

S2 searchpart (U1 *str, S2 start, S2 end)
{
    U1 quote[2], op = FALSE, ok = FALSE;
    S2 startpos, endpos;

    quote[0] = QUOTE_SB;
    quote[1] = BINUL;

    op = checksemicol (str, start, end);
    startpos = searchstr (str, quote, start, end, TRUE);
    if (startpos != -1 && t_op.pos > startpos)
    {
        /* quote found */
        while (! ok)
        {
            endpos = searchstr (str, quote, startpos + 1, end, TRUE);
            if (endpos != -1)
            {
                startpos = searchstr (str, quote, endpos + 1, end, TRUE);
                if (startpos != -1)
                {
                    op = checksemicol (str, endpos + 1, startpos - 1);
                    if (op)
                    {
                        ok = TRUE;
                    }
                }
                else
                {
                    op = checksemicol (str, endpos + 1, end);
                    ok = TRUE;
                }
            }
            else
            {
                ok = TRUE;
            }
        }
    }
    if (! op)
    {
        return (-1);
    }
    else
    {
        return (t_op.pos);
    }
}

S2 searchstr (U1 *str, U1 *srchstr, S2 start, S2 end, U1 case_sens)
{
    /* checked: ok! */

    S2 i, j = 0, pos = -1, str_len, srchstr_len;
    U1 ok = FALSE, check = TRUE;
    S2 new_end;
    U1 new_str, new_srchstr;

    str_len = strlen (str);
    srchstr_len = strlen (srchstr);

    if (start < 0 || start > str_len - 1)
    {
        i = 0;
    }
    else
    {
        i = start;
    }

    if (end == 0)
    {
        new_end = str_len - 1;
    }
    else
    {
        new_end = end;
    }

    while (! ok)
    {
        if (case_sens)
        {
            if (str[i] == srchstr[j])
            {
                pos = i;

                /* found start of searchstring, checking now */

                if (srchstr_len > 1)
                {
                    for (j = j + 1; j <= srchstr_len - 1; j++)
                    {
                        if (i < new_end)
                        {
                            i++;
                        }

                        if (str[i] != srchstr[j]) check = FALSE;
                    }
                }
                if (check)
                {
                    ok = TRUE;
                }
                else
                {
                    pos = -1;
                }
            }
            if (i < new_end)
            {
                i++;
            }
            else
            {
                ok = TRUE;
            }
        }
        else
        {
            new_str = str[i];
            new_srchstr = srchstr[j];

            if (str[i] >= 97 && str[i] <= 122)
            {
                new_str = str[i] - 32;
            }
            if (srchstr[j] >= 97 && srchstr[j] <= 122)
            {
                new_srchstr = srchstr[j] - 32;
            }

            if (new_str == new_srchstr)
            {
                pos = i;

                /* found start of searchstring, checking now */

                if (srchstr_len > 1)
                {
                    for (j = j + 1; j <= srchstr_len - 1; j++)
                    {
                        if (i < new_end)
                        {
                            i++;
                        }

                        new_str = str[i];
                        new_srchstr = srchstr[j];

                        if (str[i] >= 97 && str[i] <= 122)
                        {
                            new_str = str[i] - 32;
                        }
                        if (srchstr[j] >= 97 && srchstr[j] <= 122)
                        {
                            new_srchstr = srchstr[j] - 32;
                        }

                        if (new_str != new_srchstr) check = FALSE;
                    }
                }
                if (check)
                {
                    ok = TRUE;
                }
                else
                {
                    pos = -1;
                }
            }
            if (i < new_end)
            {
                i++;
            }
            else
            {
                ok = TRUE;
            }
        }
    }
    return (pos);
}
