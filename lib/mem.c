/****************************************************************************
*
* Filename: mem.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  memory functions
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

extern struct memblock memblock[];
extern struct t_var t_var;
extern struct plist *plist;
extern struct jumplist *jumplist;
extern struct vmreg vmreg_vm;
extern struct vm_mem vm_mem;
extern struct varlist *varlist;
extern struct varlist *pvarlist_obj;

extern S4 plist_ind;
extern S4 maxplist;
extern S4 maxcclist;
extern S4 plist_size;
extern S4 **cclist;

extern S4 cclist_size;
extern S4 plist_size;

extern S2 includes_ind;
extern struct includes *includes;

extern struct file file[];

extern struct pthreads pthreads[MAXPTHREADS];

#if DEBUG
    extern U1 print_debug;
#endif

#define ALLOCVAR();   if (! init_var (varlist, varlist_state)) return (FALSE); if (! exe_alloc_var (varlist, varlist_state->varlist_ind, t_var.type)) return (FALSE);

#if OS_AROS
    extern struct Library *ThreadBase;
#endif

int init_varlist (struct varlist *varlist, S4 init_maxvarlist)
{
    /* set all mem-pointers to NULL */

    S4 i;

    for (i = 0; i <= init_maxvarlist; i++)
    {
        varlist[i].i_m = NULL;
        varlist[i].li_m = NULL;
        varlist[i].q_m = NULL;
        varlist[i].d_m = NULL;
        varlist[i].s_m = NULL;
        varlist[i].i_vm = NULL;
        varlist[i].li_vm = NULL;
        varlist[i].q_vm = NULL;
        varlist[i].d_vm = NULL;
        varlist[i].s_vm = NULL;
        varlist[i].size = 0;
        varlist[i].dims = NODIMS;
        varlist[i].vm = FALSE;
        varlist[i].type = UNDEF_VAR;
		varlist[i].hs_sock = -1;
        strcpy (varlist[i].name, "");


        /* INITIALIZE VARIABLE MUTEX-STATE */

        #if OS_AROS
            varlist[i].mutex = CreateMutex ();
            if (varlist[i].mutex == NULL)
            {
                return (1);
            }
        #else
            pthread_mutex_init (&varlist[i].mutex, NULL);
        #endif
    }
    return (0);
}

void init_plist (S4 init_maxplist)
{
    /* set all mem-pointers to NULL */

    S4 i;

    for (i = 0; i <= init_maxplist; i++)
    {
        plist[i].memptr = NULL;
		plist[i].line_len = 0;
    }
}

#if DEBUG
void debug_varlist (struct varlist *varlist, S4 maxind)
{
    U1 type;
    S4 i;

    printf ("debug varlist:\n");
    printf ("maxvarlist %li\n", maxind);

    for (i = 0; i <= maxind; i++)
    {
        printf ("var: %li  '%s'\n", i, varlist[i].name);
        type = varlist[i].type;
        switch (type)
        {
            case INT_VAR:
                if (varlist[i].i_m != NULL)
                {
                    printf ("int-var: %li  '%s'  =  %i\n", i, varlist[i].name, *varlist[i].i_m);
                }
                break;

            case LINT_VAR:
                if (varlist[i].li_m != NULL)
                {
                    printf ("lint-var: %li  '%s'  =  %li\n", i, varlist[i].name, *varlist[i].li_m);
                }
                break;

            case QINT_VAR:
                if (varlist[i].q_m != NULL)
                {
                    printf ("qint-var: %li  '%s'  =  %li\n", i, varlist[i].name, *varlist[i].q_m);
                }
                break;

            case DOUBLE_VAR:
                if (varlist[i].d_m != NULL)
                {
                    printf ("d-var: %li  '%s'  =  %.10lf\n", i, varlist[i].name, *varlist[i].d_m);
                }
                break;

            case STRING_VAR:
                if (varlist[i].s_m != NULL)
                {
                    printf ("s-var: %li  '%s'  =  '%s'\n", i, varlist[i].name, varlist[i].s_m);
                }
                break;

            case BYTE_VAR:
                if (varlist[i].s_m != NULL)
                {
                    printf ("b-var: %li  '%s'  =  '%c'\n", i, varlist[i].name, *varlist[i].s_m);
                }
                break;
                
            default:
                printf ("unknown var: %li   type: %li\n", i, type);
        }
    }
}

