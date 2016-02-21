/****************************************************************************
*
* Filename: main.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  rs232 shared library
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2016
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

#include "../dll_shared_libs/nanovmlib.h"

#include "rs232.h"

int rs232_OpenComport (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 portnumber, baudrate;
	S8 retvar_index;
	U1 mode[256];
	S8 ret;

	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &retvar_index) != ERR_STACK_OK) printf ("rs232_OpenComport: stack error!\n");
	if (stpull_s (threadnum, pthreads, stacksize, mode) != ERR_STACK_OK) printf ("rs232_OpenComport: stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &baudrate) != ERR_STACK_OK) printf ("rs232_OpenComport: stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &portnumber) != ERR_STACK_OK) printf ("rs232_OpenComport: stack error!\n");
	
	ret = RS232_OpenComport (portnumber, baudrate, mode);

	var_lock (varlist, retvar_index);
	if (! let_2array_qint (varlist, vm_mem, ret, retvar_index, 0)) printf ("rs232_OpenComport: return value array overflow!\n");
	var_unlock (varlist, retvar_index);
	
	return (0);
}

int rs232_PollComport (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 portnumber, size, i;
	S8 retvar_index;
	S8 ret;
	S8 bvar_index;

	U1 buf[4096];
	
	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &retvar_index) != ERR_STACK_OK) printf ("rs232_PollComport: stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &size) != ERR_STACK_OK) printf ("rs232_PollComport: stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &bvar_index) != ERR_STACK_OK) printf ("rs232_PollComport: stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &portnumber) != ERR_STACK_OK) printf ("rs232_PollComport: stack error!\n");
	
	ret = RS232_PollComport (portnumber, buf, size);
	
	var_lock (varlist, bvar_index);
	
	for (i = 0; i < ret; i++)
	{
		if (! let_2array_byte (varlist, vm_mem, buf[i], bvar_index, i)) printf ("rs232_PollComport: return value array overflow!\n");
	}
	
	var_unlock (varlist, bvar_index);
			
	var_lock (varlist, retvar_index);
	if (! let_2array_qint (varlist, vm_mem, ret, retvar_index, 0)) printf ("rs232_PollComport: return value array overflow!\n");
	var_unlock (varlist, retvar_index);
	
	return (0);
}

int rs232_SendByte (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 portnumber;
	S8 retvar_index;
	S8 ret;
	S8 byte;
	U1 byte_b;

	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &retvar_index) != ERR_STACK_OK) printf ("rs232_SendByte: stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &byte) != ERR_STACK_OK) printf ("rs232_SendByte: stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &portnumber) != ERR_STACK_OK) printf ("rs232_SendByte stack error!\n");
	
	byte_b = byte;
	ret = RS232_SendByte (portnumber, byte_b);

	var_lock (varlist, retvar_index);
	if (! let_2array_qint (varlist, vm_mem, ret, retvar_index, 0)) printf ("rs232_SendByte: return value array overflow!\n");
	var_unlock (varlist, retvar_index);
	
	return (0);
}

int rs232_SendBuf (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 portnumber, size, i;
	S8 retvar_index;
	S8 ret;
	S8 bvar_index;
	S8 byte_b;

	U1 buf[4096];
	
	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &retvar_index) != ERR_STACK_OK) printf ("rs232_SendBuf: stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &size) != ERR_STACK_OK) printf ("rs232_SendBuf: stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &bvar_index) != ERR_STACK_OK) printf ("rs232_SendBuf: stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &portnumber) != ERR_STACK_OK) printf ("rs232_SendBuf: stack error!\n");
	
	var_lock (varlist, bvar_index);
	
	for (i = 0; i < size; i++)
	{
		if (! let_array2_byte (varlist, vm_mem, bvar_index, i, &byte_b)) printf ("rs232_SendBuf: value array overflow!\n");
		buf[i] = byte_b;
	}
	
	var_unlock (varlist, bvar_index);
	
	ret = RS232_SendBuf (portnumber, buf, size);
	
	var_lock (varlist, retvar_index);
	if (! let_2array_qint (varlist, vm_mem, ret, retvar_index, 0)) printf ("rs232_SendBuf: return value array overflow!\n");
	var_unlock (varlist, retvar_index);
	
	
	return (0);
}

int rs232_CloseComport (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 portnumber;

	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &portnumber) != ERR_STACK_OK) printf ("rs232_CloseComport stack error!\n");
	
	RS232_CloseComport (portnumber);
	
	return (0);
}
