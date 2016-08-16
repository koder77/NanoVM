/****************************************************************************
*
* Filename: vm.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  virtual memory
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

/* This is a new code for swapping array variables to disk.
 * It's using pages in RAM. See MAXVMPAGES definition in include/global_d.h.
 * 
 * This keeps 10 pages in RAM on default MAXVMPAGES. This should be enough.
 * The goal was to replace the old "read ahead paging" code from earlier versions.
 * I did some checks and it seems to work right.
 * 
 * HOWEVER this is new EXPERIMENTAL code! USE IT AT OUR VERY OWN RISK!!!
 * You have been warned...
 * 
 */

#include "host.h"
#include "global_d.h"

extern struct varlist *varlist;
extern struct vmreg vmreg_vm;
extern struct vm_mem vm_mem;

#if DEBUG
    extern U1 print_debug;
#endif

extern U1 portable_install;
	
U1 init_vmfile (void)
{
	// create swapfile
	
    U1 buf[256], ok, *writecache;
    S8 i ALIGN;
	S8 todosize ALIGN;
	S8 writesize ALIGN;
	
	writesize = VMINITWRITECACHE;

    /* append time to vmfile name: hhmmss */

    exe_time ();

	#if OS_ANDROID
		strcpy (vm_mem.filename, ANDROID_SDCARD);
		strcat (vm_mem.filename, "nanovm/tmp-");
	#else
		if (getenv (VM_FILE_SB) == NULL)
		{
			if (portable_install)
			{
				strcpy (vm_mem.filename, "../tmp-");
			}
			else
			{
				printerr (ENV_NOT_SET, NOTDEF, ST_EXE, VM_FILE_SB);
				return (FALSE);
			}
		}
		else
		{
			if (strlen (getenv (VM_FILE_SB)) > 255 - 6)
			{
				printerr (OVERFLOW_STR, NOTDEF, ST_EXE, VM_FILE_SB);
				return (FALSE);
			}
		
			strcpy (vm_mem.filename, getenv (VM_FILE_SB));
		}
	#endif
	
    i = *varlist[TIME_HOUR].i_m;
    if (i < 10)
    {
        strcat (vm_mem.filename, "0");
    }
    sprintf (buf, "%li", i);
    strcat (vm_mem.filename, buf);

    i = *varlist[TIME_MIN].i_m;
    if (i < 10)
    {
        strcat (vm_mem.filename, "0");
    }
    sprintf (buf, "%li", i);
    strcat (vm_mem.filename, buf);

    i = *varlist[TIME_SEC].i_m;
    if (i < 10)
    {
        strcat (vm_mem.filename, "0");
    }
    sprintf (buf, "%li", i);
    strcat (vm_mem.filename, buf);
	
    writecache = (U1 *) malloc (writesize * sizeof (U1));
    if (writecache == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_EXE, "");
        return (FALSE);
    }

    if (! (vm_mem.file = fopen (vm_mem.filename, "w+b")))
    {
        printerr (OPEN, NOTDEF, ST_EXE, vm_mem.filename);
        return (FALSE);
    }

    for (i = writesize - 1; i >= 0; i--)
    {
        writecache[i] = 0;
    }

    todosize = vmreg_vm.l[VMBSIZE];

	printmsg (VM_INIT, "");
	
	printf ("creating %lli MB swap file...\n", todosize / 1024 / 1024);
	
    ok = FALSE;
    while (! ok)
    {
        if (fwrite (writecache, sizeof (U1), writesize, vm_mem.file) != writesize)
        {
            free (writecache);
            printerr (OPEN, NOTDEF, ST_EXE, vm_mem.filename);
            return (FALSE);
        }
        else
        {
            if (todosize > 0)
            {
                todosize = todosize - writesize;
                if (todosize < writesize)
                {
                    writesize = todosize;
                }
            }
            else
            {
                ok = TRUE;
            }
        }
    }
    free (writecache);

    vm_mem.init = TRUE;
	vm_mem.ind = 0; 			/* sapfile offset index to ZER0 */
    printmsg (STATUS_OK, "");
    return (TRUE);
}

S8 get_vmblock (S8 msize)
{
	// get empty start position in swapfile
	
    S8 retvm_ind = -1;

    if (vm_mem.ind <= vmreg_vm.l[VMBSIZE] - msize)
    {
        retvm_ind = vm_mem.ind;
        vm_mem.ind = vm_mem.ind + msize;
    }

    return (retvm_ind);
}

