/****************************************************************************
*
* Filename: exe_stack.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  stack functions
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2006
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
#include "arch.h"

#if OS_AROS
    #define LOCK();              LockMutex (pthreads[threadnum].stack_mutex);
    #define UNLOCK();            UnlockMutex (pthreads[threadnum].stack_mutex);
#else
    #define LOCK();              pthread_mutex_lock(&pthreads[threadnum].stack_mutex);
    #define UNLOCK();            pthread_mutex_unlock(&pthreads[threadnum].stack_mutex);
#endif

extern struct varlist *varlist;
extern struct vmreg vmreg_vm;
extern struct pthreads pthreads[MAXPTHREADS];
extern S4 stack_cli;


S2 get_sterr (S2 err)
{
    S2 sterr = 0;

    switch (err)
    {
        case ERR_STACK_MEMORY:
            sterr = MEMORY;
            break;

        case ERR_STACK_OVERFLOW:
            sterr = STACK_OVERFLOW;
            break;

        case ERR_STACK_OVERFLOW_VAR:
            sterr = OVERFLOW_VAR;
            break;

        case ERR_STACK_WRONG_TYPE:
            sterr = WRONG_VARTYPE;
            break;
    }

    return (sterr);
}

int init_stack (void)
{
    S4 threadnum;

    for (threadnum = 0; threadnum < MAXPTHREADS; threadnum++)
    {
        pthreads[threadnum].stack = NULL;
	pthreads[threadnum].stack_elements = -1;
	
        /* INITIALIZE VARIABLE MUTEX-STATE */
        
        #if OS_AROS
            pthreads[threadnum].stack_mutex = CreateMutex ();
            if (pthreads[threadnum].stack_mutex == NULL)
            {
                return (1);
            }
        #else
            pthread_mutex_init (&pthreads[threadnum].stack_mutex, NULL);
        #endif
    }
    return (0);
}

U1 alloc_stack (void)
{
    S4 threadnum;

    if (stack_cli != 0)
    {
        /* set cli stack size */
        vmreg_vm.l[STSIZE] = stack_cli;
    }

    for (threadnum = 0; threadnum < MAXPTHREADS; threadnum++)
    {
        LOCK();
        pthreads[threadnum].stack = (U1 *) malloc (vmreg_vm.l[STSIZE] * sizeof (U1));
        if (pthreads[threadnum].stack == NULL)
        {
            UNLOCK();
            return (ERR_STACK_MEMORY);
        }
        else
        {
            pthreads[threadnum].stack_ind = 0;
            pthreads[threadnum].stack_elements = 0;
        }
        UNLOCK();
    }
    return (ERR_STACK_OK);
}

U1 check_stack (S4 threadnum)
{
    /* checks if stack is allocated */

    LOCK();
    if (pthreads[threadnum].stack != NULL)
    {
        UNLOCK();
        return (TRUE);
    }
    else
    {
        UNLOCK();
        return (FALSE);
    }
}


U1 exe_stpush_l (S4 threadnum, S8 reg)
{
    U1 *ptr;
    S4 i, pos;

    if (! check_stack (threadnum))
    {
        return (ERR_STACK_OVERFLOW);
    }

    LOCK();
    pos = vmreg_vm.l[STSIZE] - (sizeof (U1) + sizeof (S8));
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
        UNLOCK();
        return (ERR_STACK_OK);
    }
    else
    {
        UNLOCK();
        return (ERR_STACK_OVERFLOW);
    }
}

U1 exe_stpush_d (S4 threadnum, F8 reg)
{
    U1 *ptr;
    S4 i, pos;

    if (! check_stack (threadnum))
    {
        return (ERR_STACK_OVERFLOW);
    }

    LOCK();
    pos = vmreg_vm.l[STSIZE] - (sizeof (U1) + sizeof (F8));
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
        UNLOCK();
        return (ERR_STACK_OK);
    }
    else
    {
        UNLOCK();
        return (ERR_STACK_OVERFLOW);
    }
}

U1 exe_stpush_s (S4 threadnum, U1 *s_reg)
{
    U1 *ptr;
    S4 i, pos;
    S4 str_len;

    if (! check_stack (threadnum))
    {
        return (ERR_STACK_OVERFLOW);
    }

    LOCK();
    str_len = strlen (s_reg) + 1;
    ptr = (U1 *) &str_len;

    pos = vmreg_vm.l[STSIZE] - (sizeof (U1) + sizeof (S4) + str_len);
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
        UNLOCK();
        return (ERR_STACK_OK);
    }
    else
    {
        UNLOCK();
        return (ERR_STACK_OVERFLOW);
    }
}