void debug_exelist (void)
{
    S4 i;

    printf ("\n\ndebug exelist:\n");
    printf ("maxcclist: %li\n", maxcclist);

    for (i = 0; i <= maxcclist; i++)
    {
        printf ("opcode: %li: %li, r1: %li,  r2: %li,  r3: %li\n", i, cclist[i][0], cclist[i][1], cclist[i][2], cclist[i][3]);
    }
}
#endif


U1 init_var (struct varlist *varlist, struct varlist_state *varlist_state)
{
    S4 i;

    if (varlist_state->varlist_ind < varlist_state->varlist_size)
    {
        varlist_state->varlist_ind++;
        varlist_state->maxvarlist = varlist_state->varlist_ind;

        #if DEBUG
            printf ("init_var: varlist_ind: '%li'\n", varlist_state->varlist_ind);
        #endif
    }
    else
    {
        printerr (VARLIST, plist_ind, ST_PRE, t_var.varname);
        return (FALSE);
    }

    varlist[varlist_state->varlist_ind].type = t_var.type;
    strcpy (varlist[varlist_state->varlist_ind].name, t_var.varname);

    varlist[varlist_state->varlist_ind].dims = t_var.dims;

    if (t_var.dims != NODIMS)
    {
        /* array variable: save indexes of the dimension variables */

        for (i = 0; i <= t_var.dims; i++)
        {
            varlist[varlist_state->varlist_ind].dimens[i] = t_var.dimens[i];
        }
    }
    return (TRUE);
}

