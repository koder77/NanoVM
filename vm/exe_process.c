/****************************************************************************
*
* Filename: exe_process.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  process functions
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

#if OS_WINDOWS
    #include <windows.h>
    #include <errno.h>
#endif

#if OS_LINUX || OS_WINDOWS_CYGWIN || OS_ANDROID || OS_DRAGONFLY
    #include <sys/wait.h>
    #include <errno.h>
#endif


extern struct varlist *varlist;

extern struct rights rights;


/* WINDOWS ---------------------------------------------------------- */

#if OS_WINDOWS

U1 exe_run_process (U1 *name, S8 *process)
{
    U1 ret;
    U1 new_name[MAXSTRING_VAR + 1];

    STARTUPINFO start_info;
    PROCESS_INFORMATION process_info;

    if (rights.process == FALSE)
	{
		printf ("ERROR: no rights for execute process!\n");
		return (FALSE);
	}
	
	
	GetStartupInfo (&start_info);

    /* convert slashes */
    if (strlen (name) > MAXSTRING_VAR)
    {
        *process = 0;
        return (FALSE);
    }

    if (! conv_fname (new_name, name))
    {
        *process = 0;
        return (FALSE);
    }


    ret = CreateProcess (NULL, new_name, NULL, NULL, FALSE, 0, NULL, NULL, &start_info, &process_info);
    if (! ret)
    {
        *process = 0;
        return (FALSE);
    }
    else
    {
        *process = (S4) process_info.hProcess;
        return (TRUE);
    }
}

U1 exe_run_shell (U1 *command, S8 *retval)
{
    U1 new_command[MAXSTRING_VAR + 1];
    S2 ret;

	if (rights.process == FALSE)
	{
		printf ("ERROR: no rights for execute process!\n");
		return (FALSE);
	}
	
	
    /* convert slashes */
    if (strlen (command) > MAXSTRING_VAR)
    {
        *retval = 0;
        return (FALSE);
    }

    if (! conv_fname (new_command, command))
    {
        *retval = 0;
        return (FALSE);
    }


    ret = system (new_command);
    if (ret <= 0)
    {
        *retval = 0;
        perror ("exe_run_shell:\n");
        return (FALSE);
    }
    else
    {
        *retval = ret;
        return (TRUE);
    }
}

U1 exe_wait_process (S8 process, S8 *retval)
{
    U1 ret;
    S4 code = STILL_ACTIVE;

    while (code == STILL_ACTIVE)
    {
        /* wait 5 secs, for "friendly" polling */
        Sleep (250 * CLOCKS_PER_SEC / NANO_TIMER_TICKS);

        ret = GetExitCodeProcess ((HANDLE) process, &code);
    }

    if (! ret)
    {
        *retval = 0;
        return (FALSE);
    }
    else
    {
        *retval = code;
        return (TRUE);
    }
}

#endif


/* LINUX ------------------------------------------------------------ */

#if OS_LINUX || OS_WINDOWS_CYGWIN || OS_ANDROID || OS_DRAGONFLY

U1 exe_run_process (U1 *name, S8 *process)
{
    pid_t child;
    U1 new_name[MAXSTRING_VAR + 1];

    if (rights.process == FALSE)
	{
		printf ("ERROR: no rights for execute process!\n");
		return (FALSE);
	}
	
	/* convert slashes */
    if (strlen (name) > MAXSTRING_VAR - 2)
    {
        *process = 0;
        return (FALSE);
    }

    if (! conv_fname (new_name, name))
    {
        *process = 0;
        return (FALSE);
    }

    /* add " &" at the end of new_name, to start as background process */

    strcat (new_name, " &");

    child = fork ();

    if (child == 0)
    {
        exit (system (new_name));
    }
    else
    {
        *process = (S4) child;
        return (TRUE);
    }
}

U1 exe_run_shell (U1 *command, S8 *retval)
{
    U1 new_command[MAXSTRING_VAR + 1];
    S2 ret;

    if (rights.process == FALSE)
	{
		printf ("ERROR: no rights for execute process!\n");
		return (FALSE);
	}
	
	/* convert slashes */
    if (strlen (command) > MAXSTRING_VAR)
    {
        *retval = 0;
        return (FALSE);
    }

    if (! conv_fname (new_command, command))
    {
        *retval = 0;
        return (FALSE);
    }

    printf ("command: %s\n", new_command);

    ret = system (new_command);
    if (ret <= 0)
    {
        *retval = 0;
        perror ("exe_run_shell:\n");
        return (FALSE);
    }
    else
    {
        *retval = ret;
        return (TRUE);
    }
}

U1 exe_wait_process (S8 process, S8 *retval)
{
    pid_t ret;
    /* S4 code; */
    int code;

    ret = waitpid ((pid_t) process, &code, 0);
    if (ret != ((pid_t) process))
    {
        *retval = 0;
        return (FALSE);
    }
    else
    {
        if (WIFEXITED (code))
        {
            *retval = (S4) WEXITSTATUS (code);
            return (TRUE);
        }
        else
        {
            *retval = 0;
            return (FALSE);
        }
    }
}

#endif


/* AMIGA ------------------------------------------------------------ */

#if OS_AMIGA || OS_AROS

/* I couldn't find any good information about how to do this.
 * All I know is that CreateNewProc() launches a new process.
 * If you now more, then please drop me a line!
 */

U1 exe_run_process (U1 *name, S8 *process)
{
    *process = 0;
    printf ("error: runpr not implemented!\n");

    return (FALSE);
}

U1 exe_run_shell (U1 *command, S8 *retval)
{
    *retval = 0;
    printf ("error: runsh not implemented!\n");

    return (FALSE);
}

U1 exe_wait_process (S8 process, S8 *retval)
{
    *retval = 0;
    printf ("error: waitpr not implemented!\n");

    return (FALSE);
}

#endif
