/****************************************************************************
*
* Filename: nanovmlib.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  Nano VM shared library - stack functions for developing native C Nano VM libraries 
*           see vm/exe_stack.c
* 
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2014
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

#include "../include/global_d.h"

/* stack push functions ------------------------------------------------------- */

U1 stpush_l (S4 threadnum, struct pthreads *pthreads, S8 stacksize, S8 reg)
{
    U1 *ptr;
    S4 i, pos;

    pos = stacksize - (sizeof (U1) + sizeof (S8));
    
	if (pthreads[threadnum].stack_ind <= pos)
    {
        ptr = (U1 *) &reg;

        for (i = 0; i < sizeof (S8); i++)
        {
            pthreads[threadnum].stack[pthreads[threadnum].stack_ind] = ptr[i];
            pthreads[threadnum].stack_ind++;
        }

        /* write type */
        pthreads[threadnum].stack[pthreads[threadnum].stack_ind] = L_REG;
        pthreads[threadnum].stack_ind++;

        pthreads[threadnum].stack_elements++;
        return (ERR_STACK_OK);
    }
    else
    {
        return (ERR_STACK_OVERFLOW);
    }
}

U1 stpush_d (S4 threadnum, struct pthreads *pthreads, S8 stacksize, F8 reg)
{
    U1 *ptr;
    S4 i, pos;
	
    pos = stacksize - (sizeof (U1) + sizeof (F8));
    if (pthreads[threadnum].stack_ind <= pos)
    {
        ptr = (U1 *) &reg;

        for (i = 0; i < sizeof (F8); i++)
        {
            pthreads[threadnum].stack[pthreads[threadnum].stack_ind] = ptr[i];
            pthreads[threadnum].stack_ind++;
        }

        /* write type */
        pthreads[threadnum].stack[pthreads[threadnum].stack_ind] = D_REG;
        pthreads[threadnum].stack_ind++;

        pthreads[threadnum].stack_elements++;
        return (ERR_STACK_OK);
    }
    else
    {
        return (ERR_STACK_OVERFLOW);
    }
}

U1 stpush_s (S4 threadnum, struct pthreads *pthreads, S8 stacksize, U1 *s_reg)
{
    U1 *ptr;
    S4 i, pos;
    S4 str_len;

    str_len = strlen (s_reg) + 1;
    ptr = (U1 *) &str_len;

    pos = stacksize - (sizeof (U1) + sizeof (S4) + str_len);
    if (pthreads[threadnum].stack_ind <= pos)
    {
        /* write string */
        for (i = 0; i < str_len; i++)
        {
            pthreads[threadnum].stack[pthreads[threadnum].stack_ind] = s_reg[i];
            pthreads[threadnum].stack_ind++;
        }

        /* write string length */
        for (i = 0; i < sizeof (S4); i++)
        {
            pthreads[threadnum].stack[pthreads[threadnum].stack_ind] = ptr[i];
            pthreads[threadnum].stack_ind++;
        }

        /* write type */
        pthreads[threadnum].stack[pthreads[threadnum].stack_ind] = STRING_VAR;
        pthreads[threadnum].stack_ind++;

        pthreads[threadnum].stack_elements++;
        return (ERR_STACK_OK);
    }
    else
    {
        return (ERR_STACK_OVERFLOW);
    }
}


/* stack pull functions ------------------------------------------------------- */

U1 stpull_l (S4 threadnum, struct pthreads *pthreads, S8 stacksize, S8 *reg)
{
    U1 type, *ptr;
    S4 i, pos;
	
    pos = pthreads[threadnum].stack_ind - (sizeof (U1) + sizeof (S8));
    if (pos >= 0)
    {
        pthreads[threadnum].stack_ind--;
        type = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];
        ptr = (U1 *) reg;

        if (type != L_REG)
        {
            /* error: wrong type */

            return (ERR_STACK_WRONG_TYPE);
        }

        for (i = sizeof (S8) - 1; i >= 0; i--)
        {
            pthreads[threadnum].stack_ind--;
            ptr[i] = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];
        }

        pthreads[threadnum].stack_elements--;
        return (ERR_STACK_OK);
    }
    else
    {
        return (STACK_OVERFLOW);
    }
}