U1 init_int_var (struct varlist *varlist, struct varlist_state *varlist_state)
{
    /* initalize internal-vars */

    S4 i;

    strcpy (t_var.varname, "");
    t_var.dims = NODIMS;

    t_var.type = INT_VAR;
    for (i = 0; i <= 1; i++)
    {
        ALLOCVAR();
    }

    t_var.type = LINT_VAR;
    ALLOCVAR();

    *varlist[LINT_TMP_1].li_m = MAXSTRING_VAR + 1;

    t_var.type = STRING_VAR;
    t_var.dims = 0;
    t_var.dimens[0] = LINT_TMP_1;

    ALLOCVAR();

    strcpy (varlist[STRING_TMP_1].s_m, "");

    t_var.type = STRING_VAR;
    t_var.dims = 0;
    t_var.dimens[0] = LINT_TMP_1;

    ALLOCVAR();

    strcpy (varlist[STRING_TMP_2].s_m, "");

    t_var.dims = NODIMS;
    t_var.type = DOUBLE_VAR;
    for (i = 0; i <= 1; i++)
    {
        ALLOCVAR();
    }

    /* _arrayind, for debugging */

    t_var.type = LINT_VAR;
    strcpy (t_var.varname, LINT_TMP_2_SB);
    ALLOCVAR();

    /* _break, for break-handling */

    t_var.type = INT_VAR;
    strcpy (t_var.varname, BREAK_SB);
    ALLOCVAR();

    *varlist[BREAK].i_m = TRUE;         /* break allowed */

    /* _timer, for ton, toff */

    t_var.type = QINT_VAR;
    strcpy (t_var.varname, TIMER_SB);
    ALLOCVAR();

    t_var.type = LINT_VAR;
    strcpy (t_var.varname, TIMER_TICKS_SB);
    ALLOCVAR();

    *varlist[TIMER_TICKS].li_m = NANO_TIMER_TICKS;

    /* variable sizes */

    t_var.type = INT_VAR;
    strcpy (t_var.varname, INTSIZE_SB);
    ALLOCVAR();

    *varlist[INTSIZE].i_m = sizeof (S2);

    t_var.type = INT_VAR;
    strcpy (t_var.varname, LINTSIZE_SB);
    ALLOCVAR();

    *varlist[LINTSIZE].i_m = sizeof (S4);

    t_var.type = INT_VAR;
    strcpy (t_var.varname, DOUBLESIZE_SB);
    ALLOCVAR();

    *varlist[DOUBLESIZE].i_m = sizeof (F8);

    t_var.type = INT_VAR;
    strcpy (t_var.varname, MACHINE_SB);
    ALLOCVAR();

    *varlist[MACHINE].i_m = MACHINE_TYPE;

    t_var.type = INT_VAR;
    strcpy (t_var.varname, ERR_ALLOC_SB);
    ALLOCVAR();

    *varlist[ERR_ALLOC].i_m = FALSE;

    t_var.type = INT_VAR;
    strcpy (t_var.varname, ERR_FILE_SB);
    ALLOCVAR();

    *varlist[ERR_FILE].i_m = FALSE;

    t_var.type = INT_VAR;
    strcpy (t_var.varname, TIME_HOUR_SB);
    ALLOCVAR();

    t_var.type = INT_VAR;
    strcpy (t_var.varname, TIME_MIN_SB);
    ALLOCVAR();

    t_var.type = INT_VAR;
    strcpy (t_var.varname, TIME_SEC_SB);
    ALLOCVAR();

    t_var.type = INT_VAR;
    strcpy (t_var.varname, DATE_YEAR_SB);
    ALLOCVAR();

    t_var.type = INT_VAR;
    strcpy (t_var.varname, DATE_MONTH_SB);
    ALLOCVAR();

    t_var.type = INT_VAR;
    strcpy (t_var.varname, DATE_DAY_SB);
    ALLOCVAR();

    t_var.type = INT_VAR;
    strcpy (t_var.varname, DATE_WDAY_SB);
    ALLOCVAR();

    t_var.type = INT_VAR;
    strcpy (t_var.varname, DATE_YDAY_SB);
    ALLOCVAR();

    t_var.type = INT_VAR;
    strcpy (t_var.varname, VERSION_SB);
    ALLOCVAR();

    *varlist[VERSION].i_m = VERSION_NUM;

    t_var.type = INT_VAR;
    strcpy (t_var.varname, VMREGS_SB);
    ALLOCVAR();

    *varlist[VMREGS].i_m = MAXVMREG;

    /* _language */
    t_var.type = STRING_VAR;
    t_var.dims = 0;
    t_var.dimens[0] = LINT_TMP_1;
    strcpy (t_var.varname, LANGUAGE_SB);

    ALLOCVAR();

    /* _fnewline */
    t_var.type = STRING_VAR;
    t_var.dims = 0;
    t_var.dimens[0] = LINT_TMP_1;
    strcpy (t_var.varname, FILE_NEWLINE_SB);

    ALLOCVAR();

    strcpy (varlist[FILE_NEWLINE].s_m, NEWLINE_SB);

    /* _fendian */
    t_var.type = INT_VAR;
    t_var.dims = NODIMS;
    strcpy (t_var.varname, FILE_ENDIAN_SB);
    ALLOCVAR();

    *varlist[FILE_ENDIAN].i_m = BS_LITTLE_ENDIAN;

    /* _os */
    t_var.type = INT_VAR;
    t_var.dims = NODIMS;
    strcpy (t_var.varname, OS_SB);
    ALLOCVAR();

    *varlist[OS].i_m = OS_TYPE;
    
    
    t_var.type = INT_VAR;
    t_var.dims = NODIMS;
    strcpy (t_var.varname, QINTSIZE_SB);
    ALLOCVAR();

    *varlist[QINTSIZE].i_m = sizeof (S8);
    
    /* _endian (machine) */
    t_var.type = INT_VAR;
    t_var.dims = NODIMS;
    strcpy (t_var.varname, ENDIAN_SB);
    ALLOCVAR();

    *varlist[ENDIAN].i_m = BS_LITTLE_ENDIAN;
	
	
	/* nanopath = nanoroot */
	t_var.type = STRING_VAR;
    t_var.dims = 0;
    t_var.dimens[0] = LINT_TMP_1;
    
	strcpy (t_var.varname, NANOVM_PATH_SB);

    ALLOCVAR();
	
	#if OS_ANDROID
		strcpy (varlist[NANOVM_PATH].s_m, "/sdcard/nanovm/");	
	#else
		/* check ENV variable */
		
		if (getenv (NANOVM_ROOT_SB) == NULL)
		{
			/* ENV not set use defaults */
			
			if (OS_AMIGA || OS_AROS)
			{
				strcpy (varlist[NANOVM_PATH].s_m, "Work:nanovm/");
			}

			if (OS_LINUX)
			{
				strcpy (varlist[NANOVM_PATH].s_m, "~/nanovm/");
			}
            
			if (OS_WINDOWS)
			{
				strcpy (varlist[NANOVM_PATH].s_m, "C:/nanovm/");
			}
		}
		else
		{
			/* use ENV variable setting */
			strcpy (varlist[NANOVM_PATH].s_m, getenv (NANOVM_ROOT_SB));
			strcat (varlist[NANOVM_PATH].s_m, "/");
		}
	#endif
	
	t_var.type = QINT_VAR;
    t_var.dims = NODIMS;
    strcpy (t_var.varname, VECTOR_START_SB);
    ALLOCVAR();

    *varlist[VECTOR_START].q_m = 0;
	
	t_var.type = QINT_VAR;
    t_var.dims = NODIMS;
    strcpy (t_var.varname, VECTOR_END_SB);
    ALLOCVAR();

    *varlist[VECTOR_END].q_m = 0;

    return (TRUE);
}

