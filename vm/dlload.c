/****************************************************************************
*
* Filename: dlload.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  load shared libraries, call library functions
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

#include "host.h"
#include "global_d.h"

extern struct dlls dlls[MAXDLLS];
extern struct pthreads pthreads[MAXPTHREADS];
extern struct vmreg vmreg_vm;

extern struct rights rights;

extern U1 portable_install;

U1 expand_dll_pathname (U1 *path, U1 *newpath)
{
	if (path[0] == '/')
	{
		/* file root access, check if possible */
		
		if (rights.fileroot == FALSE)
		{
			printf ("ERROR: no rights for root file access!\n");
			return (ERR_FILE_OPEN);
		}
		
		#if OS_ANDROID
			strcpy (newpath, "/sdcard/nanovm/");
		#else
			/* check ENV variable */
		
			if (getenv (NANOVM_ROOT_SB) == NULL)
			{
				/* ENV not set use defaults */
			
				if (OS_AMIGA || OS_AROS)
				{
					strcpy (newpath, "Work:nanovm/");
				}

				if (OS_LINUX || OS_DRAGONFLY)
				{
					strcpy (newpath, "~/nanovm/");
				}
            
				if (OS_WINDOWS)
				{
					strcpy (newpath, "C:/nanovm/");
				}
				
				if (portable_install)
				{
					strcpy (newpath, "../");
				}
			}
			else
			{
				/* use ENV variable setting */
				strcpy (newpath, getenv (NANOVM_ROOT_SB));
			}
		#endif
	}
	else
	{
		/* /lib directory access */
		
		#if OS_ANDROID
			strcpy (newpath, "/sdcard/nanovm/lib/");
		#else
			/* check ENV variable */
		
			if (getenv (NANOVM_ROOT_SB) == NULL)
			{
				/* ENV not set use defaults */
			
				if (OS_AMIGA || OS_AROS)
				{
					strcpy (newpath, "Work:nanovm/lib/");
				}

				if (OS_LINUX || OS_DRAGONFLY)
				{
					strcpy (newpath, "~/nanovm/lib/");
				}
            
				if (OS_WINDOWS)
				{
					strcpy (newpath, "C:/nanovm/lib/");
				}
				
				if (portable_install)
				{
					strcpy (newpath, "../../lib/");
				}
			}
			else
			{
				/* use ENV variable setting */
				strcpy (newpath, getenv (NANOVM_ROOT_SB));
				strcat (newpath, "/lib/");
			}	
		#endif
	}
	
	strcat (newpath, path);
	return (0);
}

void init_dlls (void)
{
	S8 i, f;
	
	for (i = 0; i < MAXDLLS; i++)
	{
		strcpy (dlls[i].name, "");		/* mark handle as free */
		
		for (f = 0; f < MAXDLLFUNC; f++)
		{
			dlls[i].func[f] = NULL;
		}
	}
}

S4 get_dll_handle (void)
{
	S8 handle = -1, i;
	for (i = 0; i < MAXDLLS; i++)
	{
		if (strcmp (dlls[i].name, "") == 0)
		{
			/* found free handle with empty name string */
			handle = i;
			break;
		}
	}
	
	return (handle);
}

S4 get_dll_func_handle (S8 index)
{
	S8 handle = -1, f;
	for (f = 0; f < MAXDLLFUNC; f++)
	{
		if (dlls[index].func[f] == NULL)
		{
			/* found free handle */
			handle = f;
			break;
		}
	}
	
	return (handle);
}

U1 exe_lopen (S8 *llist_ind, U1 *lname)
{
	U1 new_fname[MAXSTRING_VAR + 1], path[MAXSTRING_VAR + 1], convname[MAXSTRING_VAR + 1];
	S8 handle;
	
	if (rights.file == FALSE)
	{
		printf ("library open: ERROR: no rights for file access!\n");
		return (ERR_FILE_OPEN);
	}
	
	handle = get_dll_handle ();
	if (handle == -1)
	{
		/* error no free handle! */
		printf ("error no free dll handle!\n");
		return (ERR_FILE_OPEN);
	}
	
	strcpy (new_fname, lname);

    #if DEBUG
        printf ("debug: exe_lopen  new filename: '%s'\n", new_fname);
    #endif

    if (expand_dll_pathname (new_fname, path) != 0)
	{
		return (ERR_FILE_OPEN);
	}
	
	strcpy (new_fname, path);
    
	if (! conv_fname (convname, new_fname))
    {
        return (ERR_FILE_OPEN);
    }
	
	if (check_path_break (convname) != 0)
	{
		printf ("ERROR: illegal filename!\n");
		
		return (ERR_FILE_OPEN);
	}
	
	
    // load the library -------------------------------------------------
#if OS_WINDOWS
    dlls[handle].lptr = LoadLibrary (convname);
    if (!dlls[handle].lptr) 
	{
        return (ERR_FILE_OPEN);
    }
#else
#if OS_LINUX || OS_DRAGONFLY
    dlls[handle].lptr = dlopen (convname, RTLD_LAZY);
    if (!dlls[handle].lptr) 
	{
		printf ("error load dll!\n");
       return (ERR_FILE_OPEN);
    }
#endif
#endif

	strcpy (dlls[handle].name, convname);
	*llist_ind = handle;
	return (ERR_FILE_OK);
}

U1 exe_lclose (S8 llist_ind)
{
#if OS_WINDOWS
	FreeLibrary	(dlls[llist_ind].lptr);
#else
#if OS_LINUX || OS_DRAGONFLY
	dlclose (dlls[llist_ind].lptr);
#endif
#endif
	
	strcpy (dlls[llist_ind].name, "");		/* mark as empty handle */	
	
	return (ERR_FILE_OK);
}

U1 exe_lfunc (S8 llist_ind, S8 *flist_ind, U1 *fname)
{
	S8 handle;
	
	handle = get_dll_func_handle (llist_ind);
	if (handle == -1)
	{
		/* error no free handle! */
		printf ("error no free dll func handle!\n");
		return (ERR_FILE_OPEN);
	}
	
#if OS_WINDOWS

    // load the symbols -------------------------------------------------

    dlls[llist_ind].func[handle] = (dll_func*) GetProcAddress(dlls[llist_ind].lptr, fname);
    if (!dlls[llist_ind].func[handle]) 
	{
		return (ERR_FILE_READ);
    }
#else
#if OS_LINUX || OS_DRAGONFLY
    // reset errors
    dlerror ();

    // load the symbols (handle to function)
	dlls[llist_ind].func[handle] = dlsym(dlls[llist_ind].lptr, fname);
    const char* dlsym_error = dlerror ();
    if (dlsym_error) 
	{
		printf ("%s\n", dlsym_error);
        return (ERR_FILE_READ);
    }
#endif
#endif

	*flist_ind = handle;
	return (ERR_FILE_OK);
}

U1 exe_lcall (S8 llist_ind, S8 flist_ind, U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	(*dlls[llist_ind].func[flist_ind])(pthreads_ptr, varlist_ptr, vm_mem_ptr, threadnum, stacksize);
	return (0);
}


S4 close_dll_handles (void)
{
	S8 handle = -1, i;
	for (i = 0; i < MAXDLLS; i++)
	{
		if (strcmp (dlls[i].name, "") != 0)
		{
			/* found used handle, close handle */
			exe_lclose (i);
		}
	}
	
	return (handle);
}
