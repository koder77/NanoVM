/****************************************************************************
*
* Filename: hyperspace_server.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  Nano VM - multicomputer shared memory server
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

#if OS_AROS
    #define LOCK();              LockMutex (varlist[varind].mutex);
    #define UNLOCK();            UnlockMutex (varlist[varind].mutex);
#else
    #define LOCK();              pthread_mutex_lock (&varlist[varind].mutex);
    #define UNLOCK();            pthread_mutex_unlock (&varlist[varind].mutex);
#endif


extern struct varlist *varlist;
extern struct varlist *pvarlist_obj;                /* thread private varlist */
extern struct varlist_state varlist_state;

extern struct sock sock[];

S4 getvarind (U1 *str)
{
    S4 i, ind = NOTDEF;

    #if DEBUG
        printf ("getvarind: '%s'\n", str);
    #endif

    /* normal varlist */

    for (i = VARLIST_START; i <= varlist_state.varlist_size; i++)
    {
        if (strcmp (varlist[i].name, str) == 0)
        {
            return (i);
        }
    }

    #if DEBUG
        printf ("getvarind: ind: %li\n", ind);
    #endif

    return (ind);
}

U1 hs_read (S4 slist_ind, S4 len)
{
    U1 *buf;
    S2 sockh = 0, ret;
    S4 todo, buf_ind = 0;

    if (len < 0 || len > SOCKBUFSIZE)
    {
        return (ERR_FILE_READ);
    }

    sockh = sock[slist_ind].serv_conn;

    todo = len;
    buf = sock[slist_ind].buf;

    while (todo > 0)
    {
        ret = recv (sockh, &(buf[buf_ind]), todo, MSG_NOSIGNAL);
        if (ret == -1)
        {
            return (errno);
        }

        todo = todo - ret;
        buf_ind = buf_ind + ret;
    }

    return (ERR_FILE_OK);
}

S2 hs_sread_byte (S8 slist_ind, S8 *reg)
{
    S2 ret;

    ret = recv (sock[slist_ind].serv_conn, (U1 *) reg, 1, MSG_NOSIGNAL);

    if (ret == -1)
    {
        return (ERR_FILE_READ);
    }

    return (ERR_FILE_OK);
}

S2 hs_sread_qint (S8 slist_ind, S8 *reg)
{
    S2 ret;
    S8 r;

    ret = recv (sock[slist_ind].serv_conn, (U1 *) &r, sizeof (S8), MSG_NOSIGNAL);

    if (ret == -1)
    {
        return (ERR_FILE_READ);
    }

    *reg = ntohq ((S8) r);

    return (ERR_FILE_OK);
}

S2 hs_sread_double (S8 slist_ind, F8 *reg)
{
    S2 ret;
    F8 r;

    ret = recv (sock[slist_ind].serv_conn, (U1 *) &r, sizeof (F8), MSG_NOSIGNAL);

    if (ret == -1)
    {
        return (ERR_FILE_READ);
    }

    *reg = (F8) ntohd ((F8) r);

    return (ERR_FILE_OK);
}

S2 hs_swrite_byte (S8 slist_ind, S8 reg)
{
    S2 ret;

    ret = send (sock[slist_ind].serv_conn, (U1 *) &reg, 1, MSG_NOSIGNAL);
    if (ret == -1)
    {
        return (errno);
    }

    return (ERR_FILE_OK);
}

S2 hs_swrite_qint (S8 slist_ind, S8 reg)
{
    S2 ret;
    S8 r;
    
    r = htonq (reg);

    ret = send (sock[slist_ind].serv_conn, (U1 *) &r, sizeof (S8), MSG_NOSIGNAL);
    if (ret == -1)
    {
        return (errno);
    }

    return (ERR_FILE_OK);
}