U1 exe_stpull_l (S4 threadnum, S8 *reg)
{
    U1 type, *ptr;
    S4 i, pos;

    if (! check_stack (threadnum))
    {
        return (ERR_STACK_OVERFLOW);
    }

    LOCK();
    pos = pthreads[threadnum].stack_ind - (sizeof (U1) + sizeof (S8));
    if (pos >= 0)
    {
        pthreads[threadnum].stack_ind--;
        type = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];
        ptr = (U1 *) reg;

        if (type != L_REG)
        {
            /* error: wrong type */

            UNLOCK();
            return (ERR_STACK_WRONG_TYPE);
        }

        for (i = sizeof (S8) - 1; i >= 0; i--)
        {
            pthreads[threadnum].stack_ind--;
            ptr[i] = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];
        }

        pthreads[threadnum].stack_elements--;
        UNLOCK();
        return (ERR_STACK_OK);
    }
    else
    {
        UNLOCK();
        return (STACK_OVERFLOW);
    }
}

U1 exe_stpull_d (S4 threadnum, F8 *reg)
{
    U1 type, *ptr;
    S4 i, pos;

    if (! check_stack (threadnum))
    {
        return (ERR_STACK_OVERFLOW);
    }

    LOCK();
    pos = pthreads[threadnum].stack_ind - (sizeof (U1) + sizeof (F8));
    if (pos >= 0)
    {
        pthreads[threadnum].stack_ind--;
        type = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];
        ptr = (U1 *) reg;

        if (type != D_REG)
        {
            /* error: wrong type */

            UNLOCK();
            return (ERR_STACK_WRONG_TYPE);
        }

        for (i = sizeof (F8) - 1; i >= 0; i--)
        {
            pthreads[threadnum].stack_ind--;
            ptr[i] = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];
        }

        pthreads[threadnum].stack_elements--;
        UNLOCK();
        return (ERR_STACK_OK);
    }
    else
    {
        UNLOCK();
        return (ERR_STACK_OVERFLOW);
    }
}

U1 exe_stpull_s (S4 threadnum, U1 *s_reg)
{
    U1 type, *ptr;
    S4 i, pos;
    S4 str_len;

    if (! check_stack (threadnum))
    {
        return (ERR_STACK_OVERFLOW);
    }

    LOCK();

    ptr = (U1 *) &str_len;

    pos = pthreads[threadnum].stack_ind - sizeof (U1);
    if (pos >= 0)
    {
        pthreads[threadnum].stack_ind--;
        type = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];

        if (type != STRING_VAR)
        {
            /* error: wrong type */

            UNLOCK();
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
            UNLOCK();
            return (ERR_STACK_OVERFLOW);
        }

        if (pthreads[threadnum].stack_ind - str_len >= 0)
        {
            if (MAXSTRING_VAR < str_len)
            {
                /* error: string var overflow */

                UNLOCK();
                return (ERR_STACK_OVERFLOW_VAR);
            }

            /* get string */
            for (i = str_len - 1; i >= 0; i--)
            {
                pthreads[threadnum].stack_ind--;
                s_reg[i] = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];
            }


            pthreads[threadnum].stack_elements--;
            UNLOCK();
            return (ERR_STACK_OK);
        }
        else
        {
            UNLOCK();
            return (ERR_STACK_OVERFLOW);
        }
    }
    else
    {
        UNLOCK();
        return (ERR_STACK_OVERFLOW);
    }
}

