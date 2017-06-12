/****************************************************************************
*
* Filename: i2c-main.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  i2c shared library for Raspberry Pi
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2017
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

#include <wiringPiI2C.h>

int i2c_setup (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 id, fd;
	S8 ret;

	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &id) != ERR_STACK_OK) printf ("i2c_setup: id stack error!\n");
	
	if ((fd = wiringPiI2CSetup (id)) < 0)
	{
		printf ("i2c_setup: ERROR: opening I2C bus!\n");
	}

	if (stpush_l (threadnum, pthreads, stacksize, fd) != ERR_STACK_OK) printf ("i2c_setup: fd return stack error!\n");
	return (0);
}

int i2c_read (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 fd;
	S8 ret;

	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &fd) != ERR_STACK_OK) printf ("i2c_read: fd stack error!\n");
	
	ret = wiringPiI2CRead (fd);
	if (ret < 0)
	{
		printf ("i2c_read: ERROR!\n");
	}
	
	if (stpush_l (threadnum, pthreads, stacksize, ret) != ERR_STACK_OK) printf ("i2c_read: ret return stack error!\n");
	return (0);
}

int i2c_write (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 fd;
	S8 data, ret;

	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &data) != ERR_STACK_OK) printf ("i2c_write: data stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &fd) != ERR_STACK_OK) printf ("i2c_write: fd stack error!\n");
	
	ret = wiringPiI2CWrite (fd, data);
	if (ret < 0)
	{
		printf ("i2c_write: ERROR!\n");
	}
	return (0);
}

int i2c_writereg8 (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 fd;
	S8 data, reg, ret;

	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &data) != ERR_STACK_OK) printf ("i2c_writereg8: data stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &reg) != ERR_STACK_OK) printf ("i2c_writereg8: reg stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &fd) != ERR_STACK_OK) printf ("i2c_writereg8: fd stack error!\n");
	
	ret = wiringPiI2CWriteReg8 (fd, reg, data);
	if (ret < 0)
	{
		printf ("i2c_writereg8: ERROR!\n");
	}
	return (0);
}

int i2c_writereg16 (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 fd;
	S8 data, reg, ret;

	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &data) != ERR_STACK_OK) printf ("i2c_writereg16: data stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &reg) != ERR_STACK_OK) printf ("i2c_writereg16: reg stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &fd) != ERR_STACK_OK) printf ("i2c_writereg16: fd stack error!\n");
	
	ret = wiringPiI2CWriteReg16 (fd, reg, data);
	if (ret < 0)
	{
		printf ("i2c_writereg16: ERROR!\n");
	}
	return (0);
}


int i2c_readreg8 (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 fd;
	S8 reg, ret;

	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &reg) != ERR_STACK_OK) printf ("i2c_readreg8: reg stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &fd) != ERR_STACK_OK) printf ("i2c_readreg8: fd stack error!\n");
	
	ret = wiringPiI2CReadReg8 (fd, reg);
	if (ret < 0)
	{
		printf ("i2c_readreg8: ERROR!\n");
	}
	
	if (stpush_l (threadnum, pthreads, stacksize, ret) != ERR_STACK_OK) printf ("i2c_readreg8: ret return stack error!\n");
	return (0);
}

int i2c_readreg16 (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 fd;
	S8 reg, ret;

	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &reg) != ERR_STACK_OK) printf ("i2c_readreg16: reg stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &fd) != ERR_STACK_OK) printf ("i2c_readreg16: fd stack error!\n");
	
	ret = wiringPiI2CReadReg16 (fd, reg);
	if (ret < 0)
	{
		printf ("i2c_readreg16: ERROR!\n");
	}
	
	if (stpush_l (threadnum, pthreads, stacksize, ret) != ERR_STACK_OK) printf ("i2c_readreg16: ret return stack error!\n");
	return (0);
}
