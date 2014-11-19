/****************************************************************************
*
* Filename: exe_vector_arr.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  vector array functions
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2012
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

#if OS_AROS
    #define LOCK();              LockMutex (global[VECTOR_START].mutex); LockMutex (global[VECTOR_END].mutex);
    #define UNLOCK();            UnlockMutex (global[VECTOR_START].mutex); UnlockMutex (global[VECTOR_END].mutex);
#else
    #define LOCK();              pthread_mutex_lock (&global[VECTOR_START].mutex); pthread_mutex_lock (&global[VECTOR_END].mutex);
    #define UNLOCK();            pthread_mutex_unlock (&global[VECTOR_START].mutex); pthread_mutex_unlock (&global[VECTOR_END].mutex);
#endif

extern struct vm_mem vm_mem;


U1 exe_vadd_l (struct varlist *varlist, struct varlist *global, S4 reg1, S8 reg2, S4 reg3, S4 threadnum)
{
    S8 *q_memptr_src, *q_memptr_dst;
    S8 i;
    S8 start, end;
	S8 err;
	
    if (varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
    /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        q_memptr_src = (S8 *) get_vmvar (varlist, reg1, i, VM_READ);
        q_memptr_dst = (S8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *q_memptr_dst = *q_memptr_src + reg2;
    }
    
    return (TRUE);
}

U1 exe_vadd_d (struct varlist *varlist, struct varlist *global, S4 reg1, F8 reg2, S4 reg3, S4 threadnum)
{
    F8 *d_memptr_src, *d_memptr_dst;
    S8 i;
	S8 start, end;
    S8 err;
	
    if (varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
    /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        d_memptr_src = (F8 *) get_vmvar (varlist, reg1, i, VM_READ);
        d_memptr_dst = (F8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *d_memptr_dst = *d_memptr_src + reg2;
    }
    
    return (TRUE);
}
 
 
U1 exe_vsub_l (struct varlist *varlist, struct varlist *global, S4 reg1, S8 reg2, S4 reg3, S4 threadnum)
{
    S8 *q_memptr_src, *q_memptr_dst;
    S8 i;
	S8 start, end;
    S8 err;
	
    if (varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
    /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        q_memptr_src = (S8 *) get_vmvar (varlist, reg1, i, VM_READ);
        q_memptr_dst = (S8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *q_memptr_dst = *q_memptr_src - reg2;
    }
    
    return (TRUE);
}

U1 exe_vsub_d (struct varlist *varlist, struct varlist *global, S4 reg1, F8 reg2, S4 reg3, S4 threadnum)
{
    F8 *d_memptr_src, *d_memptr_dst;
    S8 i;
	S8 start, end;
    S8 err;
	
    if (varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
   /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        d_memptr_src = (F8 *) get_vmvar (varlist, reg1, i, VM_READ);
        d_memptr_dst = (F8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *d_memptr_dst = *d_memptr_src - reg2;
    }
    
    return (TRUE);
}


U1 exe_vmul_l (struct varlist *varlist, struct varlist *global, S4 reg1, S8 reg2, S4 reg3, S4 threadnum)
{
    S8 *q_memptr_src, *q_memptr_dst;
    S8 i;
	S8 start, end;
    S8 err;
	
    if (varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
    /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        q_memptr_src = (S8 *) get_vmvar (varlist, reg1, i, VM_READ);
        q_memptr_dst = (S8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *q_memptr_dst = *q_memptr_src * reg2;
    }
    
    return (TRUE);
}

U1 exe_vmul_d (struct varlist *varlist, struct varlist *global, S4 reg1, F8 reg2, S4 reg3, S4 threadnum)
{
    F8 *d_memptr_src, *d_memptr_dst;
    S8 i;
	S8 start, end;
    S8 err;
	
    if (varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
   /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        d_memptr_src = (F8 *) get_vmvar (varlist, reg1, i, VM_READ);
        d_memptr_dst = (F8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *d_memptr_dst = *d_memptr_src * reg2;
    }
    
    return (TRUE);
}


U1 exe_vdiv_l (struct varlist *varlist, struct varlist *global, S4 reg1, S8 reg2, S4 reg3, S4 threadnum)
{
    S8 *q_memptr_src, *q_memptr_dst;
    S8 i;
	S8 start, end;
    S8 err;
	
    if (varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
	/* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
   
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        q_memptr_src = (S8 *) get_vmvar (varlist, reg1, i, VM_READ);
        q_memptr_dst = (S8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *q_memptr_dst = *q_memptr_src / reg2;
    }
    
    return (TRUE);
}

U1 exe_vdiv_d (struct varlist *varlist, struct varlist *global, S4 reg1, F8 reg2, S4 reg3, S4 threadnum)
{
    F8 *d_memptr_src, *d_memptr_dst;
    S8 i;
	S8 start, end;
    S8 err;
	
    if (varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
    /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        d_memptr_src = (F8 *) get_vmvar (varlist, reg1, i, VM_READ);
        d_memptr_dst = (F8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *d_memptr_dst = *d_memptr_src / reg2;
    }
    
    return (TRUE);
}


/* two array opcodes ========================================================== */

