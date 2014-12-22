/****************************************************************************
*
* Filename: arr.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  array functions
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

#include "../include/host.h"
#include "../include/global_d.h"


void *get_vmvarlib (struct varlist *varlist, S4 vind, S8 ind, U1 access, struct vm_mem *vm_mem)
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

    vm_mem->error = FALSE;

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
            vm_mem->error = TRUE;
            return (NULL);
        }
    }
    else
    {
		/* variable is in virtual memory */
		
		for (i = 0; i < MAXVMPAGES; i++)
		{
			/*
			printf ("get_vmvarlib: page_hits: %lli\n", varlist[vind].vm_pages.page_hits[i]);
			printf ("get_vmvarlib: page_start_addr: %lli\n", varlist[vind].vm_pages.page_start_addr[i]);
			printf ("get_vmvarlib: page_end_addr: %lli\n", varlist[vind].vm_pages.page_end_addr[i]);
			*/
			if ((varlist[vind].vm_pages.page_end_addr[i] != 0) && (ind >= varlist[vind].vm_pages.page_start_addr[i] && ind <= varlist[vind].vm_pages.page_end_addr[i]))
			{
				// found page
			
				cache_hit = 1;
				
				/* correct index */
					
				new_ind = ind - varlist[vind].vm_pages.page_start_addr[i];
				
				/* DEBUG */
				/*
				if (new_ind > varlist[vind].vmcachesize - 1)
				{
					vm_mem->error = TRUE;
				
					printf ("get_vmvarlib: FATAL ERROR: page index out of range! [cache hit]\n");
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
	
		if (cache_hit == 0)
		{
			/* no cache hit, search page memory to load */
		
			hits = varlist[vind].vm_pages.page_hits[0]; page = 0;
		
			for (i = 1; i < MAXVMPAGES; i++)
			{
				// printf ("page hits: page %lli, hits %lli\n", i, varlist[vind].vm_pages.page_hits[i]);
				
				if (varlist[vind].vm_pages.page_hits[i] < hits)
				{
					hits = varlist[vind].vm_pages.page_hits[i]; page = i;
				}
			}
		
			if (varlist[vind].vm_pages.page_mod[page] == PAGE_MODIFIED)
			{
				if (save_page (varlist, vind, page) == FALSE)
				{
					vm_mem->error = TRUE;

					return (NULL);
				}
			}
		
			if (ind + varlist[vind].vmcachesize > varlist[vind].size - 1)
			{
				/* correct page read position */
			
				read_ind = varlist[vind].size - varlist[vind].vmcachesize;

				if (load_page (varlist, vind, page, read_ind) == FALSE)
				{
					vm_mem->error = TRUE;

					return (NULL);
				}
			}
			else
			{
				if (load_page (varlist, vind, page, ind) == FALSE)
				{
					vm_mem->error = TRUE;

					return (NULL);
				}
			}
			
			varlist[vind].vm_pages.page_mod[page] = PAGE_READ;
			new_ind = ind - varlist[vind].vm_pages.page_start_addr[page];
			
			/* DEBUG */
			/*
			if (new_ind > varlist[vind].vmcachesize - 1)
			{
				vm_mem->error = TRUE;
				
				printf ("get_vmvarlib: FATAL ERROR: page index out of range! [cache load]\n");
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
		}
	}
	return (ret_memptr);
}
		

U1 let_array2_int (struct varlist *varlist, struct vm_mem *vm_mem, S4 reg1, S8 reg2, S8 *reg3)
{
    /* reg3 = reg1[reg2] */

    S2 *new_i_memptr;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    new_i_memptr = (S2 *) get_vmvarlib (varlist, reg1, reg2, VM_READ, vm_mem);
    if (vm_mem->error)
    {
        return (FALSE);
    }

    *reg3 = *new_i_memptr;

    return (TRUE);
}

U1 let_array2_lint (struct varlist *varlist, struct vm_mem *vm_mem, S4 reg1, S8 reg2, S8 *reg3)
{
    S4 *new_li_memptr;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    new_li_memptr = (S4 *) get_vmvarlib (varlist, reg1, reg2, VM_READ, vm_mem);
    if (vm_mem->error)
    {
        return (FALSE);
    }

    *reg3 = *new_li_memptr;

    return (TRUE);
}

U1 let_array2_qint (struct varlist *varlist, struct vm_mem *vm_mem, S4 reg1, S8 reg2, S8 *reg3)
{
    S8 *new_q_memptr;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    new_q_memptr = (S8 *) get_vmvarlib (varlist, reg1, reg2, VM_READ, vm_mem);
    if (vm_mem->error)
    {
        return (FALSE);
    }

    *reg3 = *new_q_memptr;

    return (TRUE);
}

U1 let_array2_double (struct varlist *varlist, struct vm_mem *vm_mem, S4 reg1, S8 reg2, F8 *reg3)
{
    F8 *new_d_memptr;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    new_d_memptr = (F8 *) get_vmvarlib (varlist, reg1, reg2, VM_READ, vm_mem);
    if (vm_mem->error)
    {
        return (FALSE);
    }

    *reg3 = *new_d_memptr;

    return (TRUE);
}

U1 let_array2_byte (struct varlist *varlist, struct vm_mem *vm_mem, S4 reg1, S8 reg2, S8 *reg3)
{
    U1 *new_b_memptr;

    if (varlist[reg1].size - 1 < reg2 || reg2 < 0)
    {
        return (FALSE);
    }

    new_b_memptr = (U1 *) get_vmvarlib (varlist, reg1, reg2, VM_READ, vm_mem);
    if (vm_mem->error)
    {
        return (FALSE);
    }

    *reg3 = *new_b_memptr;

    return (TRUE);
}

U1 let_array2_string (struct varlist *varlist, struct vm_mem *vm_mem, S4 reg1, S8 reg2, U1 *s_reg3)
{
    /* reg3 = reg1[reg2] */

    S4 var, pos, array_maxdim, array_string_len, i, j, ind, string_len;

    var = varlist[reg1].dimens[0];
    if (varlist[var].type == INT_VAR)
    {
        array_maxdim = *varlist[var].i_m;
    }
    else
    {
        array_maxdim = *varlist[var].li_m;
    }

    var = varlist[reg1].dimens[1];
    if (varlist[var].type == INT_VAR)
    {
        array_string_len = *varlist[var].i_m;
    }
    else
    {
        array_string_len = *varlist[var].li_m;
    }

    if (reg2 > array_maxdim - 1 || reg2 < 0)
    {
        /* array index overflow */
        return (FALSE);
    }

    /* calculate read start position */

    pos = reg2 * array_string_len;

    /* get string length */

    string_len = -1;
    j = pos;
    for (i = 1; i <= array_string_len; i++)
    {
        if (varlist[reg1].s_m[j] == BINUL)
        {
            string_len = i;
            break;
        }
        j++;
    }

    if (string_len == -1)
    {
        /* error: no string end found */
        return (FALSE);
    }

    if (string_len > MAXSTRING_VAR)
    {
        /* destination string overflow */
        return (FALSE);
    }

    /* all ok, copy string */

    j = pos; ind = 0;
    for (i = 1; i <= string_len; i++)
    {
        s_reg3[ind] = varlist[reg1].s_m[j];
        j++;
        ind++;
    }

    return (TRUE);
}


/* Always set vmcache_mod to TRUE on array write! */
/* This marks the cache as modified. */

U1 let_2array_int (struct varlist *varlist, struct vm_mem *vm_mem, S8 reg1, S4 reg2, S8 reg3)
{
    /* reg2[reg3] = reg1 */

    S2 *new_i_memptr;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    new_i_memptr = (S2 *) get_vmvarlib (varlist, reg2, reg3, VM_WRITE, vm_mem);
    if (vm_mem->error)
    {
        return (FALSE);
    }

    *new_i_memptr = reg1;

    return (TRUE);
}

U1 let_2array_lint (struct varlist *varlist, struct vm_mem *vm_mem, S8 reg1, S4 reg2, S8 reg3)
{
    S4 *new_li_memptr;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    new_li_memptr = (S4 *) get_vmvarlib (varlist, reg2, reg3, VM_WRITE, vm_mem);
    if (vm_mem->error)
    {
        return (FALSE);
    }

    *new_li_memptr = reg1;

    return (TRUE);
}

U1 let_2array_qint (struct varlist *varlist, struct vm_mem *vm_mem, S8 reg1, S4 reg2, S8 reg3)
{
    S8 *new_q_memptr;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    new_q_memptr = (S8 *) get_vmvarlib (varlist, reg2, reg3, VM_WRITE, vm_mem);
    if (vm_mem->error)
    {
        return (FALSE);
    }

    *new_q_memptr = reg1;

    return (TRUE);
}

U1 let_2array_double (struct varlist *varlist, struct vm_mem *vm_mem, F8 reg1, S4 reg2, S8 reg3)
{
    F8 *new_d_memptr;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    new_d_memptr = (F8 *) get_vmvarlib (varlist, reg2, reg3, VM_WRITE, vm_mem);
    if (vm_mem->error)
    {
        return (FALSE);
    }

    *new_d_memptr = reg1;

    return (TRUE);
}

U1 let_2array_byte (struct varlist *varlist, struct vm_mem *vm_mem, S8 reg1, S4 reg2, S8 reg3)
{
    U1 *new_b_memptr;

    if (varlist[reg2].size - 1 < reg3 || reg3 < 0)
    {
        return (FALSE);
    }

    new_b_memptr = (U1 *) get_vmvarlib (varlist, reg2, reg3, VM_WRITE, vm_mem);
    if (vm_mem->error)
    {
        return (FALSE);
    }

    *new_b_memptr = reg1;

    return (TRUE);
}

U1 let_2array_string (struct varlist *varlist, struct vm_mem *vm_mem, U1 *s_reg1, S4 reg2, S8 reg3)
{
    /* reg2[reg3] = reg1 */

    S4 var, pos, array_maxdim, array_string_len, i, j, ind, string_len;

    var = varlist[reg2].dimens[0];
    if (varlist[var].type == INT_VAR)
    {
        array_maxdim = *varlist[var].i_m;
    }
    else
    {
        array_maxdim = *varlist[var].li_m;
    }

    var = varlist[reg2].dimens[1];
    if (varlist[var].type == INT_VAR)
    {
        array_string_len = *varlist[var].i_m;
    }
    else
    {
        array_string_len = *varlist[var].li_m;
    }

    if (reg3 > array_maxdim - 1 || reg3 < 0)
    {
        /* array index overflow */
        return (FALSE);
    }

    /* calculate read start position */

    pos = reg3 * array_string_len;

    /* get string length */

    string_len = strlen (s_reg1) + 1;

    if (string_len > array_string_len)
    {
        /* array string overflow */
        return (FALSE);
    }

    /* all ok, copy string */

    j = pos; ind = 0;
    for (i = 1; i <= string_len; i++)
    {
        varlist[reg2].s_m[j] = s_reg1[ind];
        j++;
        ind++;
    }

    return (TRUE);
}
