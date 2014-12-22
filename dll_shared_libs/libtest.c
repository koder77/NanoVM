/****************************************************************************
*
* Filename: libtest.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  Nano VM shared library - test library 
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

#include "../include/host.h"
#include "../include/global_d.h"

#include "nanovmlib.h"

S8 pushtostack (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 ret = 42;
	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
	
	printf ("pushtostack start...\n");
	
	if (stpush_l (threadnum, pthreads, stacksize, ret) != ERR_STACK_OK) printf ("pushtostack: stack error!\n");
		
	printf ("now on stack: %lli\n", ret);
	return (0);
}

S8 initarrayq (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 i, max, var;

	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
	
	if (stpull_l (threadnum, pthreads, stacksize, &var) != ERR_STACK_OK) printf ("initarrayq: stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &max) != ERR_STACK_OK) printf ("initarrayq: stack error!\n");
	
	printf ("initializing array...\n");
	printf ("variable ptr: %lli\n", var);
	printf ("max: %lli\n", max);
	
	for (i = 0; i <= max; i++)
	{
		if (! let_2array_qint (varlist, vm_mem, i, var, i)) printf ("initarrayq: array overflow!\n");
	}
	
	printf ("finished!\n");
	
	return (0);
}

		