U1 memblock_gonext (void)
{
    if (vm_mem.mblock_ind < MAXMEMBLOCK - 1)
    {
        vm_mem.mblock_ind++;
        memblock[vm_mem.mblock_ind].memptr = (U1 *) malloc (vmreg_vm.l[MEMBSIZE] * sizeof (U1));
        if (memblock[vm_mem.mblock_ind].memptr == NULL)
        {
            return (FALSE);
        }
        else
        {
            memblock[vm_mem.mblock_ind].size = vmreg_vm.l[MEMBSIZE];
            memblock[vm_mem.mblock_ind].nextpos = 0;
            return (TRUE);
        }
    }
    else
    {
        return (FALSE);
    }
}

#if OS_ANDROID

void *get_memblock (S2 msize)
{
    U1 *ptr;
    S4 nextpos, size;

	if (msize == 8)
	{
		/* do malloc to get right data alignment */
		
		ptr = (S8 *) malloc (sizeof (S8));
		return (ptr);
	}
	else
	{
		nextpos = memblock[vm_mem.mblock_ind].nextpos;
		size = memblock[vm_mem.mblock_ind].size;

		if (size - nextpos + 1 >= msize)
		{
			ptr = memblock[vm_mem.mblock_ind].memptr + nextpos;
			nextpos = nextpos + msize;
		}
		else
		{
			if (! memblock_gonext ())
			{
				ptr = NULL;
				return (ptr);
			}
			else
			{
				ptr = memblock[vm_mem.mblock_ind].memptr + nextpos;
				nextpos = nextpos + msize;
			}
		}
		memblock[vm_mem.mblock_ind].nextpos = nextpos;
		return (ptr);
	}
}


#else

void *get_memblock (S2 msize)
{
    U1 *ptr;
    S4 nextpos, size;

    nextpos = memblock[vm_mem.mblock_ind].nextpos;
    size = memblock[vm_mem.mblock_ind].size;

    if (size - nextpos + 1 >= msize)
    {
        ptr = memblock[vm_mem.mblock_ind].memptr + nextpos;
        nextpos = nextpos + msize;
    }
    else
    {
        if (! memblock_gonext ())
        {
            ptr = NULL;
            return (ptr);
        }
        else
        {
            ptr = memblock[vm_mem.mblock_ind].memptr + nextpos;
            nextpos = nextpos + msize;
        }
    }
    memblock[vm_mem.mblock_ind].nextpos = nextpos;
    return (ptr);
}

