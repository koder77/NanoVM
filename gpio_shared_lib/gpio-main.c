/****************************************************************************
*
* Filename: gpio-main.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  gpio shared library for Raspberry Pi
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

// proof of concept, a minimal digital read/write GPIO library

#include "../include/host.h"
#include "../include/global_d.h"

#include "../dll_shared_libs/nanovmlib.h"

#include <wiringPi.h>

int gpio_setup (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	wiringPiSetup ();
	return (0);
}

int gpio_digital_write (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 value, pin;
	
	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &value) != ERR_STACK_OK) printf ("gpio_digital_write: value stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &pin) != ERR_STACK_OK) printf ("gpio_digital_write: pin stack error!\n");
	
	digitalWrite (pin, value);
	return (0);
}

int gpio_digital_read (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 value, pin;
	
	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
	
	if (stpull_l (threadnum, pthreads, stacksize, &pin) != ERR_STACK_OK) printf ("gpio_digital_read: pin stack error!\n");

	value = digitalRead (pin);
	
	if (stpush_l (threadnum, pthreads, stacksize, value) != ERR_STACK_OK) printf ("gpio_digital_read: value return stack error!\n");
	return (0);
}

int gpio_pwm_write (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 value, pin;
	
	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &value) != ERR_STACK_OK) printf ("gpio_pwm_write: value stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &pin) != ERR_STACK_OK) printf ("gpio_pwm_write: pin stack error!\n");
	
	pwmWrite (pin, value);
	return (0);
}

int gpio_pwm_set_mode (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 mode;
	
	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &mode) != ERR_STACK_OK) printf ("gpio_pwm_set_mode: mode stack error!\n");
	
	pwmSetMode (mode);
	return (0);
}

int gpio_pwm_set_range (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 range;
	
	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &range) != ERR_STACK_OK) printf ("gpio_pwm_set_range: range stack error!\n");
	
	pwmSetRange (range);
	return (0);
}

int gpio_pwm_set_clock (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 divisor;
	
	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &divisor) != ERR_STACK_OK) printf ("gpio_pwm_set_clock: divisor stack error!\n");
	
	pwmSetClock (divisor);
	return (0);
}

int gpio_pullupdn_control (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 pud, pin;
	
	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &pud) != ERR_STACK_OK) printf ("gpio_pullupdn_control: pud stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &pin) != ERR_STACK_OK) printf ("gpio_pullupdn_control: pin stack error!\n");
	
	pullUpDnControl (pin, pud);
	return (0);
}

int gpio_pinmode (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
	S8 mode, pin;
	
	struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

	if (stpull_l (threadnum, pthreads, stacksize, &mode) != ERR_STACK_OK) printf ("gpio_pinmode: mode stack error!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &pin) != ERR_STACK_OK) printf ("gpio_pinmode: pin stack error!\n");
	
	pinMode (pin, mode);
	return (0);
}