U1 exe_vadd2_l (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum)
{
    S8 *q_memptr_src1, *q_memptr_src2, *q_memptr_dst;
    S8 i;
	S8 start, end;
    S8 err;
	
    if (varlist[reg1].size != varlist[reg2].size && varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
   /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        q_memptr_src1 = (S8 *) get_vmvar (varlist, reg1, i, VM_READ);
        q_memptr_src2 = (S8 *) get_vmvar (varlist, reg2, i, VM_READ);
        q_memptr_dst = (S8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *q_memptr_dst = *q_memptr_src1 + *q_memptr_src2;
    }
    
    return (TRUE);
}

U1 exe_vadd2_d (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum)
{
    F8 *d_memptr_src1, *d_memptr_src2, *d_memptr_dst;
    S8 i;
    S8 start, end;
	S8 err;
	
    if (varlist[reg1].size != varlist[reg2].size && varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
    /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        d_memptr_src1 = (F8 *) get_vmvar (varlist, reg1, i, VM_READ);
        d_memptr_src2 = (F8 *) get_vmvar (varlist, reg2, i, VM_READ);
        d_memptr_dst = (F8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *d_memptr_dst = *d_memptr_src1 + *d_memptr_src2;
    }
    
    return (TRUE);
}

U1 exe_vsub2_l (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum)
{
    S8 *q_memptr_src1, *q_memptr_src2, *q_memptr_dst;
    S8 i;
    S8 start, end;
	S8 err;
	
    if (varlist[reg1].size != varlist[reg2].size && varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
   /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        q_memptr_src1 = (S8 *) get_vmvar (varlist, reg1, i, VM_READ);
        q_memptr_src2 = (S8 *) get_vmvar (varlist, reg2, i, VM_READ);
        q_memptr_dst = (S8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *q_memptr_dst = *q_memptr_src1 - *q_memptr_src2;
    }
    
    return (TRUE);
}

U1 exe_vsub2_d (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum)
{
    F8 *d_memptr_src1, *d_memptr_src2, *d_memptr_dst;
    S8 i;
    S8 start, end;
	S8 err;
	
    if (varlist[reg1].size != varlist[reg2].size && varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
   /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        d_memptr_src1 = (F8 *) get_vmvar (varlist, reg1, i, VM_READ);
        d_memptr_src2 = (F8 *) get_vmvar (varlist, reg2, i, VM_READ);
        d_memptr_dst = (F8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *d_memptr_dst = *d_memptr_src1 + *d_memptr_src2;
    }
    
    return (TRUE);
}

U1 exe_vmul2_l (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum)
{
    S8 *q_memptr_src1, *q_memptr_src2, *q_memptr_dst;
    S8 i;
	S8 start, end;
    S8 err;
	
    if (varlist[reg1].size != varlist[reg2].size && varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
    /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        q_memptr_src1 = (S8 *) get_vmvar (varlist, reg1, i, VM_READ);
        q_memptr_src2 = (S8 *) get_vmvar (varlist, reg2, i, VM_READ);
        q_memptr_dst = (S8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *q_memptr_dst = *q_memptr_src1 * *q_memptr_src2;
    }
    
    return (TRUE);
}

U1 exe_vmul2_d (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum)
{
    F8 *d_memptr_src1, *d_memptr_src2, *d_memptr_dst;
    S8 i;
	S8 start, end;
    S8 err;
	
    if (varlist[reg1].size != varlist[reg2].size && varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
    /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        d_memptr_src1 = (F8 *) get_vmvar (varlist, reg1, i, VM_READ);
        d_memptr_src2 = (F8 *) get_vmvar (varlist, reg2, i, VM_READ);
        d_memptr_dst = (F8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *d_memptr_dst = *d_memptr_src1 * *d_memptr_src2;
    }
    
    return (TRUE);
}

U1 exe_vdiv2_l (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum)
{
    S8 *q_memptr_src1, *q_memptr_src2, *q_memptr_dst;
    S8 i;
	S8 start, end;
    S8 err;
	
    if (varlist[reg1].size != varlist[reg2].size && varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
    /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
    
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        q_memptr_src1 = (S8 *) get_vmvar (varlist, reg1, i, VM_READ);
        q_memptr_src2 = (S8 *) get_vmvar (varlist, reg2, i, VM_READ);
        q_memptr_dst = (S8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *q_memptr_dst = *q_memptr_src1 / *q_memptr_src2;
    }
    
    return (TRUE);
}

U1 exe_vdiv2_d (struct varlist *varlist, struct varlist *global, S4 reg1, S4 reg2, S4 reg3, S4 threadnum)
{
    F8 *d_memptr_src1, *d_memptr_src2, *d_memptr_dst;
    S8 i;
	S8 start, end;
    S8 err;
	
    if (varlist[reg1].size != varlist[reg2].size && varlist[reg1].size != varlist[reg3].size)
    {
        return (FALSE);
    }
    
    /* take start and end element from stack */
        
    err = exe_stpull_l (threadnum, &end);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
	err = exe_stpull_l (threadnum, &start);
	if (err != ERR_STACK_OK)
	{
		return (FALSE);
	}
	
    if (start < 0)
	{
		return (FALSE);
	}
	
	if (end >= varlist[reg1].size)
	{
		return (FALSE);
	}
    
    for (i = start; i <= end; i++)
    {
        d_memptr_src1 = (F8 *) get_vmvar (varlist, reg1, i, VM_READ);
        d_memptr_src2 = (F8 *) get_vmvar (varlist, reg2, i, VM_READ);
        d_memptr_dst = (F8 *) get_vmvar (varlist, reg3, i, VM_WRITE);
        
        *d_memptr_dst = *d_memptr_src1 / *d_memptr_src2;
    }
    
    return (TRUE);
}