#endif

/* changed to more compact code */

U1 exe_alloc_var (struct varlist *varlist, S4 ind, S4 type)
{
    U1 nomem = FALSE;
    S4 dim_ind;
	S8 i, varsize = 1, dim ALIGN;
    struct dynamic_array *dyn;

    varlist[ind].vm = FALSE;

    if (varlist[ind].dims != NODIMS)
    {
        /* a[x][y][z] size = x * y * z  */
        /* multiply all dimensions */

        for (i = 0; i <= varlist[ind].dims; i++)
        {
            dim_ind = varlist[ind].dimens[i];

            switch (varlist[dim_ind].type)
            {
                case INT_VAR:
                    dim = (S8) *(varlist)[dim_ind].i_m;
                    varsize = varsize * dim;
                    break;

                case LINT_VAR:
                    dim = (S8) *(varlist)[dim_ind].li_m;
                    varsize = varsize * dim;
                    break;

                case QINT_VAR:
                    dim = (S8) *(varlist)[dim_ind].q_m;
                    varsize = varsize * dim;
                    break;
            }
        }
    }

    /* allocating */

    if (varlist[ind].size == 0)
    {
        if (varlist[ind].dims != NODIMS)
        {
            /* array variable */

            switch (type)
            {
                case INT_VAR:
                    varlist[ind].i_m = (S2 *) malloc (varsize * sizeof (S2));
                    if (varlist[ind].i_m == NULL)
                    {
                        nomem = TRUE;
                    }
                    break;

                case LINT_VAR:
                    varlist[ind].li_m = (S4 *) malloc (varsize * sizeof (S4));
                    if (varlist[ind].li_m == NULL)
                    {
                        nomem = TRUE;
                    }
                    break;

                case QINT_VAR:
                    varlist[ind].q_m = (S8 *) malloc (varsize * sizeof (S8));
                    if (varlist[ind].q_m == NULL)
                    {
                        nomem = TRUE;
                    }
                    break;

                case DOUBLE_VAR:
                    varlist[ind].d_m = (F8 *) malloc (varsize * sizeof (F8));
                    if (varlist[ind].d_m == NULL)
                    {
                        nomem = TRUE;
                    }
                    break;

                case STRING_VAR:
                case BYTE_VAR:
                    varlist[ind].s_m = (U1 *) malloc (varsize * sizeof (U1));
                    if (varlist[ind].s_m == NULL)
                    {
                        nomem = TRUE;
                    }
                    break;

                case DYNAMIC_VAR:
                    varlist[ind].s_m = (U1 *) malloc (varsize * sizeof (struct dynamic_array));
                    if (varlist[ind].s_m == NULL)
                    {
                        nomem = TRUE;
                    }
                    else
                    {
                        /* initialize dynamic array */

                        dyn = (struct dynamic_array *) varlist[ind].s_m;

                        for (i = 0; i < varsize; i++)
                        {
                            dyn[i].memptr = NULL;
                        }
                    }
                    break;
            }

            if (nomem)
            {
                if (type != STRING_VAR && vmreg_vm.l[VMUSE])
                {
                    /* virtual memory */

                    if (! vm_mem.init)
                    {
                        if (! init_vmfile ())
                        {
                            return (FALSE);
                        }
                    }

                    /* allocate vm cache and check space in swapfile */
					// U1 vm_alloc_pages (struct varlist *varlist, S4 vind, S8 vmcachesize, U1 type)
                    switch (type)
                    {
                        case INT_VAR:
							if (vm_alloc_pages (varlist, ind, vmreg_vm.l[VMCACHESIZE], type) == 0)
							{
								return (FALSE);
							}

                            varlist[ind].vm_pages.vm_ind = get_vmblock (varsize * sizeof (S2));
                            if (varlist[ind].vm_pages.vm_ind == -1)
                            {
                                /* vm-file full */

                                return (FALSE);
                            }
                            break;

                        case LINT_VAR:
							if (vm_alloc_pages (varlist, ind, vmreg_vm.l[VMCACHESIZE], type) == 0)
							{
								return (FALSE);
							}
							
                            varlist[ind].vm_pages.vm_ind = get_vmblock (varsize * sizeof (S4));
                            if (varlist[ind].vm_pages.vm_ind == -1)
                            {
                                /* vm-file full */

                                return (FALSE);
                            }
                            break;

                        case QINT_VAR:
							if (vm_alloc_pages (varlist, ind, vmreg_vm.l[VMCACHESIZE], type) == 0)
							{
								return (FALSE);
							}

                            varlist[ind].vm_pages.vm_ind = get_vmblock (varsize * sizeof (S8));
                            if (varlist[ind].vm_pages.vm_ind == -1)
                            {
                                /* vm-file full */

                                return (FALSE);
                            }
                            break;

                        case DOUBLE_VAR:
							if (vm_alloc_pages (varlist, ind, vmreg_vm.l[VMCACHESIZE], type) == 0)
							{
								return (FALSE);
							}

                            varlist[ind].vm_pages.vm_ind = get_vmblock (varsize * sizeof (F8));
                            if (varlist[ind].vm_pages.vm_ind == -1)
                            {
                                /* vm-file full */

                                return (FALSE);
                            }
                            break;

                        case BYTE_VAR:
							if (vm_alloc_pages (varlist, ind, vmreg_vm.l[VMCACHESIZE], type) == 0)
							{
								return (FALSE);
							}

                            varlist[ind].vm_pages.vm_ind = get_vmblock (varsize * sizeof (U1));
                            if (varlist[ind].vm_pages.vm_ind == -1)
                            {
                                /* vm-file full */

                                return (FALSE);
                            }
                            break;
                    }

                    varlist[ind].vm = TRUE;
                    varlist[ind].vmcachesize = vmreg_vm.l[VMCACHESIZE];
                }
                else
                {
                    return (FALSE);
                }
            }
        }
        else
        {
            /* normal variable */

            switch (type)
            {
                case INT_VAR:
                    varlist[ind].i_m = (S2 *) get_memblock (sizeof (S2));
                    if (varlist[ind].i_m == NULL)
                    {
                        return (FALSE);
                    }
                    break;

                case LINT_VAR:
                    varlist[ind].li_m = (S4 *) get_memblock (sizeof (S4));
                    if (varlist[ind].li_m == NULL)
                    {
                        return (FALSE);
                    }
                    break;

                case QINT_VAR:
                    varlist[ind].q_m = (S8 *) get_memblock (sizeof (S8));
                    if (varlist[ind].q_m == NULL)
                    {
                        return (FALSE);
                    }
                    break;

                case DOUBLE_VAR:
                    varlist[ind].d_m = (F8 *) get_memblock (sizeof (F8));
                    if (varlist[ind].d_m == NULL)
                    {
                        return (FALSE);
                    }
                    break;

                case STRING_VAR:
                case BYTE_VAR:
                    varlist[ind].s_m = (U1 *) get_memblock (sizeof (U1));
                    if (varlist[ind].s_m == NULL)
                    {
                        return (FALSE);
                    }
                    break;
            }
        }

        varlist[ind].size = varsize;

        #if DEBUG
            if (print_debug)
            {
                printf ("exe_alloc: allocated\n");
            }
        #endif
    }

    return (TRUE);
}


