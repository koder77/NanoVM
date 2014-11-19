/****************************************************************************
*
* Filename: pthread.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  POSIX thread functions
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
#include "arch.h"

extern struct pthreads pthreads[MAXPTHREADS];

#if OS_AROS
    extern void *pthreads_mutex;
    extern void *create_pthread_mutex;
    extern void *socket_mutex;
    extern void *file_mutex;
#else
    extern pthread_mutex_t pthreads_mutex;
    extern pthread_mutex_t create_pthread_mutex;
#endif
    
int init_pthreads (void)
{
    S4 i;

    #if OS_AROS
        pthreads_mutex = CreateMutex ();
        if (pthreads_mutex == NULL)
        {
            return (1);
        }
        
        create_pthread_mutex = CreateMutex ();
        if (create_pthread_mutex == NULL)
        {
            return (1);
        }
        
        socket_mutex = CreateMutex ();
        if (socket_mutex == NULL)
        {
            return (1);
        }
        
        file_mutex = CreateMutex ();
        if (file_mutex == NULL)
        {
            return (1);
        }
    #endif
    
    for (i = 0; i < MAXPTHREADS; i++)
    {
        pthreads[i].state = PTHREAD_UNUSED;
        #if OS_AROS
        	pthreads[i].socket_lib_open = FALSE;        /* for AROS OpenLibrary() call, open flag */
    	#endif
    }
    return (0);
}

S4 get_new_thread (void)
{
    S4 i, ret = -1;

    #if OS_AROS
        LockMutex (pthreads_mutex);
    #else
        pthread_mutex_lock (&pthreads_mutex);
    #endif
        
    for (i = 0; i < MAXPTHREADS; i++)
    {
        if (pthreads[i].state == PTHREAD_STOPPED || pthreads[i].state == PTHREAD_UNUSED)
        {
            ret = i;
            break;
        }
    }

    #if OS_AROS
        UnlockMutex (pthreads_mutex);
    #else
        pthread_mutex_unlock (&pthreads_mutex);
    #endif
    return (ret);
}

void set_thread_running (S4 threadnum)
{
    #if OS_AROS
        LockMutex (pthreads_mutex);
    #else
        pthread_mutex_lock (&pthreads_mutex);
    #endif
        
    pthreads[threadnum].state = PTHREAD_RUNNING;
    
    #if OS_AROS
        UnlockMutex (pthreads_mutex);
    #else
        pthread_mutex_unlock (&pthreads_mutex);
    #endif
}

void set_thread_stopped (S4 threadnum)
{
    #if OS_AROS
        LockMutex (pthreads_mutex);
    #else
        pthread_mutex_lock (&pthreads_mutex);
    #endif
        
    pthreads[threadnum].state = PTHREAD_STOPPED;     /* clear thread in pthreads */
    
    #if OS_AROS
        UnlockMutex (pthreads_mutex);
    #else
        pthread_mutex_unlock (&pthreads_mutex);
    #endif
}

U1 get_thread_state (S4 threadnum)
{
    U1 state;

    #if OS_AROS
        LockMutex (pthreads_mutex);
    #else
        pthread_mutex_lock (&pthreads_mutex);
    #endif
        
    state = pthreads[threadnum].state;
    
    #if OS_AROS
        UnlockMutex (pthreads_mutex);
    #else
        pthread_mutex_unlock (&pthreads_mutex);
    #endif

    return (state);
}

void join_threads (S4 threadnum)
{
    /* wait until all threads are stopped */

    U1 threads_stopped;
    S4 i;

    while (1)
    {
        threads_stopped = TRUE;

        #if OS_AROS
            LockMutex (pthreads_mutex);
        #else
            pthread_mutex_lock (&pthreads_mutex);
        #endif
            
        for (i = 1; i < MAXPTHREADS; i++)    /* thread zero is the main thread, don't wait for it!!! */
        {
            if (i != threadnum)             /* don't check the calling thread!!! */
            {
                if (pthreads[i].state == PTHREAD_RUNNING)
                {
                    threads_stopped = FALSE;
                }
            }
        }
        
        #if OS_AROS
            UnlockMutex (pthreads_mutex);
        #else
            pthread_mutex_unlock (&pthreads_mutex);
        #endif

        if (threads_stopped == TRUE)
        {
            break;
        }
    }
}


void wait_thread_running (S4 threadnum)
{
    /* wait till new thread is running... */

    U1 state;

    while (1)
    {
        #if OS_AROS
            LockMutex (pthreads_mutex);
        #else
            pthread_mutex_lock (&pthreads_mutex);
        #endif
        
        state = pthreads[threadnum].state;
        
        #if OS_AROS
            UnlockMutex (pthreads_mutex);
        #else
            pthread_mutex_unlock (&pthreads_mutex);
        #endif

        if (state == PTHREAD_RUNNING)
        {
             break;
        }
        PWAIT_TICK();
    }
}

S8 create_new_thread (S4 startpos)
{
    /* search for free thread in pthreads... */

    S4 i;
    S8 ret = -1;

    #if OS_AROS
        LockMutex (pthreads_mutex);
    #else
        pthread_mutex_lock (&pthreads_mutex);
    #endif
        
    for (i = 0; i < MAXPTHREADS; i++)
    {
        if (pthreads[i].state == PTHREAD_STOPPED || pthreads[i].state == PTHREAD_UNUSED)
        {
            ret = i;
            break;
        }
    }

    if (ret != -1)
    {
        pthreads[ret].state = PTHREAD_REQUEST;
        pthreads[ret].startpos = startpos;
    }
    
    #if OS_AROS
        UnlockMutex (pthreads_mutex);
    #else
        pthread_mutex_unlock (&pthreads_mutex);
    #endif
    return (ret);
}

S4 get_new_thread_startpos (void)
{
    /* search for thread start request in pthread list... */

    S4 i, ret = -1;

    #if OS_AROS
        LockMutex (pthreads_mutex);
    #else
        pthread_mutex_lock (&pthreads_mutex);
    #endif
        
    for (i = 0; i < MAXPTHREADS; i++)
    {
        if (pthreads[i].state == PTHREAD_REQUEST)
        {
            ret = i;
            break;
        }
    }

    #if OS_AROS
        UnlockMutex (pthreads_mutex);
    #else
        pthread_mutex_unlock (&pthreads_mutex);
    #endif
    return (ret);
}