S2 hs_swrite_double (S8 slist_ind, F8 reg)
{
    S2 ret;
    F8 r;
    
    r = htond (reg);

    ret = send (sock[slist_ind].serv_conn, (U1 *) &r, sizeof (F8), MSG_NOSIGNAL);
    if (ret == -1)
    {
        return (errno);
    }

    return (ERR_FILE_OK);
}

S2 hyperspace_server (S8 socket)
{
    U1 run = TRUE;
    U1 buf[4096];
    U1 varname[256];
    S4 varind;
    S8 index, max_ind;
    U1 vartype;
    U1 access;
    U1 ret;
	
    S8 qint_var; F8 double_var; U1 string_var[4096];

    if (socket < 0 || socket > MAXSOCKETS - 1)
    {
        return (1);
    }

    if (sock[socket].status != FILEOPEN)
    {
        return (1);
    }

    while (run)
    {
        /* get hyperspace variable name */
        
		// printf ("hsserver DEBUG waiting...\n");
		
        if (exe_sread_line_string (socket, varname) != ERR_FILE_OK)
        {
            printf ("hyperspace: read varname error!\n");
            return (1);
        }
        
        if (hs_sread_byte (socket, &access) != ERR_FILE_OK)
        {
            printf ("hyperspace: read access error!\n");
            return (1);
        }
        
        /* get hyperspace variable index */
        
        if (hs_sread_qint (socket, &max_ind) != ERR_FILE_OK)
        {
            printf ("hyperspace: read index error!\n");
            return (1);
        }
        
        varind = getvarind (varname);
        if (varind == NOTDEF)
        {
            printf ("hyperspace: unknown variable: %s !\n", varname);
            return (1);
        }
        
        // printf ("hsserver DEBUG got request variable: %s\n", varname);
        // printf ("hsserver DEBUG max_ind: %lli\n", max_ind);
		
        ret = 0;
		
        if (access == 1)
        {
            /* write into variable array */
            
            switch (varlist[varind].type)
            {
                case INT_VAR:
					LOCK();
					
					for (index = 0; index < max_ind; index++)
					{
						// printf ("hyperspace server: DEBUG: hssave int index: %lli ...\n", index);
						
						if (hs_sread_qint (socket, &qint_var) != ERR_FILE_OK)
						{
							printf ("hyperspace: read int error!\n");
							return (1);
						}
             
						if (! exe_let_2array_int (varlist, qint_var, varind, index))
						{
							return (1);
						}
						
						// printf ("data received!\n");
					}
					UNLOCK();
					break;
                    
                case LINT_VAR:
                    LOCK();
					
					for (index = 0; index < max_ind; index ++)
					{
						if (hs_sread_qint (socket, &qint_var) != ERR_FILE_OK)
						{
							printf ("hyperspace: read int error!\n");
							return (1);
						}
             
						if (! exe_let_2array_lint (varlist, qint_var, varind, index))
						{
							return (1);
						}
					}
					UNLOCK();
					break;
                    
                case QINT_VAR:
                    LOCK();
					
					for (index = 0; index < max_ind; index ++)
					{
						if (hs_sread_qint (socket, &qint_var) != ERR_FILE_OK)
						{
							printf ("hyperspace: read int error!\n");
							return (1);
						}
             
						if (! exe_let_2array_qint (varlist, qint_var, varind, index))
						{
							return (1);
						}
					}
					UNLOCK();
					break; 
                    
                case DOUBLE_VAR:
                    LOCK();
					
					for (index = 0; index < max_ind; index ++)
					{
						if (hs_sread_double (socket, &double_var) != ERR_FILE_OK)
						{
							printf ("hyperspace: read int error!\n");
							return (1);
						}
             
						if (! exe_let_2array_double (varlist, double_var, varind, index))
						{
							return (1);
						}
					}
					UNLOCK();
					break;
					
                 case STRING_VAR:
					LOCK();
					
					for (index = 0; index < max_ind; index ++)
					{
						if (exe_sread_line_string (socket, string_var) != ERR_FILE_OK)
						{
							printf ("hyperspace: read string error!\n");
							return (1);
						}
                
						if (! exe_let_2array_string (varlist, string_var, varind, index))
						{
							return (1);
						}
					}
					
                    UNLOCK();
                    break;
					
				case BYTE_VAR:
					LOCK();
					
					for (index = 0; index < max_ind; index ++)
					{
						if (hs_sread_qint (socket, &qint_var) != ERR_FILE_OK)
						{
							printf ("hyperspace: read int error!\n");
							return (1);
						}
             
						if (! exe_let_2array_byte (varlist, qint_var, varind, index))
						{
							return (1);
						}
					}
					UNLOCK();
					break;
            }
            
            if (exe_swrite_byte (socket, ret) != ERR_FILE_OK)
            {
                printf ("hyperspace: return error!\n");
                return (1);
            }
        }
        else
        {
            /* read from variable array */
            
            switch (varlist[varind].type)
            {
                case INT_VAR:
					// printf ("hyperspace server: DEBUG try to get LOCK for int var...\n");
					
                    LOCK();
                    
					for (index = 0; index < max_ind; index ++)
					{
						// printf ("hyperspace server: DEBUG: hsload int index: %lli ...\n", index);
						
						if (! exe_let_array2_int (varlist, varind, index, &qint_var))
						{
							printf ("hyperspace: array error!\n");
							return (1);
						}
                    
						if (hs_swrite_qint (socket, qint_var) != ERR_FILE_OK)
						{
							printf ("hyperspace: write int error!\n");
							return (1);
						}
						
						// printf ("data sent!\n");
					}
                    
                    UNLOCK();
                    break;
                    
                case LINT_VAR:
                    LOCK();
                    
					for (index = 0; index < max_ind; index ++)
					{
						if (! exe_let_array2_lint (varlist, varind, index, &qint_var))
						{
							printf ("hyperspace: array error!\n");
							return (1);
						}
                    
						if (hs_swrite_qint (socket, qint_var) != ERR_FILE_OK)
						{
							printf ("hyperspace: write int error!\n");
							return (1);
						}
					}
                    
                    UNLOCK();
                    break;
                    
                case QINT_VAR:
                    LOCK();
                    
					for (index = 0; index < max_ind; index ++)
					{
						if (! exe_let_array2_qint (varlist, varind, index, &qint_var))
						{
							printf ("hyperspace: array error!\n");
							return (1);
						}
                    
						if (hs_swrite_qint (socket, qint_var) != ERR_FILE_OK)
						{
							printf ("hyperspace: write int error!\n");
							return (1);
						}
					}
                    
                    UNLOCK();
                    break;
                    
                case DOUBLE_VAR:
                    LOCK();
                    
					for (index = 0; index < max_ind; index ++)
					{
						if (! exe_let_array2_double (varlist, varind, index, &double_var))
						{
							printf ("hyperspace: array error!\n");
							return (1);
						}
                    
						if (hs_swrite_double (socket, double_var) != ERR_FILE_OK)
						{
							printf ("hyperspace: write int error!\n");
							return (1);
						}
					}
                    
                    UNLOCK();
                    break;
					
                case STRING_VAR:
                    LOCK();
                    
					for (index = 0; index < max_ind; index ++)
					{
						if (! exe_let_array2_string (varlist, varind, index, string_var))
						{
							ret = 1;
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
                    
                    UNLOCK();
                    break; 
					
                case BYTE_VAR:
                    LOCK();
                    
					for (index = 0; index < max_ind; index ++)
					{
						if (! exe_let_array2_byte (varlist, varind, index, &qint_var))
						{
							printf ("hyperspace: array error!\n");
							return (1);
						}
                    
						if (hs_swrite_qint (socket, qint_var) != ERR_FILE_OK)
						{
							printf ("hyperspace: write int error!\n");
							return (1);
						}
					}
                    
                    UNLOCK();
                    break;
            }
        }
    }
    printf ("hyperspace: EXIT\n");
}
