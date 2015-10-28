/****************************************************************************
*
* Filename: hyperspace_client.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  Nano VM - multicomputer shared memory client functions
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

/* fallback for Amiga OS or any other without MSG_NOSIGNAL */
#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
#endif

/* return values now S2 -> bugfix */

extern struct varlist *varlist;
extern struct sock sock[];

S2 hs_cread_byte (S8 slist_ind, S8 *reg)
{
    S2 ret;

    ret = recv (sock[slist_ind].sock, (U1 *) reg, 1, MSG_NOSIGNAL);

    if (ret == -1)
    {
        return (ERR_FILE_READ);
    }

    return (ERR_FILE_OK);
}

S2 hs_cread_qint (S8 slist_ind, S8 *reg)
{
    S2 ret;
    S8 r;

    ret = recv (sock[slist_ind].sock, (U1 *) &r, sizeof (S8), MSG_NOSIGNAL);

    if (ret == -1)
    {
        return (ERR_FILE_READ);
    }

    *reg = ntohq ((S8) r);

    return (ERR_FILE_OK);
}

S2 hs_cread_double (S8 slist_ind, F8 *reg)
{
    S2 ret;
    F8 r;

    ret = recv (sock[slist_ind].sock, (U1 *) &r, sizeof (F8), MSG_NOSIGNAL);

    if (ret == -1)
    {
        return (ERR_FILE_READ);
    }

    *reg = (F8) ntohd ((F8) r);

    return (ERR_FILE_OK);
}

S2 hs_cwrite_byte (S8 slist_ind, S8 reg)
{
    S2 ret;

    ret = send (sock[slist_ind].sock, (U1 *) &reg, 1, MSG_NOSIGNAL);
    if (ret == -1)
    {
        return (errno);
    }

    return (ERR_FILE_OK);
}

S2 hs_cwrite_qint (S8 slist_ind, S8 reg)
{
    S2 ret;
    S8 r;
    
    r = htonq (reg);

    ret = send (sock[slist_ind].sock, (U1 *) &r, sizeof (S8), MSG_NOSIGNAL);
    if (ret == -1)
    {
        return (errno);
    }

    return (ERR_FILE_OK);
}

S2 hs_cwrite_double (S8 slist_ind, F8 reg)
{
    S2 ret;
    F8 r;
    
    r = htond (reg);

    ret = send (sock[slist_ind].sock, (U1 *) &r, sizeof (F8), MSG_NOSIGNAL);
    if (ret == -1)
    {
        return (errno);
    }

    return (ERR_FILE_OK);
}

S2 hs_send (S8 socket, S4 varind, S8 index, U1 access)
{
    /* send hyperspace variable name */
    
    if (exe_swrite_string (socket, varlist[varind].name) != ERR_FILE_OK)
    {
        return (1);
    }
    
	if (exe_swrite_newline (socket, 1) != ERR_FILE_OK)
	{
	   return (1);
	}
   
    if (hs_cwrite_byte (socket, access) != ERR_FILE_OK)
    {
        return (1);
    }
    
    if (hs_cwrite_qint (socket, index) != ERR_FILE_OK)
    {
        return (1);
    }
    
    return (0);
}

S2 hs_load (struct varlist *varlist, S4 reg1, S8 reg2)
{
	S8 socket = varlist[reg1].hs_sock;
	S8 ind;
	S8 qint_var; F8 double_var; U1 string_var[4096];
	
	if (varlist[reg1].size < reg2 || reg2 < 0)
    {
		printf ("hs_load: ERROR!\n");
        return (1);
    }

    if (hs_send (socket, reg1, reg2, HS_READ) != 0)
	{
		printf ("hs_send: error!\n");
        return (1);
	}
    
    switch (varlist[reg1].type)
	{
		case INT_VAR:
			for (ind = 0; ind < reg2; ind++)
			{
				if (hs_cread_qint (socket, &qint_var) != ERR_FILE_OK)
				{
					printf ("hs_load: qint error!\n");
					return (1);
				}
				
				if (exe_let_2array_int (varlist, qint_var, reg1, ind) != TRUE)
				{
					printf ("hs_load: load error!\n");
					return (1);
				}
			}
			break;
			
		case LINT_VAR:
			for (ind = 0; ind < reg2; ind++)
			{
				if (hs_cread_qint (socket, &qint_var) != ERR_FILE_OK)
				{
					printf ("hs_load: qint error!\n");
					return (1);
				}
				
				if (exe_let_2array_lint (varlist, qint_var, reg1, ind) != TRUE)
				{
					printf ("hs_load: load error!\n");
					return (1);
				}
			}
			break;
			
		case QINT_VAR:
			for (ind = 0; ind < reg2; ind++)
			{
				if (hs_cread_qint (socket, &qint_var) != ERR_FILE_OK)
				{
					printf ("hs_load: qint error!\n");
					return (1);
				}
				
				if (exe_let_2array_qint (varlist, qint_var, reg1, ind) != TRUE)
				{
					printf ("hs_load: load error!\n");
					return (1);
				}
			}
			break;
			
		case DOUBLE_VAR:
			for (ind = 0; ind < reg2; ind++)
			{
				if (hs_cread_double (socket, &double_var) != ERR_FILE_OK)
				{
					printf ("hs_load: qint error!\n");
					return (1);
				}
				
				if (exe_let_2array_double (varlist, double_var, reg1, ind) != TRUE)
				{
					printf ("hs_load: load error!\n");
					return (1);
				}
			}
			break;
			
		case STRING_VAR:
			for (ind = 0; ind < reg2; ind++)
			{
				if (exe_sread_line_string (socket, string_var) != ERR_FILE_OK)
				{
					printf ("hs_load: read string error!\n");
					return (1);
				}
                
				if (! exe_let_2array_string (varlist, string_var, reg1, ind))
				{
					printf ("hs_load: load error!\n");
					return (1);
				}
			}
			break;
			
		case BYTE_VAR:
			for (ind = 0; ind < reg2; ind++)
			{
				if (hs_cread_qint (socket, &qint_var) != ERR_FILE_OK)
				{
					printf ("hs_load: qint error!\n");
					return (1);
				}
				
				if (exe_let_2array_byte (varlist, qint_var, reg1, ind) != TRUE)
				{
					printf ("hs_load: load error!\n");
					return (1);
				}
			}
			break;
	}
	return (0);
}
			
