/****************************************************************************
*
* Filename: checkd.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  check for valid number type
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

extern struct t_var t_var;

U1 checkdigit (U1 *str)
{
    U1 binarystr[MAXSTRING_VAR + 1];
	S2 i, j, b, str_len;
    U1 ok = FALSE, check;

    str_len = strlen (str);
    t_var.digitstr_type = UNDEF_VAR;
	t_var.base = 10;

    for (i = 0; i <= str_len - 1; i++)
    {
        if (str[i] == '.')
        {
            t_var.digitstr_type = DOUBLE_VAR;
        }
    }

    if (t_var.digitstr_type == UNDEF_VAR)
    {
        /* INT, LINT, QINT or BYTE */

        i = 0;
        j = 0;
        ok = FALSE;

        while (! ok)
        {
            if ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'A' && str[i] <= 'F') || (str[i] == '-' || str[i] == '+') || str[i] == ' ')
            {
				if (str[i] >= 'A' && str[i] <= 'F')
				{
					t_var.base = 16;
				}
				
                if (j <= MAXSTRING_VAR - 1 && str[i] != ' ')
                {
                    t_var.digitstr[j] = str[i];
                    j++;
                }
                else
                {
                    ok = TRUE;
                }
                check = TRUE;
            }
            else
            {
                switch (str[i])
                {
                    case 'I':
                        t_var.digitstr_type = INT_VAR;
                        check = TRUE;
                        break;

                    case 'L':
                        t_var.digitstr_type = LINT_VAR;
                        check = TRUE;
                        break;

                   case 'Q':
                        t_var.digitstr_type = QINT_VAR;
                        check = TRUE;
                        break;

                    case '%':
                        t_var.digitstr_type = BYTE_VAR;
                        check = TRUE;
                        break;
						
					case '&':
						t_var.base = 16;
						break;
						
					case '$':
						t_var.base = 2;
						break;
						
                    default:
                        check = FALSE;
                        ok = TRUE;
                }
            }

            if (i < str_len - 1)
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
        /* DOUBLE */

        i = 0;
        j = 0;
        ok = FALSE;

        while (! ok)
        {
            if ((str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '-' || str[i] == '+' || str[i] == 'e' || str[i] == 'E' || str[i] == ' ')
            {
                if (j <= MAXSTRING_VAR - 1 && str[i] != ' ')
                {
                    t_var.digitstr[j] = str[i];
                    j++;
                }
                else
                {
                    ok = TRUE;
                }
                check = TRUE;
            }
            else
            {
                check = FALSE;
                ok = TRUE;
            }

            if (i < str_len - 1)
            {
                i++;
            }
            else
            {
                ok = TRUE;
            }
        }
    }

    t_var.digitstr[j] = '\0';
    
    
    if (t_var.base == 2)
	{
		/* if base = 2 = binary then cut away leading zeroes */
		j = 0; b = 0;
		str_len = strlen (t_var.digitstr);
		for (i = 0; i < str_len; i++)
		{
			if (t_var.digitstr[i] == '0')
			{
				if (j == 1)
				{
					binarystr[b] = '0';
					b++;
				}
			}
			
			if (t_var.digitstr[i] == '1')
			{
				j = 1;
				binarystr[b] = '1';
				b++;
			}
		}
		binarystr[b] = '\0';
	
		strcpy (t_var.digitstr, binarystr);
	}
		
    
    return (check);
}