U1 exe_show_stack (S4 threadnum)
{
    U1 type, read = TRUE;
    S4 i, pos, ind = pthreads[threadnum].stack_ind;
    S4 str_len;

    U1 *string, *ptr;
    S4 reg_l;
    F8 reg_d;

    if (! check_stack (threadnum))
    {
        return (ERR_STACK_OVERFLOW);
    }

    LOCK();
    printf ("\ndebug: show stack\n");

    while (read)
    {
        pos = ind - sizeof (U1);
        if (pos >= 0)
        {
            ind--;
            type = pthreads[threadnum].stack[ind];

            switch (type)
            {
                case L_REG:
                    ptr = (U1 *) &reg_l;

                    if (ind - sizeof (S8) >= 0)
                    {
                        for (i = sizeof (S8) - 1; i >= 0; i--)
                        {
                            ind--;
                            ptr[i] = pthreads[threadnum].stack[ind];
                        }

                        printf ("lint: %li\n", reg_l);
                    }
                    else
                    {
                        UNLOCK();
                        return (ERR_STACK_OVERFLOW);
                    }
                    break;

                case D_REG:
                    ptr = (U1 *) &reg_d;

                    if (ind - sizeof (F8) >= 0)
                    {
                        for (i = sizeof (F8) - 1; i >= 0; i--)
                        {
                            ind--;
                            ptr[i] = pthreads[threadnum].stack[ind];
                        }

                        printf ("double: %.10lf\n", reg_d);
                    }
                    else
                    {
                        UNLOCK();
                        return (ERR_STACK_OVERFLOW);
                    }
                    break;

                case STRING_VAR:
                    ptr = (U1 *) &str_len;

                    pos = ind - sizeof (S4);
                    if (pos >= 0)
                    {
                        /* get string length */
                        for (i = sizeof (S4) - 1; i >= 0; i--)
                        {
                            ind--;
                            ptr[i] = pthreads[threadnum].stack[ind];
                        }
                    }
                    else
                    {
                        UNLOCK();
                        return (ERR_STACK_OVERFLOW);
                    }

                    pos = ind - str_len;
                    if (pos >= 0)
                    {
                        string = (U1 *) malloc (str_len * sizeof (U1));
                        if (string == NULL)
                        {
                            printf ("can't allocate memory for string!\n");
                            return (ERR_STACK_MEMORY);
                        }

                        /* get string */
                        for (i = str_len - 1; i >= 0; i--)
                        {
                            ind--;
                            string[i] = pthreads[threadnum].stack[ind];
                        }

                        printf ("string: '%s'\n", string);
                        free (string);
                    }
                    else
                    {
                        UNLOCK();
                        return (ERR_STACK_OVERFLOW);
                    }
                    break;
            }
        }
        else
        {
            read = FALSE;
        }
    }

    printf ("stack end\n");

    UNLOCK();
    return (ERR_STACK_OK);
}

U1 exe_stgettype (S4 threadnum, S8 *reg)
{
    /* get type of stack element */

    U1 type;

    if (! check_stack (threadnum))
    {
        return (ERR_STACK_OVERFLOW);
    }

    LOCK();
    if (pthreads[threadnum].stack_ind > 0)
    {
        type = pthreads[threadnum].stack[pthreads[threadnum].stack_ind - 1];
        *reg = type;

        UNLOCK();
        return (ERR_STACK_OK);
    }
    else
    {
        UNLOCK();
        return (STACK_OVERFLOW);
    }
}

U1 exe_stelements (S4 threadnum, S8 *reg)
{
    /* get number of stack elements */

    if (! check_stack (threadnum))
    {
        return (ERR_STACK_OVERFLOW);
    }

    LOCK();
    *reg = pthreads[threadnum].stack_elements;
    UNLOCK();

    return (ERR_STACK_OK);
}

/* THREAD SYNC FUNCTIONS */

U1 exe_stpush_sync (S4 threadnum)
{
    S4 pos;

    if (! check_stack (threadnum))
    {
        return (ERR_STACK_OVERFLOW);
    }

    LOCK();
    pos = vmreg_vm.l[STSIZE] - (sizeof (U1));
    if (pthreads[threadnum].stack_ind <= pos)
    {
        /* write type */
        pthreads[threadnum].stack[pthreads[threadnum].stack_ind] = THREAD_SYNC;
        pthreads[threadnum].stack_ind++;

        pthreads[threadnum].stack_elements++;
        UNLOCK();
        return (ERR_STACK_OK);
    }
    else
    {
        UNLOCK();
        return (ERR_STACK_OVERFLOW);
    }
}

U1 exe_stpull_sync (S4 threadnum)
{
    U1 type, sync = FALSE;
    S4 pos, i;

    if (! check_stack (threadnum))
    {
        return (ERR_STACK_OVERFLOW);
    }

    /* check_stack = syncing with thread */

    while (sync == FALSE)
    {
        LOCK();
        for (i = pthreads[threadnum].stack_ind - 1; i >= 0; i--)
        {
            if (pthreads[threadnum].stack[i] == THREAD_SYNC)
            {
                sync = TRUE;
                break;
            }
        }
        UNLOCK();
        PWAIT_TICK();
    }

    LOCK();
    pos = pthreads[threadnum].stack_ind - (sizeof (U1));
    if (pos >= 0)
    {
        pthreads[threadnum].stack_ind--;
        type = pthreads[threadnum].stack[pthreads[threadnum].stack_ind];

        if (type != THREAD_SYNC)
        {
            /* error: wrong type */

            UNLOCK();
            return (ERR_STACK_WRONG_TYPE);
        }

        pthreads[threadnum].stack_elements--;
        UNLOCK();
        return (ERR_STACK_OK);
    }
    else
    {
        UNLOCK();
        return (STACK_OVERFLOW);
    }
}