U1 stpull_d (S4 threadnum, struct pthreads *pthreads, S8 stacksize, F8 *reg)
{
    U1 type, *ptr;
    S4 i, pos;

    pos = pthreads[threadnum].stack_ind - (sizeof (U1) + sizeof (F8));
    if (pos >= 0)
    {
        pthreads[threadnum].stack_ind--;
        type = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];
        ptr = (U1 *) reg;

        if (type != D_REG)
        {
            /* error: wrong type */

            return (ERR_STACK_WRONG_TYPE);
        }

        for (i = sizeof (F8) - 1; i >= 0; i--)
        {
            pthreads[threadnum].stack_ind--;
            ptr[i] = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];
        }

        pthreads[threadnum].stack_elements--;
        return (ERR_STACK_OK);
    }
    else
    {
        return (ERR_STACK_OVERFLOW);
    }
}

U1 stpull_s (S4 threadnum, struct pthreads *pthreads, S8 stacksize, U1 *s_reg)
{
    U1 type, *ptr;
    S4 i, pos;
    S4 str_len;


    ptr = (U1 *) &str_len;

    pos = pthreads[threadnum].stack_ind - sizeof (U1);
    if (pos >= 0)
    {
        pthreads[threadnum].stack_ind--;
        type = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];

        if (type != STRING_VAR)
        {
            /* error: wrong type */

            return (ERR_STACK_WRONG_TYPE);
        }

        pos = pthreads[threadnum].stack_ind - sizeof (S4);
        if (pos >= 0)
        {
            /* get string length */
            for (i = sizeof (S4) - 1; i >= 0; i--)
            {
                pthreads[threadnum].stack_ind--;
                ptr[i] = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];
            }
        }
        else
        {
            return (ERR_STACK_OVERFLOW);
        }

        if (pthreads[threadnum].stack_ind - str_len >= 0)
        {
            if (MAXSTRING_VAR < str_len)
            {
                /* error: string var overflow */

                return (ERR_STACK_OVERFLOW_VAR);
            }

            /* get string */
            for (i = str_len - 1; i >= 0; i--)
            {
                pthreads[threadnum].stack_ind--;
                s_reg[i] = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];
            }


            pthreads[threadnum].stack_elements--;
            return (ERR_STACK_OK);
        }
        else
        {
            return (ERR_STACK_OVERFLOW);
        }
    }
    else
    {
        return (ERR_STACK_OVERFLOW);
    }
}


/* stack info functions ------------------------------------------------------- */

U1 stgettype (S4 threadnum, struct pthreads *pthreads, S8 *reg)
{
    /* get type of stack element */

    U1 type;

    if (pthreads[threadnum].stack_ind > 0)
    {
        type = pthreads[threadnum].stack[pthreads[threadnum].stack_ind - 1];
        *reg = type;

        return (ERR_STACK_OK);
    }
    else
    {
        return (STACK_OVERFLOW);
    }
}

U1 stelements (S4 threadnum, struct pthreads *pthreads, S8 *reg)
{
    /* get number of stack elements */

    *reg = pthreads[threadnum].stack_elements;

    return (ERR_STACK_OK);
}



S8 showstr (U1 *ptr, S4 threadnum, S8 stacksize)
{
	S8 ret = 42;
	struct pthreads *pthreads;
	pthreads = (struct pthreads *) ptr;
	
	printf ("DLL start...\n");
	
	if (stpush_l (threadnum, pthreads, stacksize, ret) != ERR_STACK_OK) printf ("showstr: stack error!\n");
		
	
	printf ("stack: %lli\n", ret);
	printf ("EXIT DLL\n");
	return (0);
}