#if OS_AMIGA

S4 **alloc_array_lint (size_t x, size_t y)
{
    S4 **array = NULL, i, alloc;
    U1 err = FALSE;

    array = (S4 **) malloc (x * sizeof (*array));
    if (array != NULL)
    {
        for (i = 0; i < x; i++)
        {
            array[i] = (S4 *) malloc (y * sizeof (*array));
            if (array[i] == NULL)
            {
                alloc = i - 1;
                err = TRUE;
                break;
            }
        }
        if (err)
        {
            if (alloc >= 0)
            {
                for (i = 0; i <= alloc; i++)
                {
                    free (array[i]);
                }
            }
            free (array);
            array = NULL;
        }
    }
    return (array);
}

void dealloc_array_lint (S4 **array, size_t x)
{
    S4 i;

    for (i = 0; i < x; i++)
    {
        free (array[i]);
    }
    free (array);
}

#else

void dealloc_array_lint (S4** array, size_t n_rows)
{
    size_t c;

    assert (array || n_rows == 0);

    for (c = 0; c != n_rows; ++c)
    {
        free (array[c]);
    }
    free (array);
}

S4** alloc_array_lint (size_t n_rows, size_t n_columns)
{
    size_t c;
    S4 **array;

    assert (n_columns > 0);

    if (n_rows == 0) { return 0; }

    array = (S4**) (calloc (n_rows, sizeof (S4*)));
    if (!array)
    {
        errno = ENOMEM;
        return 0;
    }

    for (c = 0; c != n_rows; ++c)
    {
        array[c] = (S4*) (calloc (n_columns, sizeof (S4)));
        if (!array[c])
        {
            free (array[c]);
            errno = ENOMEM;
            return 0;
        }
    }

    return array;
}