S2 hs_save (struct varlist *varlist, S4 reg1, S8 reg2)
{
	S8 socket = varlist[reg1].hs_sock;
	S8 ind;
	S8 qint_var; F8 double_var; U1 string_var[4096];
	U1 buf[4096];
	U1 ret;
	
	if (varlist[reg1].size < reg2 || reg2 < 0)
    {
		printf ("hs_save: ERROR!\n");
        return (1);
    }

    if (hs_send (socket, reg1, reg2, HS_WRITE) != 0)
	{
		printf ("hs_send: error!\n");
        return (1);
	}					
	
	switch (varlist[reg1].type)
	{
		case INT_VAR:
			for (ind = 0; ind < reg2; ind++)
			{
				if (! exe_let_array2_int (varlist, reg1, ind, &qint_var))
				{
					printf ("hyperspace: RETURN error!\n");
					return (1);
				}  
                    
				if (hs_cwrite_qint (socket, qint_var) != ERR_FILE_OK)
				{
					printf ("hyperspace: write int error!\n");
					return (1);
				}
		    }
		    break;
			
		case LINT_VAR:
			for (ind = 0; ind < reg2; ind++)
			{
				if (! exe_let_array2_lint (varlist, reg1, ind, &qint_var))
				{
					printf ("hyperspace: RETURN error!\n");
					return (1);
				}
                    
				if (hs_cwrite_qint (socket, qint_var) != ERR_FILE_OK)
				{
					printf ("hyperspace: write int error!\n");
					return (1);
				}
		    }
		    break;
			
		case QINT_VAR:
			for (ind = 0; ind < reg2; ind++)
			{
				if (! exe_let_array2_qint (varlist, reg1, ind, &qint_var))
				{
					printf ("hyperspace: RETURN error!\n");
					return (1);
				}
                    
				if (hs_cwrite_qint (socket, qint_var) != ERR_FILE_OK)
				{
					printf ("hyperspace: write int error!\n");
					return (1);
				}
		    }
		    break;
			
		case DOUBLE_VAR:
			for (ind = 0; ind < reg2; ind++)
			{
				if (! exe_let_array2_double (varlist, reg1, ind, &double_var))
				{
					printf ("hyperspace: RETURN error!\n");
					return (1);
				}
                    
				if (hs_cwrite_double (socket, double_var) != ERR_FILE_OK)
				{
					printf ("hyperspace: write int error!\n");
					return (1);
				}
		    }
		    break;
			
		case STRING_VAR:
			for (ind = 0; ind < reg2; ind++)
			{
				if (! exe_let_array2_string (varlist, reg1, ind, string_var))
				{
				    printf ("hyperspace: RETURN error!\n");
					return (1);
				} 		
						
			    if (exe_swrite_string (socket, string_var) != ERR_FILE_OK)
				{
					printf ("hyperspace: write string error!\n");
					return (1);
				}
                    
				buf[0] = '\n';
				buf[1] = BINUL;
                    
				if (exe_swrite_string (socket, buf) != ERR_FILE_OK)
				{
					printf ("hyperspace: write string newline error!\n");
					return (1);
				}
			}
			break;
			
		case BYTE_VAR:
			for (ind = 0; ind < reg2; ind++)
			{
				if (! exe_let_array2_byte (varlist, reg1, ind, &qint_var))
				{
					printf ("hyperspace: RETURN error!\n");
					return (1);
				}
                    
				if (hs_cwrite_qint (socket, qint_var) != ERR_FILE_OK)
				{
					printf ("hyperspace: write int error!\n");
					return (1);
				}
		    }
		    break;
	}
	
	if (hs_cread_byte (socket, &ret) != ERR_FILE_OK)
	{
		printf ("hyperspace: hs_save get ACK error!\n");
		return (1);
	}
	if (ret != 0) 
	{
		printf ("hyperspace: hs_save ACK error!\n");
		return (1);
	}
	
	return (0);
}	