U1 vm_alloc_pages (struct varlist *varlist, S4 vind, S8 vmcachesize, U1 type)
{
	// allocate virtual memory pages in RAM
	
	S8 pagesize ALIGN;
	S8 i ALIGN;
	S8 j ALIGN;
	
	U1 error = 1;
	
	switch (type)
    {
        case INT_VAR:
            pagesize = vmcachesize * sizeof (S2);
            break;

        case LINT_VAR:
            pagesize = vmcachesize * sizeof (S4);
            break;

        case QINT_VAR:
            pagesize = vmcachesize * sizeof (S8);
            break;

        case DOUBLE_VAR:
            pagesize = vmcachesize * sizeof (F8);
            break;

        case BYTE_VAR:
            pagesize = vmcachesize * sizeof (U1);
            break;
    }

	// alloc pages
	// varlist[ind].i_m = (S2 *) malloc (varsize * sizeof (S2));
	
	
	varlist[vind].vm_pages.size = pagesize;
	varlist[vind].vm_pages.last_hit = 0;		/* last hit in page zero, init */
	
	for (i = 0; i < MAXVMPAGES; i++)
	{
		varlist[vind].vm_pages.pages[i] = (U1 *) malloc (pagesize);
		varlist[vind].vm_pages.page_mod[i] = PAGE_READ;
		varlist[vind].vm_pages.page_hits[i] = 0;
		varlist[vind].vm_pages.page_start_addr[i] = 0;
		varlist[vind].vm_pages.page_end_addr[i] = 0;
	
		if (varlist[vind].vm_pages.pages[i] == NULL)
		{
			/* ERROR can't allocate VM page!
			 * 
			 * free all previous allocated ones...
			 */
			
			error = 0;
			
			if (i > 0)
			{
				for (j = 0; j < i; j++)
				{
					free (varlist[vind].vm_pages.pages[j]);
				}
			}
			
			break;
		}
		
		/* printf ("vm_alloc_pages: page %i set.\n", i); */
	}
	return (error);
}

void vm_free_pages (struct varlist *varlist, S4 vind)
{
	// deallocate virtual memory pages in RAM
	
	S8 i ALIGN;
	
	if (varlist[vind].vm)
    {
		// variable is in VM -> deallocate pages
		
		for (i = 0; i < MAXVMPAGES; i++)
		{
			free (varlist[vind].vm_pages.pages[i]);
			varlist[vind].vm_pages.pages[i] = NULL;
		}
	}
}

U1 save_page (struct varlist *varlist, S4 vind, S8 page)
{
	// save one page to swapfile
	
	U1 type = varlist[vind].type;
	S8 seek ALIGN;
	
	switch (type)
    {
		case INT_VAR:
			seek = varlist[vind].vm_pages.vm_ind + (varlist[vind].vm_pages.page_start_addr[page] * sizeof (S2)); 
            break;
			
		case LINT_VAR:
			seek = varlist[vind].vm_pages.vm_ind + (varlist[vind].vm_pages.page_start_addr[page] * sizeof (S4)); 
            break;

		case QINT_VAR:
			seek = varlist[vind].vm_pages.vm_ind + (varlist[vind].vm_pages.page_start_addr[page] * sizeof (S8)); 
            break;
		
		case DOUBLE_VAR:
			seek = varlist[vind].vm_pages.vm_ind + (varlist[vind].vm_pages.page_start_addr[page] * sizeof (F8)); 
            break;
		
		case BYTE_VAR:
			seek = varlist[vind].vm_pages.vm_ind + (varlist[vind].vm_pages.page_start_addr[page] * sizeof (U1)); 
            break;
	}
	
	/* set position in swapfile */
	fseek (vm_mem.file, seek, SEEK_SET);

	/* save page */
    
	/* DEBUG */
	/*
	printf ("swapping page: %lli to offset %lli, page size: %lli bytes\n", page, seek, varlist[vind].vm_pages.size);
	printf ("page mem address = %lli\n", &(varlist[vind].vm_pages.pages[page]));
	printf ("page start index: %lli\n\n", varlist[vind].vm_pages.page_start_addr[page]);
	*/
	
	varlist[vind].vm_pages.page_mod[page] = PAGE_READ;	/* reset */
	
	if (fwrite (varlist[vind].vm_pages.pages[page], sizeof (U1), varlist[vind].vm_pages.size, vm_mem.file) != varlist[vind].vm_pages.size * sizeof (U1))
	{
		return (FALSE);
	}
	else
	{
		return (TRUE);
	}
}