#endif


void dealloc_memblocks (void)
{
    /* remove all memblocks (nodim vars) from ram */

    S8 i;

    for (i = 0; i <= vm_mem.mblock_ind; i++)
    {
        free (memblock[i].memptr);
    }
}

U1 exe_dealloc_var (struct varlist *varlist, S4 ind)
{
    U1 type = varlist[ind].type;
    S4 i;
    struct dynamic_array *dyn;

    if (varlist[ind].size != 0 && varlist[ind].dims != NODIMS)
    {
        switch (type)
        {
            case INT_VAR:
                if (varlist[ind].i_m != NULL)
                {
                    free (varlist[ind].i_m);
                    varlist[ind].i_m = NULL;
                }
                
                vm_free_pages (varlist, ind);
                break;

            case LINT_VAR:
                if (varlist[ind].li_m != NULL)
                {
                    free (varlist[ind].li_m);
                    varlist[ind].li_m = NULL;
                }
                
                vm_free_pages (varlist, ind);
                break;

            case QINT_VAR:
                if (varlist[ind].q_m != NULL)
                {
                    free (varlist[ind].q_m);
                    varlist[ind].q_m = NULL;
                }
               
				vm_free_pages (varlist, ind);
                break;

            case DOUBLE_VAR:
                if (varlist[ind].d_m != NULL)
                {
                    free (varlist[ind].d_m);
                    varlist[ind].d_m = NULL;
                }
                
                vm_free_pages (varlist, ind);
                break;

            case STRING_VAR:
                if (varlist[ind].s_m != NULL)
                {
                    free (varlist[ind].s_m);
                    varlist[ind].s_m = NULL;
                }
                break;

            case BYTE_VAR:
                if (varlist[ind].s_m != NULL)
                {
                    free (varlist[ind].s_m);
                    varlist[ind].s_m = NULL;
                }
                
                vm_free_pages (varlist, ind);
                break;

            case DYNAMIC_VAR:
                if (varlist[ind].s_m != NULL)
                {
                    dyn = (struct dynamic_array *) varlist[ind].s_m;

                    for (i = 0; i < varlist[ind].size; i++)
                    {
                        if (dyn[i].memptr)
                        {
                            free (dyn[i].memptr);
                            dyn[i].memptr = NULL;
                        }
                    }
                    
                    free (varlist[ind].s_m);
                    varlist[ind].s_m = NULL;
                }
                break;
                
                /* no virtual memory support for now!!!! */
        }

        varlist[ind].size = 0;

        #if DEBUG
            if (print_debug)
            {
                printf ("exe_dealloc: deallocated\n");
            }
        #endif

        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}

void exe_dealloc_varlist (struct varlist *varlist)
{
    /* remove all variables from memory */

    S4 i;

    if (vm_mem.obj_maxvarlist > -1)
    {
        for (i = vm_mem.obj_maxvarlist; i >= 0; i--)
        {
            exe_dealloc_var (varlist, i);
        }
    }
    dealloc_memblocks ();
    free (varlist);
}

void exe_dealloc_pvarlist (struct varlist *pvarlist)
{
    /* remove all thread private variables from memory */

    S4 i;

    if (vm_mem.obj_maxpvarlist > -1)
    {
        for (i = vm_mem.obj_maxpvarlist; i >= 0; i--)
        {
            exe_dealloc_var (pvarlist, i);
        }
    }
    free (pvarlist);
}

void exe_dealloc_plist (void)
{
    /* remove all script-lines from memory */

    S4 i;

    if (plist_size > -1)
    {
        /* assembler */

        maxplist = plist_size - 1;
    }

    if (maxplist > -1)
    {
        for (i = maxplist; i >= 0; i--)
        {
            if (plist[i].memptr != NULL)
            {
                free (plist[i].memptr);
            }
        }
        free (plist);
    }
}

void exe_dealloc_includes (void)
{
    S4 i;

    if (includes != NULL)
    {
        for (i = 0; i <= includes_ind; i++)
        {
            if (includes[i].name != NULL)
            {
                /* valgrind says here is a memory leak, but I didn't get it */
                /* whats wrong here? */

                free (includes[i].name);
            }
        }

        free (includes);
    }
}

U1 init_files (void)
{
    S4 i;

    for (i = 0; i < MAXFILES; i++)
    {
        strcpy (file[i].name, "");
        file[i].status = FILECLOSED;
        file[i].fptr = NULL;
        file[i].buf = NULL;
        /* allocate file read buffer */

        file[i].bufsize = FILEBUFSIZE;
        file[i].buf = (U1 *) malloc (file[i].bufsize * sizeof (U1));
        if (file[i].buf == NULL)
        {
            return (FALSE);
        }
    }
    return (TRUE);
}

void exe_dealloc_files (void)
{
    S4 i;

    for (i = 0; i < MAXFILES; i++)
    {
        if (file[i].buf != NULL) free (file[i].buf);
    }
}

void dealloc_stack (void)
{
    S4 i;

    for (i = 0; i < MAXPTHREADS; i++)
    {
        if (pthreads[i].stack_elements >= 0)
        {
            pthreads[i].stack_elements = -1;
            free (pthreads[i].stack);
        }    
    }
    
}


void free_memory (void)
{
    S2 err;

    exe_dealloc_pvarlist (pvarlist_obj);
    exe_dealloc_varlist (varlist);
    exe_dealloc_plist ();
    exe_dealloc_includes ();
    exe_dealloc_files ();

    if (cclist != NULL)
    {
        if (cclist_size > -1)
        {
            /* assembler */

            if (cclist != NULL) dealloc_array_lint (cclist, cclist_size);
        }
        else
        {
            /* vm */

            if (cclist != NULL) dealloc_array_lint (cclist, maxcclist + 1);
        }
    }

    if (jumplist != NULL) free (jumplist);
    if (vm_mem.init)
    {
        fclose (vm_mem.file);
        err = remove (vm_mem.filename);
        if (err < 0)
        {
            printerr (VM_FILE, NOTDEF, ST_EXE, VM_FILE_SB);
        }
    }

    /* disabled if OS_AROS: got error memory freed twice if enabled!!! */

    #if ! OS_AROS
        dealloc_stack ();
    #endif
    
    #if OS_AROS
        CloseLibrary ((struct Library *) ThreadBase);
    #endif
}

void exe_shutdown (S2 value)
{
    free_memory ();
    exit (value);
}
