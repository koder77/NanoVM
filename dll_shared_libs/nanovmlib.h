/****************************************************************************
*
* Filename: nanovmlib.h
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  function protos for Nano VM library
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

/* arr.c */

U1 let_array2_int (struct varlist *varlist, struct vm_mem *vm_mem, S4 reg1, S8 reg2, S8 *reg3);
U1 let_array2_lint (struct varlist *varlist, struct vm_mem *vm_mem, S4 reg1, S8 reg2, S8 *reg3);
U1 let_array2_qint (struct varlist *varlist, struct vm_mem *vm_mem, S4 reg1, S8 reg2, S8 *reg3);
U1 let_array2_double (struct varlist *varlist, struct vm_mem *vm_mem, S4 reg1, S8 reg2, F8 *reg3);
U1 let_array2_byte (struct varlist *varlist, struct vm_mem *vm_mem, S4 reg1, S8 reg2, S8 *reg3);
U1 let_array2_string (struct varlist *varlist, struct vm_mem *vm_mem, S4 reg1, S8 reg2, U1 *s_reg3);

U1 let_2array_int (struct varlist *varlist, struct vm_mem *vm_mem, S8 reg1, S4 reg2, S8 reg3);
U1 let_2array_lint (struct varlist *varlist, struct vm_mem *vm_mem, S8 reg1, S4 reg2, S8 reg3);
U1 let_2array_qint (struct varlist *varlist, struct vm_mem *vm_mem, S8 reg1, S4 reg2, S8 reg3);
U1 let_2array_double (struct varlist *varlist, struct vm_mem *vm_mem, F8 reg1, S4 reg2, S8 reg3);
U1 let_2array_byte (struct varlist *varlist, struct vm_mem *vm_mem, S8 reg1, S4 reg2, S8 reg3);
U1 let_2array_string (struct varlist *varlist, struct vm_mem *vm_mem, U1 *s_reg1, S4 reg2, S8 reg3);


/* nanovmlib.c */

U1 stpush_l (S4 threadnum, struct pthreads *pthreads, S8 stacksize, S8 reg);
U1 stpush_d (S4 threadnum, struct pthreads *pthreads, S8 stacksize, F8 reg);
U1 stpush_s (S4 threadnum, struct pthreads *pthreads, S8 stacksize, U1 *s_reg);

U1 stpull_l (S4 threadnum, struct pthreads *pthreads, S8 stacksize, S8 *reg);
U1 stpull_d (S4 threadnum, struct pthreads *pthreads, S8 stacksize, F8 *reg);
U1 stpull_s (S4 threadnum, struct pthreads *pthreads, S8 stacksize, U1 *s_reg);

U1 stgettype (S4 threadnum, struct pthreads *pthreads, S8 *reg);
U1 stelements (S4 threadnum, struct pthreads *pthreads, S8 *reg);