U1 load_page (struct varlist *varlist, S4 vind, S8 page, S8 index)
{
	// load one page from swapfile to page in RAM
	
	U1 type = varlist[vind].type;
	S8 seek ALIGN;
	
	switch (type)
    {
		case INT_VAR:
			seek = varlist[vind].vm_pages.vm_ind + (index * sizeof (S2)); 
            break;
			
		case LINT_VAR:
			seek = varlist[vind].vm_pages.vm_ind + (index * sizeof (S4)); 
            break;

		case QINT_VAR:
			seek = varlist[vind].vm_pages.vm_ind + (index * sizeof (S8)); 
            break;
		
		case DOUBLE_VAR:
			seek = varlist[vind].vm_pages.vm_ind + (index * sizeof (F8)); 
            break;
		
		case BYTE_VAR:
			seek = varlist[vind].vm_pages.vm_ind + (index * sizeof (U1)); 
            break;
	}
	
	/* set position in swapfile */
	fseek (vm_mem.file, seek, SEEK_SET);

	/* read page */
	
	if (fread (varlist[vind].vm_pages.pages[page], sizeof (U1), varlist[vind].vm_pages.size, vm_mem.file) != varlist[vind].vm_pages.size * sizeof (U1))
	{
		return (FALSE);
	}
	else
	{
		varlist[vind].vm_pages.page_start_addr[page] = index;
		varlist[vind].vm_pages.page_end_addr[page] = index + varlist[vind].vmcachesize - 1;
		
		/* set cache modified flag to read */
		varlist[vind].vm_pages.page_mod[page] = PAGE_READ;
		
		/* reset page hits */
		varlist[vind].vm_pages.page_hits[page] = 0;
		return (TRUE);
	}
}

void *get_vmvar (struct varlist *varlist, S4 vind, S8 ind, U1 access)
{
	// get address of array variable index
	
    U1 type, cache_hit = 0;
	S8 i ALIGN;
	S8 addr ALIGN;
	S8 hits ALIGN;
	S8 page ALIGN;
	S8 read_ind ALIGN;
	S8 new_ind ALIGN;
	
    U1 *new_s_memptr;
    S2 *new_i_memptr;
    S4 *new_li_memptr;
    S8 *new_q_memptr;
    F8 *new_d_memptr;

    void *ret_memptr = NULL;

    vm_mem.error = FALSE;

    type = varlist[vind].type;
	
	if (!varlist[vind].vm)
    {
        /* variable not in virtual memory, read from normal array */

        if (varlist[vind].size > 0)
        {
            /* array variable allocated, get pointer */

            switch (type)
            {
                case INT_VAR:
                    new_i_memptr = varlist[vind].i_m;
                    new_i_memptr = new_i_memptr + ind;
                    ret_memptr = (S2 *) new_i_memptr;
                    break;

                case LINT_VAR:
                    new_li_memptr = varlist[vind].li_m;
                    new_li_memptr = new_li_memptr + ind;
                    ret_memptr = (S4 *) new_li_memptr;
                    break;

                case QINT_VAR:
                    new_q_memptr = varlist[vind].q_m;
                    new_q_memptr = new_q_memptr + ind;
                    ret_memptr = (S8 *) new_q_memptr;
                    break;

                case DOUBLE_VAR:
                    new_d_memptr = varlist[vind].d_m;
                    new_d_memptr = new_d_memptr + ind;
                    ret_memptr = (F8 *) new_d_memptr;
                    break;

                case BYTE_VAR:
                    new_s_memptr = varlist[vind].s_m;
                    new_s_memptr = new_s_memptr + ind;
                    ret_memptr = (U1 *) new_s_memptr;
                    break;
            }
        }
        else
        {
            /* array variable is deallocated, set error flag */
            vm_mem.error = TRUE;
            return (NULL);
        }
    }
    else
    {
		/* variable is in virtual memory */
		
		/* check if actual index is in last hit page */
		/*
		printf ("ARRAY INDEX: %lli\n", ind);
		*/
		i = varlist[vind].vm_pages.last_hit;
		
		if ((varlist[vind].vm_pages.page_end_addr[i] != 0) && (ind >= varlist[vind].vm_pages.page_start_addr[i] && ind <= varlist[vind].vm_pages.page_end_addr[i]))
		{
			/*
			printf ("HIT IN LAST PAGE!\n");
			printf ("PAGE: %i\n", i);
			printf ("get_vmvar: page_hits: %lli\n", varlist[vind].vm_pages.page_hits[i]);
			printf ("get_vmvar: page_start_addr: %lli\n", varlist[vind].vm_pages.page_start_addr[i]);
			printf ("get_vmvar: page_end_addr: %lli\n\n", varlist[vind].vm_pages.page_end_addr[i]);
			*/
			
			// found page
			
			cache_hit = 1;
			/* varlist[vind].vm_pages.last_hit = i; */
				
			/* correct index */
					
			new_ind = ind - varlist[vind].vm_pages.page_start_addr[i];
				
				/* DEBUG */
				/*
				if (new_ind > varlist[vind].vmcachesize - 1)
				{
					vm_mem.error = TRUE;
				
					printf ("get_vmvar: FATAL ERROR: page index out of range! [cache hit]\n");
					return (NULL);
				}
				*/
				
			switch (type)
			{
				case INT_VAR:
					new_i_memptr = (S2 *) (varlist[vind].vm_pages.pages[i] + (new_ind * sizeof (S2)));
					ret_memptr = (S2 *) new_i_memptr;
					break;

				case LINT_VAR:
					new_li_memptr = (S4 *) (varlist[vind].vm_pages.pages[i] + (new_ind * sizeof (S4)));
					ret_memptr = (S4 *) new_li_memptr;
					break;

				case QINT_VAR:
					new_q_memptr = (S8 *) (varlist[vind].vm_pages.pages[i] + (new_ind * sizeof (S8)));
					ret_memptr = (S8 *) new_q_memptr;
					break;

				case DOUBLE_VAR:
					new_d_memptr = (F8 *) (varlist[vind].vm_pages.pages[i] + (new_ind * sizeof (F8)));
					ret_memptr = (F8 *) new_d_memptr;
					break;
			
				case BYTE_VAR:
					new_s_memptr = (U1 *) (varlist[vind].vm_pages.pages[i] + (new_ind * sizeof (U1)));
					ret_memptr = (U1 *) new_s_memptr;
					break;
			}
			varlist[vind].vm_pages.page_hits[i]++;
			if (access == VM_WRITE)
			{
				varlist[vind].vm_pages.page_mod[i] = PAGE_MODIFIED;		// set page modified flag
			}
		}
		else
		{
			/* search all pages for index */
			
			for (i = 0; i < MAXVMPAGES; i++)
			{
				/*
				printf ("PAGE: %i\n", i);
				printf ("get_vmvar: page_hits: %lli\n", varlist[vind].vm_pages.page_hits[i]);
				printf ("get_vmvar: page_start_addr: %lli\n", varlist[vind].vm_pages.page_start_addr[i]);
				printf ("get_vmvar: page_end_addr: %lli\n\n", varlist[vind].vm_pages.page_end_addr[i]);
				*/
			
				if ((varlist[vind].vm_pages.page_end_addr[i] != 0) && (ind >= varlist[vind].vm_pages.page_start_addr[i] && ind <= varlist[vind].vm_pages.page_end_addr[i]))
				{
					// found page
			
					cache_hit = 1;
					varlist[vind].vm_pages.last_hit = i;
				
					/* correct index */
					
					new_ind = ind - varlist[vind].vm_pages.page_start_addr[i];
				
					/* DEBUG */
					/*
					if (new_ind > varlist[vind].vmcachesize - 1)
					{
						vm_mem.error = TRUE;
				
						printf ("get_vmvar: FATAL ERROR: page index out of range! [cache hit]\n");
						return (NULL);
					}
					*/
				
					switch (type)
					{
						case INT_VAR:
							new_i_memptr = (S2 *) (varlist[vind].vm_pages.pages[i] + (new_ind * sizeof (S2)));
							ret_memptr = (S2 *) new_i_memptr;
							break;

						case LINT_VAR:
							new_li_memptr = (S4 *) (varlist[vind].vm_pages.pages[i] + (new_ind * sizeof (S4)));
							ret_memptr = (S4 *) new_li_memptr;
							break;

						case QINT_VAR:
							new_q_memptr = (S8 *) (varlist[vind].vm_pages.pages[i] + (new_ind * sizeof (S8)));
							ret_memptr = (S8 *) new_q_memptr;
							break;

						case DOUBLE_VAR:
							new_d_memptr = (F8 *) (varlist[vind].vm_pages.pages[i] + (new_ind * sizeof (F8)));
							ret_memptr = (F8 *) new_d_memptr;
							break;
			
						case BYTE_VAR:
							new_s_memptr = (U1 *) (varlist[vind].vm_pages.pages[i] + (new_ind * sizeof (U1)));
							ret_memptr = (U1 *) new_s_memptr;
							break;
					}
					varlist[vind].vm_pages.page_hits[i]++;
					if (access == VM_WRITE)
					{
						varlist[vind].vm_pages.page_mod[i] = PAGE_MODIFIED;		// set page modified flag
					}
					
					break;		/* BREAK for  and continue */
				}
			}
		}
	
		if (cache_hit == 0)
		{
			/* no cache hit, search page memory to load */
		
			/*
			printf ("LOADING NEW PAGE...\n");
			*/
			
			hits = varlist[vind].vm_pages.page_hits[0]; page = 0;
		
			for (i = 1; i < MAXVMPAGES; i++)
			{
				// printf ("page hits: page %lli, hits %lli\n", i, varlist[vind].vm_pages.page_hits[i]);
				
				if (varlist[vind].vm_pages.page_hits[i] < hits)
				{
					hits = varlist[vind].vm_pages.page_hits[i]; page = i;
				}
			}
			/*
			printf ("page loaded in %i\n\n", page);
			*/
		
			if (varlist[vind].vm_pages.page_mod[page] == PAGE_MODIFIED)
			{
				if (save_page (varlist, vind, page) == FALSE)
				{
					vm_mem.error = TRUE;

					printf ("VM ERROR save page: %i\n", page);
					
					return (NULL);
				}
			}
		
			if (ind + varlist[vind].vmcachesize > varlist[vind].size - 1)
			{
				/* correct page read position */
			
				read_ind = varlist[vind].size - varlist[vind].vmcachesize;

				if (load_page (varlist, vind, page, read_ind) == FALSE)
				{
					vm_mem.error = TRUE;

					printf ("VM ERROR load page: %i\n", page);
					
					return (NULL);
				}
			}
			else
			{
				if (load_page (varlist, vind, page, ind) == FALSE)
				{
					vm_mem.error = TRUE;

					printf ("VM ERROR load page: %i\n", page);
					
					return (NULL);
				}
			}
			
			varlist[vind].vm_pages.page_mod[page] = PAGE_READ;
			new_ind = ind - varlist[vind].vm_pages.page_start_addr[page];
			
			/* DEBUG */
			/*
			if (new_ind > varlist[vind].vmcachesize - 1)
			{
				vm_mem.error = TRUE;
				
				printf ("get_vmvar: FATAL ERROR: page index out of range! [cache load]\n");
				return (NULL);
			}
			*/
			
			switch (type)
			{
				case INT_VAR:
					new_i_memptr = (S2 *) (varlist[vind].vm_pages.pages[page] + (new_ind * sizeof (S2)));
					ret_memptr = (S2 *) new_i_memptr;
					break;

				case LINT_VAR:
					new_li_memptr = (S4 *) (varlist[vind].vm_pages.pages[page] + (new_ind * sizeof (S4)));
					ret_memptr = (S4 *) new_li_memptr;
					break;

				case QINT_VAR:
					new_q_memptr = (S8 *) (varlist[vind].vm_pages.pages[page] + (new_ind * sizeof (S8)));
					ret_memptr = (S8 *) new_q_memptr;
					break;

				case DOUBLE_VAR:
					new_d_memptr = (F8 *) (varlist[vind].vm_pages.pages[page] + (new_ind * sizeof (F8)));
					ret_memptr = (F8 *) new_d_memptr;
					break;
			
				case BYTE_VAR:
					new_s_memptr = (U1 *) (varlist[vind].vm_pages.pages[page] + (new_ind * sizeof (U1)));
					ret_memptr = (U1 *) new_s_memptr;
					break;
			}
			
			if (access == VM_WRITE)
			{
				varlist[vind].vm_pages.page_mod[page] = PAGE_MODIFIED;		// set page modified flag
			}
			
			varlist[vind].vm_pages.last_hit = page;
		}
	}
	return (ret_memptr);
}
		