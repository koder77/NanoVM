/****************************************************************************
*
* Filename: exe_main.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  threaded engine
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2011
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
#include "arch.h"

#if OS_AROS
    #include <proto/dos.h>
#endif

extern struct varlist *varlist;
extern struct varlist *pvarlist_obj;
extern S4 **cclist;
extern struct jumplist jumplist[];
extern struct shell_args shell_args;

extern S4 maxcclist;
extern U1 filename[MAXSTRING_VAR + 1];

extern struct pthreads pthreads[MAXPTHREADS];

extern struct vmreg vmreg_vm;
struct t_var t_var;

extern struct vm_mem vm_mem;

#if OS_AROS
    extern void *cclist_mutex;
    extern void *pthreads_mutex;
#else
    extern pthread_mutex_t cclist_mutex;
    extern pthread_mutex_t pthreads_mutex;
#endif

#if DEBUG
    #define EXE_NEXT();  if (rinzler() != 0) { printf ("EPOS: %i, OPCODE: %i\n", epos, pcclist[epos][0]); printerr (OVERFLOW_IND, epos, ST_EXE, ""); } check_code (threadnum, maxcclist, pcclist); goto *dthread[++epos];
#else
   // #define EXE_NEXT(); goto *jumpt[pcclist[++epos][0]];
#define EXE_NEXT(); goto *dthread[++epos];
#endif

#define ERREXIT();   goto eexit;

#define ARG1  pcclist[epos][1]
#define ARG2  pcclist[epos][2]
#define ARG3  pcclist[epos][3]

#if DEBUG
    extern U1 print_debug;
    #define PRINTD(s);  if (print_debug) { printf ("%s\n", s); }
#else
    #define PRINTD(s);
#endif

extern U1 jit_on;		/* CLI -j option to turn jit compiler on */

/* hyperspace */
extern S4 hsserver;

#if OS_AROS
    extern uint32_t hs_thread;
#else
    extern pthread_t hs_thread;
#endif

extern struct rights rights;



/* AsmJit */

extern S4 JIT_code_ind;
extern U1 JIT_error;



void exe_break (void)
{
    /* break - handling
    *
    * if user answer is 'y', the engine will shutdown
    *
    */

    U1 answ[2];

    if (*varlist[BREAK].i_m != 0)
    {
        printf ("\nexe: break,  exit now (y/n)? ");
        scanf ("%1s", answ);

        if (strcmp (answ, "y") == 0 || strcmp (answ, "Y") == 0)
        {
            exe_shutdown (WARN);
        }
    }
}

#if OS_AROS

S2 exe_elist_trampoline (S4 threadnum)
{
    int rc = FALSE;

	SocketBase = (struct Library *) OpenLibrary ("bsdsocket.library", 3);
    if (SocketBase == NULL)
    {
        return (FALSE);
    }

    if (SocketBaseTags (SBTM_SETVAL (SBTC_ERRNOPTR (sizeof (errno))), (IPTR) &errno, TAG_DONE ))
    {
		CloseLibrary(SocketBase);
		SocketBase = NULL;
        return (FALSE);
    }

    rc = exe_elist(threadnum);

	CloseLibrary(SocketBase);
	SocketBase = NULL;
	
    return rc;
}

#endif

struct timespec time_diff (struct timespec start, struct timespec end);

struct timespec time_diff (struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

#if OS_LINUX
void set_sched_priority()
{
    int rc;
    struct sched_param sched_param;
    int  policy;

    policy = SCHED_BATCH;

    /** highest priority **/
    sched_param.sched_priority = 0;

    /** with chrt -m or sched_get_priority_max, I get to know the priority range **/

    rc = pthread_setschedparam (pthread_self(), policy, &sched_param);

    if (rc != 0)
    {
        printf ("ERROR: set sched_param!\n");
    }
}
#endif

void *exe_elist (S4 threadnum)
{
    /* execution part
    * runs the program in cclist
    *
    * all memory allocations are done at runtime!
    *
    * May the ThReAd be with you!
    */

    S4 i, j;

    S4 slen, size1, size3;
    S4 epos;                        /* execution position */
    S2 exit_retval = WARN;

    U1 error_inter = FALSE;         /* interrupts */

    U1 ansi_b = FALSE;              /* ANSI - Sequences (for console output only (shell)) */
    U1 ansi_i = FALSE;
    U1 ansi_u = FALSE;

    S4 **pcclist = NULL;             /* local pcclist */

    /* addresses for direct threading dispatch */
    void **dthread = NULL; 
    
    
    struct timespec timerstart;
	struct timespec timerend;
	struct timespec timerun;
    S8 timer_S8;
	
	/* Windows */
	clock_t timeron = 0, timeroff = 0;      /* timer stuff, used by ton, toff */
    F8 timer, timerfac;
	
	
    struct vmreg vmreg;
    struct varlist *pvarlist;

    /* set vm sizes in registers */
    vmreg.l[MEMBSIZE] = vmreg_vm.l[MEMBSIZE];
    vmreg.l[VMBSIZE] = vmreg_vm.l[VMBSIZE];
    vmreg.l[VMCACHESIZE] = vmreg_vm.l[VMCACHESIZE];
    vmreg.l[VMUSE] = vmreg_vm.l[VMUSE];

    vmreg.l[ERR_ALLOC_R] = TRUE;
    vmreg.l[ERR_PROCESS_R] = TRUE;
    vmreg.l[ERR_FILE_R] = 0;
    vmreg.l[ERR_SOCK_R] = 0;

    /* set stack size in register */
    vmreg.l[STSIZE] = vmreg_vm.l[STSIZE];

    /* for POINTER opcode */
    S2 popcode, poffset = 0, pmaxoffset = 4;
    
    /* jumptable, opcodes must be in the right order!
     * don't change anything...
     *
     * see include/global_d.h, include/opcodes.h for more
     */
	
	
	
    static void *jumpt[] =
    {
        &&push_i, &&push_l, &&push_q, &&push_d, &&push_b,
        &&pull_i, &&pull_l, &&pull_q, &&pull_d, &&pull_b,
        &&move_l, &&move_d,
        &&inc_l, &&inc_d, &&dec_l, &&dec_d,
        &&add_l, &&add_d, &&sub_l, &&sub_d, &&mul_l, &&mul_d, &&div_l, &&div_d, &&smul_l, &&sdiv_l,
        &&and_l, &&or_l, &&band_l, &&bor_l, &&bxor_l, &&mod_l, &&equal_l, &&equal_d, &&not_equal_l, &&not_equal_d,
        &&greater_l, &&greater_d, &&less_l, &&less_d,
        &&greater_or_eq_l, &&greater_or_eq_d,
        &&less_or_eq_l, &&less_or_eq_d,
        &&let_array2_i, &&let_array2_l, &&let_array2_q, &&let_array2_d, &&let_array2_b, &&let_array2_s,
        &&let_2array_i, &&let_2array_l, &&let_2array_q, &&let_2array_d, &&let_2array_b, &&let_2array_s,
        &&print_l, &&print_d, &&print_s, &&print_newline, &&print_space, &&print_cls, &&print_ascii, &&print_bold,
        &&print_italic, &&print_underline, &&print_normal, &&print_varname,
        &&locate, &&input_l, &&input_d, &&input_s, &&inputch_l, &&inputch_d, &&inputch_s,
        &&move_s, &&move_p2s, &&move_s2p, &&add_s,
        &&strlen, &&strleft, &&strright, &&ucase, &&lcase, &&vchar, &&asc, &&equalstr, &&not_equalstr,
        &&jmp_l, &&jsr_l, &&jmp, &&jsr, &&rts,
        &&eq_jmp_l, &&neq_jmp_l, &&gr_jmp_l, &&ls_jmp_l, &&greq_jmp_l, &&lseq_jmp_l,
        &&eq_jsr_l, &&neq_jsr_l, &&gr_jsr_l, &&ls_jsr_l, &&greq_jsr_l, &&lseq_jsr_l,
        &&curson, &&cursoff, &&cursleft, &&cursright, &&cursup, &&cursdown,
        &&timeron, &&timeroff, &&time, &&wait_sec, &&wait_tick,
        &&file_open, &&file_close,
        &&file_read_b, &&file_read_ab, &&file_read_i, &&file_read_l, &&file_read_q, &&file_read_d, &&file_read_s, &&file_read_ls,
        &&file_write_b, &&file_write_ab, &&file_write_i, &&file_write_l, &&file_write_q, &&file_write_d, &&file_write_s,
        &&file_write_sl, &&file_write_sd, &&file_write_n, &&file_write_sp,
        &&file_setpos, &&file_getpos, &&file_rewind, &&file_size, &&file_remove, &&file_rename,
        &&socket_sopen, &&socket_sopen_act, &&socket_copen,
        &&socket_sclose, &&socket_sclose_act, &&socket_cclose,
        &&clientaddr, &&hostname, &&hostbyname, &&hostbyaddr,
        &&socket_read_b, &&socket_read_ab, &&socket_read_i, &&socket_read_l, &&socket_read_q, &&socket_read_d, &&socket_read_s, &&socket_read_ls,
        &&socket_write_b, &&socket_write_ab, &&socket_write_i, &&socket_write_l, &&socket_write_q, &&socket_write_d, &&socket_write_s,
        &&socket_write_sl, &&socket_write_sd, &&socket_write_n, &&socket_write_sp,
        &&vsizeof, &&vmvar,
        &&str2val_l, &&str2val_d, &&val2str_l, &&val2str_d, &&double2int, &&int2double,
        &&srand_l, &&rand_l, &&rand_d,
        &&abs_l, &&abs_d, &&ceil_d, &&floor_d, &&exp_d, &&log_d, &&log10_d, &&pow_d, &&sqrt_d,
        &&cos_d, &&sin_d, &&tan_d, &&acos_d, &&asin_d, &&atan_d, &&cosh_d, &&sinh_d, &&tanh_d,
        &&run_process, &&run_shell, &&wait_process,
        &&argnum, &&argstr,

        &&stpush_l, &&stpush_d, &&stpush_s, &&stpush_all_l, &&stpush_all_d,
        &&stpull_l, &&stpull_d, &&stpull_s, &&stpull_all_l, &&stpull_all_d,
        &&ston,

        &&show_stack, &&stgettype, &&stelements, &&gettype,

        &&alloc, &&dealloc, &&getaddress, &&pointer, &&nop, &&pexit, &&eexit,
        &&inc_ls_jmp_l, &&inc_lseq_jmp_l, &&dec_gr_jmp_l, &&dec_greq_jmp_l,
        &&inc_ls_jsr_l, &&inc_lseq_jsr_l, &&dec_gr_jsr_l, &&dec_greq_jsr_l,
        &&hash32_ab, &&stack_set,
        &&push_s, &&pull_s, &&stpush_all_s, &&stpull_all_s, &&not_l,
        &&var_lock, &&var_unlock, &&thread_create, &&thread_join, &&thread_state, &&thread_num, &&thread_sync,
        &&thpush_l, &&thpush_d, &&thpush_s, &&thpush_sync, &&thpull_sync, &&thread_exitcode,
        &&palloc, &&pdealloc,
        &&ppush_i, &&ppush_l, &&ppush_q, &&ppush_d, &&ppush_b, &&ppush_s,
        &&ppull_i, &&ppull_l, &&ppull_q, &&ppull_d, &&ppull_b, &&ppull_s,
        &&pmove_a_i, &&pmove_a_l, &&pmove_a_q, &&pmove_a_d, &&pmove_a_b, &&pmove_a_s,
        &&pmove_i_a, &&pmove_l_a, &&pmove_q_a, &&pmove_d_a, &&pmove_b_a, &&pmove_s_a,
        &&pfread_ab, &&pfwrite_ab, &&psread_ab, &&pswrite_ab,
        &&int2hex, &&phash32_ab, &&hash64_ab, &&phash64_ab,
        &&move_adyn_q, &&move_adyn_d, &&move_adyn_s,
        &&move_q_adyn, &&move_d_adyn, &&move_s_adyn,
        &&pmove_adyn_q, &&pmove_adyn_d, &&pmove_adyn_s,
        &&pmove_q_adyn, &&pmove_d_adyn, &&pmove_s_adyn,
        &&dyngettype, &&pdyngettype,
        
        &&vadd_l, &&vadd_d, &&vsub_l, &&vsub_d, &&vmul_l, &&vmul_d, &&vdiv_l, &&vdiv_d,
        &&vadd2_l, &&vadd2_d, &&vsub2_l, &&vsub2_d, &&vmul2_l, &&vmul2_d, &&vdiv2_l, &&vdiv2_d,
        
        &&pvadd_l, &&pvadd_d, &&pvsub_l, &&pvsub_d, &&pvmul_l, &&pvmul_d, &&pvdiv_l, &&pvdiv_d,
        &&pvadd2_l, &&pvadd2_d, &&pvsub2_l, &&pvsub2_d, &&pvmul2_l, &&pvmul2_d, &&pvdiv2_l, &&pvdiv2_d,
        &&atan2_d, &&ppointer, &&pgettype,
        
        &&move_ip_b, &&move_lp_b, &&move_qp_b, &&move_dp_b,
        &&move_b_ip, &&move_b_lp, &&move_b_qp, &&move_b_dp,
        &&pmove_ip_b, &&pmove_lp_b, &&pmove_qp_b, &&pmove_dp_b,
        &&pmove_b_ip, &&pmove_b_lp, &&pmove_b_qp, &&pmove_b_dp,
        &&password, &&makedir,
        &&mod_d,
		&&hsvar, &&hsserver, &&hsload, &&hssave,
		&&stpush_all_all, &&stpull_all_all,
		&&jit, &&run_jit,
		&&dobjects, &&dnames, &&pdnames,
		&&lopen, &&lclose, &&lfunc, &&lcall
    };
    
#if OS_LINUX
    set_sched_priority();
#endif
    
	PRINTD("exe_elist: releasing rinzler...\n");
	
    #if DEBUG
        /* PROTECT CONSTANT VARIABLES (rinzler.c) */
        rinzler ();
    #endif

    PRINTD("exe_elist: initializing...\n");
    
    /* initialize string registers */
    
    for (i = 0; i < MAXVMREG; i++)
    {
        vmreg.s[i][0] = BINUL;
    }
    
        
    set_thread_running (threadnum);

    pthreads[threadnum].jumpreclist_ind = -1;

    /* copy global cclist to local pcclist */

    pcclist = (S4 **) alloc_array_lint (maxcclist + 1, MAXCCOMMARG);
    if (pcclist == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "");
        return (FALSE);
    }


    for (i = 0; i <= maxcclist; i++)
    {
        for (j = 0; j < MAXCCOMMARG; j++)
        {
            pcclist[i][j] = cclist[i][j];
        }
    }

    /* alloc direct threading list */
    dthread = calloc ((maxcclist + 1), sizeof (*dthread));
    if (dthread == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "error: can't allocate dthread memory!");
        return (FALSE);
    }
    
    /* copy opcodes to direct threading list */
    for (i = 0; i <= maxcclist; i++)
    {
        dthread[i] = jumpt[pcclist[i][0]];
    }
    
    
    
    /* copy global thread private varlist to local private varlist */

    /* alloc thread private varlist master -------------------------------- */

    pvarlist = (struct varlist *) malloc ((vm_mem.obj_maxpvarlist + 1) * sizeof (struct varlist));
    if (pvarlist == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "");
        return (FALSE);
    }

    #if DEBUG
        if (print_debug)
        {
            printf ("init_pvarlist: maxpvarlist: %li\n", vm_mem.obj_maxpvarlist);
        }
    #endif

    if (init_varlist (pvarlist, vm_mem.obj_maxpvarlist) != 0)
    {
        printf ("error: can't init varlist!\n");
        return (FALSE);
    }

    for (i = 0; i <= vm_mem.obj_maxpvarlist; i++)
    {
        pvarlist[i].type = pvarlist_obj[i].type;
        strcpy (pvarlist[i].name, pvarlist_obj[i].name);

        pvarlist[i].dims = pvarlist_obj[i].dims;

        if (pvarlist_obj[i].dims != NODIMS)
        {
            for (j = 0; j <= pvarlist_obj[i].dims; j++)
            {
                pvarlist[i].dimens[j] = pvarlist_obj[i].dimens[j];
            }
        }

        if (pvarlist_obj[i].constant == CONST_VAR)
        {
            /* variable is constant, allocate */

            #if DEBUG
                if (print_debug)
                {
                    printf ("allocating pvar: %li\n", i);
                }
            #endif

            if (! exe_alloc_var (pvarlist, i, pvarlist[i].type))
            {
                printerr (MEMORY, NOTDEF, ST_PRE, "");
                return (FALSE);
            }

            switch (pvarlist[i].type)
            {
                case INT_VAR:
                    *pvarlist[i].i_m = *pvarlist_obj[i].i_m;
                    break;

                case LINT_VAR:
                    *pvarlist[i].li_m = *pvarlist_obj[i].li_m;
                    break;
					
				case QINT_VAR:
                    *pvarlist[i].q_m = *pvarlist_obj[i].q_m;
                    break;
					
                case DOUBLE_VAR:
                    *pvarlist[i].d_m = *pvarlist_obj[i].d_m;
                    break;

                case STRING_VAR:
                    strcpy (pvarlist[i].s_m, pvarlist_obj[i].s_m);
                    break;

                case BYTE_VAR:
                    *pvarlist[i].s_m = *pvarlist_obj[i].s_m;
                    break;
            }
        }
    }


    epos = pthreads[threadnum].startpos;

    /* break-handling (Ctrl-C) */

    signal (SIGINT, (void *) exe_break);


    /* run the threaded bytecode */

    PRINTD("exe_elist: running...");

    EXE_NEXT();


    /* PUSH --------------------------------------------------------- */

    push_i:
        PRINTD("PUSH_I");

        vmreg.l[ARG2] = (S8) *varlist[ARG1].i_m;

        EXE_NEXT();

    push_l:
        PRINTD("PUSH_L");

        vmreg.l[ARG2] = (S8) *varlist[ARG1].li_m;

        EXE_NEXT();

    push_q:
        PRINTD("PUSH_Q");

        vmreg.l[ARG2] = *varlist[ARG1].q_m;

        EXE_NEXT();

    push_d:
        PRINTD("PUSH_D");

        vmreg.d[ARG2] = *varlist[ARG1].d_m;

        EXE_NEXT();

    push_b:
        PRINTD("PUSH_B");

        vmreg.l[ARG2] = (S8) varlist[ARG1].s_m[0];

        EXE_NEXT();


    /* PULL --------------------------------------------------------- */

    pull_i:
        PRINTD("PULL_I");

        *varlist[ARG2].i_m = (S2) vmreg.l[ARG1];

        EXE_NEXT();

    pull_l:
        PRINTD("PULL_L");

        *varlist[ARG2].li_m = (S4) vmreg.l[ARG1];

        EXE_NEXT();

    pull_q:
        PRINTD("PULL_Q");
        
        *varlist[ARG2].q_m = vmreg.l[ARG1];

        EXE_NEXT();

    pull_d:
        PRINTD("PULL_D");

        *varlist[ARG2].d_m = vmreg.d[ARG1];

        EXE_NEXT();

    pull_b:
        PRINTD("PULL_B");

        varlist[ARG2].s_m[0] = (U1) vmreg.l[ARG1];

        EXE_NEXT();


    /* MOVE_L ------------------------------------------------------- */

    move_l:
        PRINTD("MOVE_L");

        vmreg.l[ARG2] = vmreg.l[ARG1];

        EXE_NEXT();


    /* MOVE_D ------------------------------------------------------- */

    move_d:
        PRINTD("MOVE_D");

        vmreg.d[ARG2] = vmreg.d[ARG1];

        EXE_NEXT();


    /* INC ---------------------------------------------------------- */

    inc_l:
        PRINTD("INC_L");

        vmreg.l[ARG1]++;

        EXE_NEXT();

    inc_d:
        PRINTD("INC_D");

        vmreg.d[ARG1]++;

        EXE_NEXT();


    /* DEC ---------------------------------------------------------- */

    dec_l:
        PRINTD("DEC_L");

        vmreg.l[ARG1]--;

        EXE_NEXT();

    dec_d:
        PRINTD("DEC_D");

        vmreg.d[ARG1]--;

        EXE_NEXT();


    /* ADD ---------------------------------------------------------- */

    add_l:
        PRINTD("ADD_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] + vmreg.l[ARG2];

        EXE_NEXT();

    add_d:
        PRINTD("ADD_D");

        vmreg.d[ARG3] = vmreg.d[ARG1] + vmreg.d[ARG2];

        EXE_NEXT();


    /* SUB ---------------------------------------------------------- */

    sub_l:
        PRINTD("SUB_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] - vmreg.l[ARG2];

        EXE_NEXT();

    sub_d:
        PRINTD("SUB_D");

        vmreg.d[ARG3] = vmreg.d[ARG1] - vmreg.d[ARG2];

        EXE_NEXT();


    /* MUL ---------------------------------------------------------- */

    mul_l:
        PRINTD("MUL_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] * vmreg.l[ARG2];

        EXE_NEXT();

    mul_d:
        PRINTD("MUL_D");

        vmreg.d[ARG3] = vmreg.d[ARG1] * vmreg.d[ARG2];

        EXE_NEXT();


    /* DIV ---------------------------------------------------------- */

    div_l:
        PRINTD("DIV_L");

        if (vmreg.l[ARG2] == 0)
        {
            printerr (DIV_BY_ZERO, epos, ST_EXE, "");
            ERREXIT();
        }

        vmreg.l[ARG3] = vmreg.l[ARG1] / vmreg.l[ARG2];

        EXE_NEXT();

    div_d:
        PRINTD("DIV_D");

        if (vmreg.d[ARG2] == 0)
        {
            printerr (DIV_BY_ZERO, epos, ST_EXE, "");
            ERREXIT();
        }

        vmreg.d[ARG3] = vmreg.d[ARG1] / vmreg.d[ARG2];

        EXE_NEXT();


    /* SMUL --------------------------------------------------------- */

    smul_l:
        PRINTD("SMUL_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] << vmreg.l[ARG2];

        EXE_NEXT();


    /* SDIV --------------------------------------------------------- */

    sdiv_l:
        PRINTD("SDIV_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] >> vmreg.l[ARG2];

        EXE_NEXT();


    /* AND ---------------------------------------------------------- */

    and_l:
        PRINTD("AND_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] && vmreg.l[ARG2];

        EXE_NEXT();


    /* OR ----------------------------------------------------------- */

    or_l:
        PRINTD("OR_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] || vmreg.l[ARG2];

        EXE_NEXT();


    /* BAND --------------------------------------------------------- */

    band_l:
        PRINTD("BAND_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] & vmreg.l[ARG2];

        EXE_NEXT();


    /* BOR ---------------------------------------------------------- */

    bor_l:
        PRINTD("BOR_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] | vmreg.l[ARG2];

        EXE_NEXT();


    /* BXOR --------------------------------------------------------- */

    bxor_l:
        PRINTD("BXOR_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] ^ vmreg.l[ARG2];

        EXE_NEXT();


    /* MOD ---------------------------------------------------------- */

    mod_l:
        PRINTD("MOD_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] % vmreg.l[ARG2];

        EXE_NEXT();


    /* EQUAL -------------------------------------------------------- */

    equal_l:
        PRINTD("EQUAL_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] == vmreg.l[ARG2];

        EXE_NEXT();

    equal_d:
        PRINTD("EQUAL_D");

        vmreg.l[ARG3] = vmreg.d[ARG1] == vmreg.d[ARG2];

        EXE_NEXT();


    /* NOT EQUAL ---------------------------------------------------- */

    not_equal_l:
        PRINTD("NOT_EQUAL_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] != vmreg.l[ARG2];

        EXE_NEXT();

    not_equal_d:
        PRINTD("NOT_EQUAL_D");

        vmreg.l[ARG3] = vmreg.d[ARG1] != vmreg.d[ARG2];

        EXE_NEXT();


    /* GREATER ------------------------------------------------------ */

    greater_l:
        PRINTD("GREATER_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] > vmreg.l[ARG2];

        EXE_NEXT();

    greater_d:
        PRINTD("GREATER_D");

        vmreg.l[ARG3] = vmreg.d[ARG1] > vmreg.d[ARG2];

        EXE_NEXT();


    /* LESS --------------------------------------------------------- */

    less_l:
        PRINTD("LESS_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] < vmreg.l[ARG2];

        EXE_NEXT();

    less_d:
        PRINTD("LESS_D");

        vmreg.l[ARG3] = vmreg.d[ARG1] < vmreg.d[ARG2];

        EXE_NEXT();


    /* GREATER OR EQUAL --------------------------------------------- */

    greater_or_eq_l:
        PRINTD("GREATER_OR_EQ_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] >= vmreg.l[ARG2];

        EXE_NEXT();

    greater_or_eq_d:
        PRINTD("GREATER_OR_EQ_D");

        vmreg.l[ARG3] = vmreg.d[ARG1] >= vmreg.d[ARG2];

        EXE_NEXT();


    /* LESS OR EQUAL ------------------------------------------------ */

    less_or_eq_l:
        PRINTD("LESS_OR_EQ_L");

        vmreg.l[ARG3] = vmreg.l[ARG1] <= vmreg.l[ARG2];

        EXE_NEXT();

    less_or_eq_d:
        PRINTD("LESS_OR_EQ_D");

        vmreg.l[ARG3] = vmreg.d[ARG1] <= vmreg.d[ARG2];

        EXE_NEXT();


    /* LET_ARRAY2 --------------------------------------------------- */

    let_array2_i:
        PRINTD("LET_ARRAY2_I");

        if (! exe_let_array2_int (varlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    let_array2_l:
        PRINTD("LET_ARRAY2_L");

        if (! exe_let_array2_lint (varlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    let_array2_q:
        PRINTD("LET_ARRAY2_Q");

        if (! exe_let_array2_qint (varlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    let_array2_d:
        PRINTD("LET_ARRAY_2_D");

        if (! exe_let_array2_double (varlist, ARG1, vmreg.l[ARG2], &vmreg.d[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    let_array2_b:
        PRINTD("LET_ARRAY_2_B");

        if (! exe_let_array2_byte (varlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    let_array2_s:
        PRINTD("LET_ARRAY_2_S");

        if (! exe_let_array2_string (varlist, ARG1, vmreg.l[ARG2], (U1 *) &vmreg.s[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();


    /* LET_2ARRAY --------------------------------------------------- */

    let_2array_i:
        PRINTD("LET_2ARRAY_I");

        if (! exe_let_2array_int (varlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    let_2array_l:
        PRINTD("LET_2ARRAY_L");

        if (! exe_let_2array_lint (varlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    let_2array_q:
        PRINTD("LET_2ARRAY_Q");

        if (! exe_let_2array_qint (varlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();

    let_2array_d:
        PRINTD("LET_2ARRAY_D");

        if (! exe_let_2array_double (varlist, vmreg.d[ARG1], ARG2, vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    let_2array_b:
        PRINTD("LET_2ARRAY_B");

        if (! exe_let_2array_byte (varlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    let_2array_s:
        PRINTD("LET_2ARRAY_S");

        if (! exe_let_2array_string (varlist, vmreg.s[ARG1], ARG2, vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();


    /* PRINT -------------------------------------------------------- */

    print_l:
        PRINTD("PRINT_L");

        printf ("%lli", vmreg.l[ARG1]);
        fflush (stdout);

        EXE_NEXT();

    print_d:
        PRINTD("PRINT_D");

        printf ("%.10lf", vmreg.d[ARG1]);
        fflush (stdout);

        EXE_NEXT();

    print_s:
        PRINTD("PRINT_S");

        printf ("%s", vmreg.s[ARG1]);
        fflush (stdout);

        EXE_NEXT();


    /* PRINT NEWLINE ------------------------------------------------ */

    print_newline:
        PRINTD("PRINT_NEWLINE");

        if (ARG1 < 0)
        {
            j = 1;
        }
        else
        {
            j = vmreg.l[ARG1];
        }

        for (i = j; i > 0; i--)
        {
            printf ("\n");
        }

        EXE_NEXT();


    /* PRINT SPACE -------------------------------------------------- */

    print_space:
        PRINTD("PRINT_SPACE");

        if (ARG1 < 0)
        {
            j = 1;
        }
        else
        {
            j = vmreg.l[ARG1];
        }

        for (i = j; i > 0; i--)
        {
            printf (" ");
        }
        fflush (stdout);

        EXE_NEXT();


    /* PRINT CLS ---------------------------------------------------- */

    print_cls:
        PRINTD("PRINT_CLS");

        printf ("\x1B[2J");

        EXE_NEXT();


    /* PRINT ASCII -------------------------------------------------- */

    print_ascii:
        PRINTD("PRINT_ASCII");

        printf ("%c", (NINT) vmreg.l[ARG1]);
        fflush (stdout);

        EXE_NEXT();


    /* PRINT BOLD --------------------------------------------------- */

    print_bold:
        PRINTD("PRINT_BOLD");

        if (! ansi_b)
        {
            printf ("\x1B[1m");
            ansi_b = TRUE;
        }
        else
        {
            printf ("\x1B[22m");
            ansi_b = FALSE;
        }
        fflush (stdout);

        EXE_NEXT();


    /* PRINT ITALIC ------------------------------------------------- */

    print_italic:
        PRINTD("PRINT_ITALIC");

        if (! ansi_i)
        {
            printf ("\x1B[3m");
            ansi_i = TRUE;
        }
        else
        {
            printf ("\x1B[23m");
            ansi_i = FALSE;
        }
        fflush (stdout);

        EXE_NEXT();


    /* PRINT UNDERLINE ---------------------------------------------- */

    print_underline:
        PRINTD("PRINT_UNDERLINE");

        if (! ansi_u)
        {
            printf ("\x1B[4m");
            ansi_u = TRUE;
        }
        else
        {
            printf ("\x1B[24m");
            ansi_u = FALSE;
        }

        fflush (stdout);
    EXE_NEXT();


    /* PRINT NORMAL ------------------------------------------------- */

    print_normal:
        PRINTD("PRINT_NORMAL");

        printf ("\x1B[0m");
        ansi_b = FALSE, ansi_i = FALSE, ansi_u = FALSE;

        fflush (stdout);

        EXE_NEXT();


    /* PRINT VARNAME ------------------------------------------------ */

    print_varname:
        PRINTD("PRINT_VARNAME");

        printf ("%s", varlist[ARG1].name);
        fflush (stdout);

        EXE_NEXT();


    /* LOCATE ------------------------------------------------------- */

    locate:
        PRINTD("PRINT_LOCATE");

        printf ("\x1B[%i;%iH", (NINT) vmreg.l[ARG1], (NINT) vmreg.l[ARG2]);
        fflush (stdout);

        EXE_NEXT();


    /* INPUT -------------------------------------------------------- */

	input_l:
        PRINTD("INPUT_L");

        if (fgets (varlist[STRING_TMP_1].s_m, MAXSTRING_VAR - 1, stdin) != NULL)
		{
			sscanf(varlist[STRING_TMP_1].s_m, "%lli", &vmreg.l[ARG1]);
		}
		else
		{
			printf ("input_l: can't read!\n");
		}
			
        EXE_NEXT();

    input_d:
        PRINTD("INPUT_D");

        if (fgets (varlist[STRING_TMP_1].s_m, MAXSTRING_VAR - 1, stdin) != NULL)
		{
			sscanf(varlist[STRING_TMP_1].s_m, "%lf", &vmreg.d[ARG1]);
		}
		else
		{
			printf ("input_d: can't read!\n");
		}
		
        EXE_NEXT();

    input_s:
        PRINTD("INPUT_S");

        if (fgets (vmreg.s[ARG1], MAXSTRING_VAR - 1, stdin) != NULL)
		{
			i = strlen (vmreg.s[ARG1]);
			if (i <= MAXSTRING_VAR - 1)
			{
				vmreg.s[ARG1][i - 1] = BINUL;
			}
		}
		else
		{
			printf ("input_s: can't read!\n");
		}

        EXE_NEXT();


    /* INPUTCH ------------------------------------------------------ */

    inputch_l:
        PRINTD("INPUTCH_L");

        vmreg.l[ARG1] = (S8) getc (stdin);

        EXE_NEXT();

    inputch_d:
        PRINTD("INPUTCH_D");

        vmreg.d[ARG1] = (F8) getc (stdin);

        EXE_NEXT();

    inputch_s:
        PRINTD("INPUTCH_S");

        vmreg.s[ARG1][0] = (U1) getc (stdin);
        vmreg.s[ARG1][1] = BINUL;

        /* read the following RETURN (dummy only!) */
        i = getc (stdin);

        EXE_NEXT();


    /* MOVESTR ------------------------------------------------------- */

    move_s:
        PRINTD("MOVE_S");

        strcpy (vmreg.s[ARG2], vmreg.s[ARG1]);

        EXE_NEXT();


    /* MOVEP2STR ----------------------------------------------------- */

    move_p2s:
        PRINTD("MOVE_P2S");

        slen = strlen (vmreg.s[ARG1]);

        if (vmreg.l[ARG2] < 0 || vmreg.l[ARG2] > slen - 1)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        vmreg.s[ARG3][0] = vmreg.s[ARG1][vmreg.l[ARG2]];
        vmreg.s[ARG3][1] = BINUL;

        EXE_NEXT();


    /* MOVESTR2P ----------------------------------------------------- */

    move_s2p:
        PRINTD("MOVE_S2P");

        slen = strlen (vmreg.s[ARG2]);

        size1 = MAXSTRING_VAR + 1;

        if (size1 < 2)
        {
            printerr (OVERFLOW_STR, epos, ST_EXE, varlist[ARG1].name);
            ERREXIT();
        }
        if (vmreg.l[ARG3] < 0 || vmreg.l[ARG3] > slen - 1)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        j = 0;
        for (i = vmreg.l[ARG3]; i <= vmreg.l[ARG3] + size1 - 2; i++)
        {
            vmreg.s[ARG2][i] = vmreg.s[ARG1][j];
            j++;
        }

        EXE_NEXT();


    /* ADDSTR -------------------------------------------------------- */

    add_s:
        PRINTD("ADD_S");

        if (MAXSTRING_VAR + 1 >= strlen (vmreg.s[ARG1]) + strlen (vmreg.s[ARG2]) + 1)
        {
            if (strlen (vmreg.s[ARG1]) < MAXSTRING_VAR)
            {
                strcpy (varlist[STRING_TMP_1].s_m, vmreg.s[ARG1]);
            }
            else
            {
                printerr (OVERFLOW_STR, epos, ST_EXE, "ARG1");
                ERREXIT();
            }

            if (strlen (vmreg.s[ARG2]) < MAXSTRING_VAR)
            {
                strcpy (varlist[STRING_TMP_2].s_m, vmreg.s[ARG2]);
            }
            else
            {
                printerr (OVERFLOW_STR, epos, ST_EXE, "ARG2");
                ERREXIT();
            }

            strcpy (vmreg.s[ARG3], varlist[STRING_TMP_1].s_m);
            strcat (vmreg.s[ARG3], varlist[STRING_TMP_2].s_m);
        }
        else
        {
            printerr (OVERFLOW_STR, epos, ST_EXE, "ARG3");
            ERREXIT();
        }

        EXE_NEXT();


    /* STRLEN ------------------------------------------------------- */

    strlen:
        PRINTD("STRLEN");

        vmreg.l[ARG2] = strlen (vmreg.s[ARG1]);

        EXE_NEXT();


    /* STRLEFT ------------------------------------------------------ */

    strleft:
        PRINTD("STRLEFT");

        slen = strlen (vmreg.s[ARG1]);
        if (vmreg.l[ARG2] > slen || vmreg.l[ARG2] > MAXSTRING_VAR - 1)
        {
            /* number longer as stringlen */

            printerr (OVERFLOW_STR, epos, ST_EXE, "ARG3");
            ERREXIT();
        }

        for (i = 0; i <= vmreg.l[ARG2] - 1; i++)
        {
            vmreg.s[ARG3][i] = vmreg.s[ARG1][i];
        }
        vmreg.s[ARG3][i] = BINUL;

        EXE_NEXT();


    /* STRRIGHT ----------------------------------------------------- */

    strright:
        PRINTD("STRRIGHT");

        slen = strlen (vmreg.s[ARG1]);
        if (vmreg.l[ARG2] > slen || vmreg.l[ARG2] > MAXSTRING_VAR - 1)
        {
            /* number longer as stringlen */

            printerr (OVERFLOW_STR, epos, ST_EXE, "ARG3");
            ERREXIT();
        }

        j = 0;
        for (i = slen - vmreg.l[ARG2]; i <= slen; i++)
        {
            vmreg.s[ARG3][j] = vmreg.s[ARG1][i];
            j++;
        }
        vmreg.s[ARG3][j] = BINUL;

        EXE_NEXT();


    /* UCASE -------------------------------------------------------- */

    ucase:
        PRINTD("UCASE");

        j = strlen (vmreg.s[ARG1]) - 1;
        for (i = 0; i <= j; i++)
        {
            vmreg.s[ARG1][i] = toupper (vmreg.s[ARG1][i]);
        }

        EXE_NEXT();


    /* LCASE -------------------------------------------------------- */

    lcase:
        PRINTD("LCASE");

        j = strlen (vmreg.s[ARG1]) - 1;
        for (i = 0; i <= j; i++)
        {
            vmreg.s[ARG1][i] = tolower (vmreg.s[ARG1][i]);
        }

        EXE_NEXT();


    /* CHAR --------------------------------------------------------- */

    vchar:
        PRINTD("VCHAR");

        if (vmreg.l[ARG1] >= 0 && vmreg.l[ARG1] <= 255)
        {
             vmreg.s[ARG2][0] = (U1) vmreg.l[ARG1];
             vmreg.s[ARG2][1] = BINUL;
        }
        else
        {
            printerr (OVERFLOW_VAR, epos, ST_EXE, "ARG1");
            ERREXIT();
        }

        EXE_NEXT();


    /* ASC ---------------------------------------------------------- */

    asc:
        PRINTD("ASC");

        if (strlen (vmreg.s[ARG1]) > 0)
        {
            vmreg.l[ARG2] = (S4) vmreg.s[ARG1][0];
        }
        else
        {
            printerr (OVERFLOW_VAR, epos, ST_EXE, "ARG1");
            ERREXIT();
        }

        EXE_NEXT();


    /* EQUALSTR ----------------------------------------------------- */

    equalstr:
        PRINTD("EQUALSTR");

        if (strcmp (vmreg.s[ARG1], vmreg.s[ARG2]) == 0)
        {
            vmreg.l[ARG3] = TRUE;
        }
        else
        {
            vmreg.l[ARG3] = FALSE;
        }

        EXE_NEXT();


    /* NOT EQUALSTR ------------------------------------------------- */

    not_equalstr:
        PRINTD("NOT_EQUALSTR");

        if (strcmp (vmreg.s[ARG1], vmreg.s[ARG2]) != 0)
        {
            vmreg.l[ARG3] = TRUE;
        }
        else
        {
            vmreg.l[ARG3] = FALSE;
        }

        EXE_NEXT();


    /* COND JUMP ---------------------------------------------------- */

    jmp_l:
        PRINTD("JMP_L");

        if (vmreg.l[ARG1])
        {
            epos = ARG2 - 1;
        }

        EXE_NEXT();

    jsr_l:
        PRINTD("JSR_L");

        if (vmreg.l[ARG1])
        {
            if (pthreads[threadnum].jumpreclist_ind < MAXJUMPRECLEN - 1)
            {
                pthreads[threadnum].jumpreclist_ind++;

                /* set jumppos for return */

                pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos = epos;

                epos = ARG2 - 1;
            }
            else
            {
                printerr (JUMPRECLIST, epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();


    /* JMP ---------------------------------------------------------- */

    jmp:
        PRINTD("JMP");

        epos = ARG1 - 1;

        EXE_NEXT();


    /* JSR ---------------------------------------------------------- */

    jsr:
        PRINTD("JSR");

        if (pthreads[threadnum].jumpreclist_ind < MAXJUMPRECLEN - 1)
        {
            pthreads[threadnum].jumpreclist_ind++;

            /* set jumppos for return */

            pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos = epos;

            epos = ARG1 - 1;
        }
        else
        {
            printerr (JUMPRECLIST, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();


    /* RTS ---------------------------------------------------------- */

    rts:
        PRINTD("RTS");

        if (pthreads[threadnum].jumpreclist_ind != -1)
        {
            epos = pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos;
            pthreads[threadnum].jumpreclist_ind--;
        }
        else
        {
            printerr (JUMPRECLIST, epos, ST_EXE, "rts without jsr");
            ERREXIT();
        }

        EXE_NEXT();


    /* EQUAL_JMP_L -------------------------------------------------- */

    eq_jmp_l:
        PRINTD("EQ_JMP_L");

        if (vmreg.l[ARG1] == vmreg.l[ARG2])
        {
            epos = ARG3 - 1;
        }

        EXE_NEXT();


    /* NOT_EQUAL_JMP_L ---------------------------------------------- */

    neq_jmp_l:
        PRINTD("NEQ_JMP_L");

        if (vmreg.l[ARG1] != vmreg.l[ARG2])
        {
            epos = ARG3 - 1;
        }

        EXE_NEXT();


    /* GREATER_JMP_L ------------------------------------------------ */

    gr_jmp_l:
        PRINTD("GR_JMP_L");

        if (vmreg.l[ARG1] > vmreg.l[ARG2])
        {
            epos = ARG3 - 1;
        }

        EXE_NEXT();


    /* LESS_JMP_L --------------------------------------------------- */

    ls_jmp_l:
        PRINTD("LS_JMP_L");

        if (vmreg.l[ARG1] < vmreg.l[ARG2])
        {
            epos = ARG3 - 1;
        }

        EXE_NEXT();


    /* GREATER_OR_EQUAL_JMP_L --------------------------------------- */

    greq_jmp_l:
        PRINTD("GREQ_JMP_L");

        if (vmreg.l[ARG1] >= vmreg.l[ARG2])
        {
            epos = ARG3 - 1;
        }

        EXE_NEXT();


    /* LESS_OR_EQUAL_JMP_L ------------------------------------------ */

    lseq_jmp_l:
        PRINTD("LSEQ_JMP_L");

        if (vmreg.l[ARG1] <= vmreg.l[ARG2])
        {
            epos = ARG3 - 1;
        }

        EXE_NEXT();


    /* EQUAL_JSR_L -------------------------------------------------- */

    eq_jsr_l:
        PRINTD("EQ_JSR_L");

        if (vmreg.l[ARG1] == vmreg.l[ARG2])
        {
            if (pthreads[threadnum].jumpreclist_ind < MAXJUMPRECLEN - 1)
            {
                pthreads[threadnum].jumpreclist_ind++;

                /* set jumppos for return */

                pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos = epos;

                epos = ARG3 - 1;
            }
            else
            {
                printerr (JUMPRECLIST, epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();


    /* NOT_EQUAL_JSR_L ---------------------------------------------- */

    neq_jsr_l:
        PRINTD("NEQ_JSR_L");

        if (vmreg.l[ARG1] != vmreg.l[ARG2])
        {
            if (pthreads[threadnum].jumpreclist_ind < MAXJUMPRECLEN - 1)
            {
                pthreads[threadnum].jumpreclist_ind++;

                /* set jumppos for return */

                pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos = epos;

                epos = ARG3 - 1;
            }
            else
            {
                printerr (JUMPRECLIST, epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();


    /* GREATER_JSR_L ------------------------------------------------ */

    gr_jsr_l:
        PRINTD("GR_JSR_L");

        if (vmreg.l[ARG1] > vmreg.l[ARG2])
        {
            if (pthreads[threadnum].jumpreclist_ind < MAXJUMPRECLEN - 1)
            {
                pthreads[threadnum].jumpreclist_ind++;

                /* set jumppos for return */

                pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos = epos;

                epos = ARG3 - 1;
            }
            else
            {
                printerr (JUMPRECLIST, epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();


    /* LESS_JSR_L --------------------------------------------------- */

    ls_jsr_l:
        PRINTD("LS_JSR_L");

        if (vmreg.l[ARG1] < vmreg.l[ARG2])
        {
            if (pthreads[threadnum].jumpreclist_ind < MAXJUMPRECLEN - 1)
            {
                pthreads[threadnum].jumpreclist_ind++;

                /* set jumppos for return */

                pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos = epos;

                epos = ARG3 - 1;
            }
            else
            {
                printerr (JUMPRECLIST, epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();


    /* GREATER_OR_EQ_JSR_L ------------------------------------------ */

    greq_jsr_l:
        PRINTD("GREQ_JSR_L");

        if (vmreg.l[ARG1] >= vmreg.l[ARG2])
        {
            if (pthreads[threadnum].jumpreclist_ind < MAXJUMPRECLEN - 1)
            {
               pthreads[threadnum].jumpreclist_ind ++;

                /* set jumppos for return */

                pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos = epos;

                epos = ARG3 - 1;
            }
            else
            {
                printerr (JUMPRECLIST, epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();


    /* LESS_OR_EQ_JSR_L --------------------------------------------- */

    lseq_jsr_l:
        PRINTD("LSEQ_JSR_L");

        if (vmreg.l[ARG1] <= vmreg.l[ARG2])
        {
            if (pthreads[threadnum].jumpreclist_ind < MAXJUMPRECLEN - 1)
            {
                pthreads[threadnum].jumpreclist_ind++;

                /* set jumppos for return */

                pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos = epos;

                epos = ARG3 - 1;
            }
            else
            {
                printerr (JUMPRECLIST, epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();


    /* CURSON ------------------------------------------------------- */

    curson:
        PRINTD("CURSON");

        printf ("\033[ p");

        EXE_NEXT();


    /* CURSOFF ------------------------------------------------------ */

    cursoff:
        PRINTD("CURSOFF");

        printf ("\033[0 p");

        EXE_NEXT();


    /* CURSLEFT ----------------------------------------------------- */

    cursleft:
        PRINTD("CURSLEFT");

        if (ARG1 < 0)
        {
            j = 1;
        }
        else
        {
            j = vmreg.l[ARG1];
        }

        for (i = j; i > 0; i--)
        {
            printf ("\033[1D");
        }

        EXE_NEXT();


    /* CURSRIGHT ---------------------------------------------------- */

    cursright:
        PRINTD("CURSRIGHT");

        if (ARG1 < 0)
        {
            j = 1;
        }
        else
        {
            j = vmreg.l[ARG1];
        }

        for (i = j; i > 0; i--)
        {
            printf ("\033[1C");
        }

        EXE_NEXT();


    /* CURSUP ------------------------------------------------------- */

    cursup:
        PRINTD("CURSUP");

        if (ARG1 < 0)
        {
            j = 1;
        }
        else
        {
            j = vmreg.l[ARG1];
        }

        for (i = j; i > 0; i--)
        {
            printf ("\033[1A");
        }

        EXE_NEXT();


    /* CURSDOWN ----------------------------------------------------- */

    cursdown:
        PRINTD("CURSDOWN");

        if (ARG1 < 0)
        {
            j = 1;
        }
        else
        {
            j = vmreg.l[ARG1];
        }

        for (i = j; i > 0; i--)
        {
            printf ("\033[1B");
        }

        EXE_NEXT();


    /* TIMERON ------------------------------------------------------ */

    timeron:
        PRINTD("TIMERON");

		#if OS_LINUX
			clock_gettime (CLOCK_THREAD_CPUTIME_ID, &timerstart);
		#endif
		
		#if OS_WINDOWS
			timeron = clock();
		#endif		
        
		EXE_NEXT();


    /* TIMEROFF ----------------------------------------------------- */

    timeroff:
        PRINTD("TIMEROFF");

		#if OS_LINUX
			clock_gettime (CLOCK_THREAD_CPUTIME_ID, &timerend);
        
			timerun = time_diff (timerstart, timerend);
			timer_S8 = ((timerun.tv_sec * 1000000000) + timerun.tv_nsec) / 1000000;	/* calculate ms = milli seconds */
		
			*varlist[TIMER].q_m = timer_S8;
		#endif
		
		#if OS_WINDOWS
			timeroff = clock();

			timerfac = (F8) CLOCKS_PER_SEC / 1000;
			timer = (timeroff - timeron) / timerfac;

			*varlist[TIMER].q_m = (S8) timer;
		#endif
			
        EXE_NEXT();


    /* TIME --------------------------------------------------------- */

    time:
        PRINTD("TIME");

        exe_time ();

        EXE_NEXT();


    /* WAIT --------------------------------------------------------- */

    wait_sec:
        PRINTD("WAIT_SEC");

        EXE_WAIT_SEC();

        EXE_NEXT();

    wait_tick:
        PRINTD("WAIT_TICK");

        EXE_WAIT_TICK();

        EXE_NEXT();


    /* FOPEN -------------------------------------------------------- */

    file_open:
        PRINTD("FILE_OPEN");

        i = exe_fopen (&vmreg.l[ARG1], vmreg.s[ARG2], vmreg.s[ARG3]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();


    /* FCLOSE ------------------------------------------------------- */

    file_close:
        PRINTD("FILE_CLOSE");

        i = exe_fclose (vmreg.l[ARG1]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();


    /* FREAD -------------------------------------------------------- */

    file_read_b:
        PRINTD("FILE_READ_BYTE");

        i = exe_fread_byte (vmreg.l[ARG1], &vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_read_ab:
        PRINTD("FILE_READ_ARRAY_BYTE");

        i = exe_fread_array_byte (vmreg.l[ARG1], varlist, ARG2, vmreg.l[ARG3]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_read_i:
        PRINTD("FILE_READ_INT");

        i = exe_fread_int (vmreg.l[ARG1], &vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_read_l:
        PRINTD("FILE_READ_LINT");

        i = exe_fread_lint (vmreg.l[ARG1], &vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_read_q:
        PRINTD("FILE_READ_QINT");

        i = exe_fread_qint (vmreg.l[ARG1], &vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_read_d:
        PRINTD("FILE_READ_DOUBLE");

        i = exe_fread_double (vmreg.l[ARG1], &vmreg.d[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_read_s:
        PRINTD("FILE_READ_STRING");

        i = exe_fread_string (vmreg.l[ARG1], (U1 *) &vmreg.s[ARG2], vmreg.l[ARG3]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_read_ls:
        PRINTD("FILE_READ_LINE_STRING");

        i = exe_fread_line_string (vmreg.l[ARG1], (U1 *) &vmreg.s[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();


    /* FWRITE ------------------------------------------------------- */

    file_write_b:
        PRINTD("FILE_WRITE_BYTE");

        i = exe_fwrite_byte (vmreg.l[ARG1], vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_write_ab:
        PRINTD("FILE_WRITE_ARRAY_BYTE");

        i = exe_fwrite_array_byte (vmreg.l[ARG1], varlist, ARG2, vmreg.l[ARG3]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_write_i:
        PRINTD("FILE_WRITE_INT");

        i = exe_fwrite_int (vmreg.l[ARG1], vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_write_l:
        PRINTD("FILE_WRITE_LINT");

        i = exe_fwrite_lint (vmreg.l[ARG1], vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_write_q:
        PRINTD("FILE_WRITE_QINT");

        i = exe_fwrite_qint (vmreg.l[ARG1], vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_write_d:
        PRINTD("FILE_WRITE_DOUBLE");

        i = exe_fwrite_double (vmreg.l[ARG1], vmreg.d[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_write_s:
        PRINTD("FILE_WRITE_STRING");

        i = exe_fwrite_string (vmreg.l[ARG1], vmreg.s[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_write_sl:
        PRINTD("FILE_WRITE_STR_LINT");

        i = exe_fwrite_str_lint (vmreg.l[ARG1], vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_write_sd:
        PRINTD("FILE_WRITE_STR_DOUBLE");

        i = exe_fwrite_str_double (vmreg.l[ARG1], vmreg.d[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_write_n:
        PRINTD("FILE_WRITE_NEWLINE");

        i = exe_fwrite_newline (vmreg.l[ARG1], vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_write_sp:
        PRINTD("FILE_WRITE_SPACE");

        i = exe_fwrite_space (vmreg.l[ARG1], vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_setpos:
        PRINTD("FILE_SETPOS");

        i = exe_set_fpos (vmreg.l[ARG1], vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_getpos:
        PRINTD("FILE_GETPOS");

        i = exe_get_fpos (vmreg.l[ARG1], &vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_rewind:
        PRINTD("FILE_REWIND");

        i = exe_frewind (vmreg.l[ARG1]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_size:
        PRINTD("FILE_SIZE");

        i = exe_fsize (vmreg.l[ARG1], &vmreg.l[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_remove:
        PRINTD("FILE_REMOVE");

        i = exe_fremove (vmreg.s[ARG1]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    file_rename:
        PRINTD("FILE_RENAME");

        i = exe_frename (vmreg.s[ARG1], vmreg.s[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();


    /* SSOPEN ------------------------------------------------------- */

    socket_sopen:
        PRINTD("SSOPEN");

		if (rights.network == TRUE)
		{
			vmreg.l[ERR_SOCK_R] = exe_open_server (&vmreg.l[ARG1], vmreg.s[ARG2], vmreg.l[ARG3], threadnum);
		}
		else
		{
			printf ("ERROR: no rights for socket access!\n");
			vmreg.l[ERR_SOCK_R] = ERR_FILE_OPEN;
		}

        EXE_NEXT();


    /* SSOPENACT ---------------------------------------------------- */

    socket_sopen_act:
        PRINTD("SSOPENACT");

        vmreg.l[ERR_SOCK_R] = exe_open_accept_server (vmreg.l[ARG1], &vmreg.l[ARG2]);

        EXE_NEXT();


    /* SCOPEN ------------------------------------------------------- */

    socket_copen:
        PRINTD("SCOPEN");

		if (rights.network == TRUE)
		{
			vmreg.l[ERR_SOCK_R] = exe_open_client (&vmreg.l[ARG1], vmreg.s[ARG2], vmreg.l[ARG3], threadnum);
		}
		else
		{
			printf ("ERROR: no rights for socket access!\n");
			vmreg.l[ERR_SOCK_R] = ERR_FILE_OPEN;
		}
			
        EXE_NEXT();


    /* SSCLOSE ------------------------------------------------------ */

    socket_sclose:
        PRINTD("SSCLOSE");

        vmreg.l[ERR_SOCK_R] = exe_close_server (vmreg.l[ARG1]);

        EXE_NEXT();


    /* SSCLOSEACT --------------------------------------------------- */

    socket_sclose_act:
        PRINTD("SCLOSEACT");

        vmreg.l[ERR_SOCK_R] = exe_close_accept_server (vmreg.l[ARG1]);

        EXE_NEXT();


    /* SCCLOSE ------------------------------------------------------ */

    socket_cclose:
        PRINTD("SCCLOSE");

        vmreg.l[ERR_SOCK_R] = exe_close_client (vmreg.l[ARG1]);

        EXE_NEXT();


    /* CLIENTADDR --------------------------------------------------- */

    clientaddr:
        PRINTD("CLIENTADDR");

        vmreg.l[ERR_SOCK_R] = exe_getclientaddr (vmreg.l[ARG1], (U1 *) &vmreg.s[ARG2]);

        EXE_NEXT();


    /* HOSTNAME ----------------------------------------------------- */

    hostname:
        PRINTD("HOSTNAME");

        vmreg.l[ERR_SOCK_R] = exe_gethostname ((U1 *) &vmreg.s[ARG1]);

        EXE_NEXT();


    /* HOSTBYNAME --------------------------------------------------- */

    hostbyname:
        PRINTD("HOSTBYNAME");

        vmreg.l[ERR_SOCK_R] = exe_gethostbyname (vmreg.s[ARG1], (U1 *) &vmreg.s[ARG2]);

        EXE_NEXT();


    /* HOSTBYADDR --------------------------------------------------- */

    hostbyaddr:
        PRINTD("HOSTBYADDR");

        vmreg.l[ERR_SOCK_R] = exe_gethostbyaddr (vmreg.s[ARG1], (U1 *) &vmreg.s[ARG2]);

        EXE_NEXT();


    /* SREAD_B ------------------------------------------------------ */

    socket_read_b:
        PRINTD("SREAD_B");

        vmreg.l[ERR_SOCK_R] = exe_sread_byte (vmreg.l[ARG1], &vmreg.l[ARG2]);

        EXE_NEXT();


    /* SREAD_ARRAY_B ------------------------------------------------ */

    socket_read_ab:
        PRINTD("SREAD_ARRAY_B");

        vmreg.l[ERR_SOCK_R] = exe_sread_array_byte (vmreg.l[ARG1], varlist, ARG2, vmreg.l[ARG3]);

        EXE_NEXT();


    /* SREAD_I ------------------------------------------------------ */

    socket_read_i:
        PRINTD("SREAD_I");

        vmreg.l[ERR_SOCK_R] = exe_sread_int (vmreg.l[ARG1], &vmreg.l[ARG2]);

        EXE_NEXT();


    /* SREAD_L ------------------------------------------------------ */

    socket_read_l:
        PRINTD("SREAD_L");

        vmreg.l[ERR_SOCK_R] = exe_sread_lint (vmreg.l[ARG1], &vmreg.l[ARG2]);

        EXE_NEXT();


    /* SREAD_Q ------------------------------------------------------ */

    socket_read_q:
        PRINTD("SREAD_Q");

        vmreg.l[ERR_SOCK_R] = exe_sread_qint (vmreg.l[ARG1], &vmreg.l[ARG2]);

        EXE_NEXT();


    /* SREAD_D ------------------------------------------------------ */

    socket_read_d:
        PRINTD("SREAD_D");

        vmreg.l[ERR_SOCK_R] = exe_sread_double (vmreg.l[ARG1], &vmreg.d[ARG2]);

        EXE_NEXT();


    /* SREAD_S ------------------------------------------------------ */

    socket_read_s:
        PRINTD("SREAD_S");

        vmreg.l[ERR_SOCK_R] = exe_sread_string (vmreg.l[ARG1], (U1 *) &vmreg.s[ARG2], vmreg.l[ARG3]);

        EXE_NEXT();


    /* SREAD_LINE_S ------------------------------------------------- */

    socket_read_ls:
        PRINTD("SREAD_LINE_S");

        vmreg.l[ERR_SOCK_R] = exe_sread_line_string (vmreg.l[ARG1], (U1 *) &vmreg.s[ARG2]);

        EXE_NEXT();


    /* SWRITE_B ----------------------------------------------------- */

    socket_write_b:
        PRINTD("SWRITE_B");

        vmreg.l[ERR_SOCK_R] = exe_swrite_byte (vmreg.l[ARG1], vmreg.l[ARG2]);

        EXE_NEXT();


    /* SWRITE_ARRAY_B ----------------------------------------------- */

    socket_write_ab:
        PRINTD("SWRITE_ARRAY_B");

        vmreg.l[ERR_SOCK_R] = exe_swrite_array_byte (vmreg.l[ARG1], varlist, ARG2, vmreg.l[ARG3]);

        EXE_NEXT();


    /* SWRITE_I ----------------------------------------------------- */

    socket_write_i:
        PRINTD("SWRITE_I");

        vmreg.l[ERR_SOCK_R] = exe_swrite_int (vmreg.l[ARG1], vmreg.l[ARG2]);

        EXE_NEXT();


    /* SWRITE_L ----------------------------------------------------- */

    socket_write_l:
        PRINTD("SWRITE_L");

        vmreg.l[ERR_SOCK_R] = exe_swrite_lint (vmreg.l[ARG1], vmreg.l[ARG2]);

        EXE_NEXT();


     /* SWRITE_Q ----------------------------------------------------- */

    socket_write_q:
        PRINTD("SWRITE_Q");

        vmreg.l[ERR_SOCK_R] = exe_swrite_qint (vmreg.l[ARG1], vmreg.l[ARG2]);

        EXE_NEXT();


    /* SWRITE_D ----------------------------------------------------- */

    socket_write_d:
        PRINTD("SWRITE_D");

        vmreg.l[ERR_SOCK_R] = exe_swrite_double (vmreg.l[ARG1], vmreg.d[ARG2]);

        EXE_NEXT();


    /* SWRITE_S ----------------------------------------------------- */

    socket_write_s:
        PRINTD("SWRITE_S");

        vmreg.l[ERR_SOCK_R] = exe_swrite_string (vmreg.l[ARG1], vmreg.s[ARG2]);

        EXE_NEXT();


    /* SWRITE_STR_L ------------------------------------------------- */

    socket_write_sl:
        PRINTD("SWRITE_STR_L");

        vmreg.l[ERR_SOCK_R] = exe_swrite_str_lint (vmreg.l[ARG1], vmreg.l[ARG2]);

        EXE_NEXT();


    /* SWRITE_STR_D ------------------------------------------------- */

    socket_write_sd:
        PRINTD("SWRITE_STR_D");

        vmreg.l[ERR_SOCK_R] = exe_swrite_str_double (vmreg.l[ARG1], vmreg.d[ARG2]);

        EXE_NEXT();


    /* SWRITE_NEWLINE ----------------------------------------------- */

    socket_write_n:
        PRINTD("SWRITE_NEWLINE");

        vmreg.l[ERR_SOCK_R] = exe_swrite_newline (vmreg.l[ARG1], vmreg.l[ARG2]);

        EXE_NEXT();


    /* SWRITE_SPACE ------------------------------------------------- */

    socket_write_sp:
        PRINTD("SWRITE_SPACE");

        vmreg.l[ERR_SOCK_R] = exe_swrite_space (vmreg.l[ARG1], vmreg.l[ARG2]);

        EXE_NEXT();


    /* SIZEOF ------------------------------------------------------- */

    vsizeof:
        PRINTD("VSIZEOF");

        vmreg.l[ARG2] = varlist[ARG1].size;

        EXE_NEXT();


    /* VMVAR -------------------------------------------------------- */

    vmvar:
        PRINTD("VMVAR");

        vmreg.l[ARG2] = varlist[ARG1].vm;

        EXE_NEXT();


    /* STR2VAL ------------------------------------------------------ */

    str2val_l:
        PRINTD("STR2VAL_L");

        if (checkdigit (vmreg.s[ARG1]))
        {
            vmreg.l[ARG2] = (S8) strtoll (t_var.digitstr, NULL, t_var.base);
        }
        else
        {
            printerr (WRONG_VARTYPE, epos, ST_EXE, "ARG1");
            ERREXIT();
        }

        EXE_NEXT();

    str2val_d:
        PRINTD("STR2VAL_D");

        if (checkdigit (vmreg.s[ARG1]))
        {
            vmreg.d[ARG2] = atof (t_var.digitstr);
        }
        else
        {
            printerr (WRONG_VARTYPE, epos, ST_EXE, "ARG1");
            ERREXIT();
        }

        EXE_NEXT();


    /* VAL2STR ------------------------------------------------------ */
    /* BUG ??? */
    val2str_l:
        PRINTD("VAL2STR_L");

        snprintf (varlist[STRING_TMP_1].s_m, varlist[STRING_TMP_1].size, "%lli", vmreg.l[ARG1]);
        if (MAXSTRING_VAR + 1 >= strlen (varlist[STRING_TMP_1].s_m) + 1)
        {
            strcpy (vmreg.s[ARG2], varlist[STRING_TMP_1].s_m);
        }
        else
        {
            printerr (OVERFLOW_STR, epos, ST_EXE, "ARG2");
            ERREXIT();
        }

        EXE_NEXT();

    val2str_d:
        PRINTD("VAL2STR_D");

        snprintf (varlist[STRING_TMP_1].s_m, varlist[STRING_TMP_1].size, "%.10lf", vmreg.d[ARG1]);
        if (MAXSTRING_VAR + 1 >= strlen (varlist[STRING_TMP_1].s_m) + 1)
        {
            strcpy (vmreg.s[ARG1], varlist[STRING_TMP_1].s_m);
        }
        else
        {
            printerr (OVERFLOW_STR, epos, ST_EXE, "ARG2");
            ERREXIT();
        }

        EXE_NEXT();


    /* DOUBLE2INT --------------------------------------------------- */

    double2int:
        PRINTD("DOUBLE2INT");

        vmreg.l[ARG2] = (S8) vmreg.d[ARG1];

        EXE_NEXT();


    /* INT2DOUBLE --------------------------------------------------- */

    int2double:
        PRINTD("INT2DOUBLE");

        vmreg.d[ARG2] = (F8) vmreg.l[ARG1];

        EXE_NEXT();


    /* SRAND -------------------------------------------------------- */

    srand_l:
        PRINTD("SRAND");

        init_genrand (vmreg.l[ARG1]);

        EXE_NEXT();


    /* RAND_L ------------------------------------------------------- */

    rand_l:
        PRINTD("RAND_L");

        vmreg.l[ARG1] = (S4) genrand_int31 ();

        EXE_NEXT();


    /* RAND_D ------------------------------------------------------- */

    rand_d:
        PRINTD("RAND_D");

        vmreg.d[ARG1] = genrand_real1 ();

        EXE_NEXT();


    /* ABS_L -------------------------------------------------------- */

    abs_l:
        PRINTD("ABS_L");

        vmreg.l[ARG2] = (S8) labs (vmreg.l[ARG1]);

        EXE_NEXT();


    /* ABS_D -------------------------------------------------------- */

    abs_d:
        PRINTD("ABS_D");

        vmreg.d[ARG2] = fabs (vmreg.d[ARG1]);

        EXE_NEXT();


    /* CEIL --------------------------------------------------------- */

    ceil_d:
        PRINTD("CEIL");

        vmreg.d[ARG2] = ceil (vmreg.d[ARG1]);

        EXE_NEXT();


    /* FLOOR -------------------------------------------------------- */

    floor_d:
        PRINTD("FLOOR");

        vmreg.d[ARG2] = floor (vmreg.d[ARG1]);

        EXE_NEXT();


    /* EXP ---------------------------------------------------------- */

    exp_d:
        PRINTD("EXP");

        vmreg.d[ARG2] = exp (vmreg.d[ARG1]);

        EXE_NEXT();


    /* LOG ---------------------------------------------------------- */

    log_d:
        PRINTD("LOG");

        vmreg.d[ARG2] = log (vmreg.d[ARG1]);

        EXE_NEXT();


    /* LOG 10 ------------------------------------------------------- */

    log10_d:
        PRINTD("LOG 10");

        vmreg.d[ARG2] = log10 (vmreg.d[ARG1]);

        EXE_NEXT();


    /* POW ---------------------------------------------------------- */

    pow_d:
        PRINTD("POW");

        vmreg.d[ARG3] = pow (vmreg.d[ARG1], vmreg.d[ARG2]);

        EXE_NEXT();


    /* SQRT --------------------------------------------------------- */

    sqrt_d:
        PRINTD("SQRT");

        vmreg.d[ARG2] = sqrt (vmreg.d[ARG1]);

        EXE_NEXT();


    /* COS ---------------------------------------------------------- */

    cos_d:
        PRINTD("COS");

        vmreg.d[ARG2] = cos (vmreg.d[ARG1]);

        EXE_NEXT();


    /* SIN ---------------------------------------------------------- */

    sin_d:
        PRINTD("SIN");

        vmreg.d[ARG2] = sin (vmreg.d[ARG1]);

        EXE_NEXT();


    /* TAN ---------------------------------------------------------- */

    tan_d:
        PRINTD("TAN");

        vmreg.d[ARG2] = tan (vmreg.d[ARG1]);

        EXE_NEXT();


    /* ACOS --------------------------------------------------------- */

    acos_d:
        PRINTD("ACOS");

        vmreg.d[ARG2] = acos (vmreg.d[ARG1]);

        EXE_NEXT();


    /* ASIN --------------------------------------------------------- */

    asin_d:
        PRINTD("ASIN");

        vmreg.d[ARG2] = asin (vmreg.d[ARG1]);

        EXE_NEXT();


    /* ATAN --------------------------------------------------------- */

    atan_d:
        PRINTD("ATAN");

        vmreg.d[ARG2] = atan (vmreg.d[ARG1]);

        EXE_NEXT();


    /* COSH --------------------------------------------------------- */

    cosh_d:
        PRINTD("COSH");

        vmreg.d[ARG2] = cosh (vmreg.d[ARG1]);

        EXE_NEXT();


    /* SINH --------------------------------------------------------- */

    sinh_d:
        PRINTD("SINH");

        vmreg.d[ARG2] = sinh (vmreg.d[ARG1]);

        EXE_NEXT();


    /* TANH --------------------------------------------------------- */

    tanh_d:
        PRINTD("TANH");

        vmreg.d[ARG2] = tanh (vmreg.d[ARG1]);

        EXE_NEXT();


    /* RUNPROCESS --------------------------------------------------- */

    run_process:
        PRINTD ("RUNPROCESS");

        vmreg.l[ERR_PROCESS_R] = exe_run_process ((U1 *) vmreg.s[ARG1], &vmreg.l[ARG2]);

        EXE_NEXT();


    /* RUNSHELL ----------------------------------------------------- */

    run_shell:
        PRINTD ("RUNSHELL");

        vmreg.l[ERR_PROCESS_R] = exe_run_shell ((U1 *) vmreg.s[ARG1], &vmreg.l[ARG2]);

        EXE_NEXT();


    /* WAITPROCESS -------------------------------------------------- */

    wait_process:
        PRINTD ("WAITPROCESS");

        vmreg.l[ERR_PROCESS_R] = exe_wait_process (vmreg.l[ARG1], &vmreg.l[ARG2]);

        EXE_NEXT();


    /* ARGNUM ------------------------------------------------------- */

    argnum:
        PRINTD("ARGNUM");

        vmreg.l[ARG1] = shell_args.args;

        EXE_NEXT();


    /* ARGSTR ------------------------------------------------------- */

    argstr:
        PRINTD("ARGSTR");

        if (MAXSTRING_VAR + 1 >= strlen (shell_args.str[vmreg.l[ARG1]]) + 1)
        {
            strcpy (vmreg.s[ARG2], shell_args.str[vmreg.l[ARG1]]);
        }
        else
        {
            printerr (OVERFLOW_STR, epos, ST_EXE, varlist[ARG2].name);
            ERREXIT();
        }

        EXE_NEXT();


    /* STACK PUSH --------------------------------------------------- */

    stpush_l:
        PRINTD("STPUSH_L");

        i = exe_stpush_l (threadnum, vmreg.l[ARG1]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    stpush_d:
        PRINTD("STPUSH_D");

        i = exe_stpush_d (threadnum, vmreg.d[ARG1]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    stpush_s:
        PRINTD("STPUSH_S");

        i = exe_stpush_s (threadnum, vmreg.s[ARG1]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    stpush_all_l:
        PRINTD("STPUSH_ALL_L");

        for (i = 0; i < MAXVMREG; i++)
        {
            if (exe_stpush_l (threadnum, vmreg.l[i]) != ERR_STACK_OK)
            {
                printerr (get_sterr (i), epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();

    stpush_all_d:
        PRINTD("STPUSH_ALL_D");

        for (i = 0; i < MAXVMREG; i++)
        {
            if (exe_stpush_d (threadnum, vmreg.d[i]) != ERR_STACK_OK)
            {
                printerr (get_sterr (i), epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();


    /* STACK PULL --------------------------------------------------- */

    stpull_l:
        PRINTD("STPULL_L");

        i = exe_stpull_l (threadnum, &vmreg.l[ARG1]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    stpull_d:
        PRINTD("STPULL_D");

        i = exe_stpull_d (threadnum, &vmreg.d[ARG1]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    stpull_s:
        PRINTD("STPULL_S");

        i = exe_stpull_s (threadnum, (U1 *) &vmreg.s[ARG1]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    stpull_all_l:
        PRINTD("STPULL_ALL_L");

        for (i = MAXVMREG - 1; i >= 0; i--)
        {
            if (exe_stpull_l (threadnum, &vmreg.l[i]) != ERR_STACK_OK)
            {
                printerr (get_sterr (i), epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();

    stpull_all_d:
        PRINTD("STPULL_ALL_D");

        for (i = MAXVMREG - 1; i >= 0; i--)
        {
            if (exe_stpull_d (threadnum, &vmreg.d[i]) != ERR_STACK_OK)
            {
                printerr (get_sterr (i), epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();

    ston:
        PRINTD("STACK_ON");

        i = alloc_stack ();
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();


    /* SHOW STACK --------------------------------------------------- */

    show_stack:
        PRINTD("SHOW_STACK");

        i = exe_show_stack (vmreg.l[ARG1]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();


    /* STGETTYPE ---------------------------------------------------- */

    stgettype:
        PRINTD("STGETTYPE");

        i = exe_stgettype (threadnum, &vmreg.l[ARG1]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();


    /* STELEMENTS --------------------------------------------------- */

    stelements:
        PRINTD("STELEMENTS");

        i = exe_stelements (threadnum, &vmreg.l[ARG1]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();


    /* GETTYPE ------------------------------------------------------ */

    gettype:
        PRINTD("GETTYPE");

        vmreg.l[ARG2] = varlist[vmreg.l[ARG1]].type;

        EXE_NEXT();


    /* ALLOC -------------------------------------------------------- */

    alloc:
        PRINTD("ALLOC");

        #if DEBUG
            if (print_debug)
            {
                printf ("arg: %li\n", ARG1);
            }
        #endif

        if (! exe_alloc_var (varlist, ARG1, varlist[ARG1].type))
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = FALSE;
            }
            else
            {
                printerr (MEMORY, epos, ST_EXE, varlist[ARG1].name);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = TRUE;
            }
        }

        EXE_NEXT();


    /* DEALLOC ------------------------------------------------------ */

    dealloc:
        PRINTD("DEALLOC");

        if (! varlist[ARG1].vm)
        {
            /* dealloc if not a virtual memory variable */

            if (! exe_dealloc_var (varlist, ARG1))
            {
                printerr (NOTDEF_VAR, epos, ST_EXE, varlist[ARG1].name);
                ERREXIT();
            }
        }

        EXE_NEXT();


    /* GETADDRESS --------------------------------------------------- */

    getaddress:
        PRINTD("GETADDRESS");

        vmreg.l[ARG2] = ARG1;

        EXE_NEXT();


    /* POINTER ------------------------------------------------------ */

    pointer:
        PRINTD("POINTER");

		poffset = 0;
		
    pointer_try:
        popcode = pcclist[ARG3 + poffset][0];

        /* check opcode */

        /*    printf ("exe_pointer: label: %li\n", label); */
    
        switch (popcode)
        {
            case LETARRAY2_I:
            case LETARRAY2_L:
            case LETARRAY2_Q:
            case LETARRAY2_D:
            case LETARRAY2_B:
            case LETARRAY2_S:

            case VAR_LOCK:
            case VAR_UNLOCK:
            case PMOVE_A_I:
            case PMOVE_A_L:
            case PMOVE_A_Q:
            case PMOVE_A_D:
            case PMOVE_A_B:
            case PMOVE_A_S:

            case MOVE_ADYN_Q:
            case MOVE_ADYN_D:
            case MOVE_ADYN_S:

            case PMOVE_Q_ADYN:
            case PMOVE_D_ADYN:
            case PMOVE_S_ADYN:

                /* setaddress */

                pcclist[ARG3 + poffset][1] = vmreg.l[ARG1];
                break;

            case LET2ARRAY_I:
            case LET2ARRAY_L:
            case LET2ARRAY_Q:
            case LET2ARRAY_D:
            case LET2ARRAY_B:
            case LET2ARRAY_S:

            case PMOVE_I_A:
            case PMOVE_L_A:
            case PMOVE_Q_A:
            case PMOVE_D_A:
            case PMOVE_B_A:
            case PMOVE_S_A:

            case MOVE_Q_ADYN:
            case MOVE_D_ADYN:
            case MOVE_S_ADYN:

            case PMOVE_ADYN_Q:
            case PMOVE_ADYN_D:
            case PMOVE_ADYN_S:

                /* setaddress */

                pcclist[ARG3 + poffset][2] = vmreg.l[ARG1];
                break;

            case SIZEOF:

                /* setaddress */

                pcclist[ARG3 + poffset][1] = vmreg.l[ARG1];
                break;

            default:
                /* wrong opcode */
            
                if (poffset == pmaxoffset)
                {
                    printf ("error: pointer wrong opcode: %li\n", popcode);
					printerr (SYNTAX, epos, ST_EXE, "");
                    ERREXIT();
                }
            
                poffset++;
                goto pointer_try;
        }

        EXE_NEXT();


    /* NOP ---------------------------------------------------------- */

    nop:
        PRINTD("NOP");

        EXE_NEXT();


    /* PEXIT -------------------------------------------------------- */

    pexit:
        PRINTD("PEXIT");

        pthreads[threadnum].ret_val = vmreg.l[ARG1];

        dealloc_array_lint (pcclist, maxcclist + 1);
        exe_dealloc_pvarlist (pvarlist);
        free (dthread);
		set_thread_stopped (threadnum);
        
        #if OS_AROS
            ExitThread (pthreads[threadnum].ret_val);
        #else
            pthread_exit (&pthreads[threadnum].ret_val);
        #endif
			
    /* ERREXIT (error exit) ------------------------------------------- */

    eexit:
        PRINTD("ERREXIT");

        error_inter = TRUE;

        pthreads[threadnum].ret_val = exit_retval;

        dealloc_array_lint (pcclist, maxcclist + 1);
        exe_dealloc_pvarlist (pvarlist);
        free (dthread);
        set_thread_stopped (threadnum);
       
        #if OS_AROS
            ExitThread (pthreads[threadnum].ret_val);
        #else
            pthread_exit (&pthreads[threadnum].ret_val);
        #endif

    /* SUPER OPCODES for speedup ---------------------------------------- */

    /* INC_LESS_JMP_L --------------------------------------------------- */

    inc_ls_jmp_l:
        PRINTD("INC_LS_JMP_L");

        vmreg.l[ARG1]++;
        if (vmreg.l[ARG1] < vmreg.l[ARG2])
        {
            epos = ARG3 - 1;
        }

        EXE_NEXT();


    /* INC_LESS_OR_EQUAL_JMP_L ------------------------------------------ */

    inc_lseq_jmp_l:
        PRINTD("INC_LSEQ_JMP_L");

        vmreg.l[ARG1]++;
        if (vmreg.l[ARG1] <= vmreg.l[ARG2])
        {
            epos = ARG3 - 1;
        }

        EXE_NEXT();


    /* DEC_GREATER_JMP_L ------------------------------------------------ */

    dec_gr_jmp_l:
        PRINTD("DEC_GR_JMP_L");

        vmreg.l[ARG1]--;
        if (vmreg.l[ARG1] > vmreg.l[ARG2])
        {
            epos = ARG3 - 1;
        }

        EXE_NEXT();


    /* DEC_GREATER_OR_EQUAL_JMP_L --------------------------------------- */

    dec_greq_jmp_l:
        PRINTD("DEC_GREQ_JMP_L");

        vmreg.l[ARG1]--;
        if (vmreg.l[ARG1] >= vmreg.l[ARG2])
        {
            epos = ARG3 - 1;
        }

        EXE_NEXT();


    /* INC_LESS_JSR_L --------------------------------------------------- */

    inc_ls_jsr_l:
        PRINTD("INC_LS_JSR_L");

        vmreg.l[ARG1]++;
        if (vmreg.l[ARG1] < vmreg.l[ARG2])
        {
            if (pthreads[threadnum].jumpreclist_ind < MAXJUMPRECLEN - 1)
            {
               pthreads[threadnum].jumpreclist_ind++;

                /* set jumppos for return */

                pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos = epos;

                epos = ARG3 - 1;
            }
            else
            {
                printerr (JUMPRECLIST, epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();


    /* INC_LESS_OR_EQ_JSR_L --------------------------------------------- */

    inc_lseq_jsr_l:
        PRINTD("INC_LSEQ_JSR_L");

        vmreg.l[ARG1]++;
        if (vmreg.l[ARG1] <= vmreg.l[ARG2])
        {
            if (pthreads[threadnum].jumpreclist_ind < MAXJUMPRECLEN - 1)
            {
                pthreads[threadnum].jumpreclist_ind++;

                /* set jumppos for return */

                pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos = epos;

                epos = ARG3 - 1;
            }
            else
            {
                printerr (JUMPRECLIST, epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();


   /* DEC_GREATER_JSR_L ------------------------------------------------ */

    dec_gr_jsr_l:
        PRINTD("DEC_GR_JSR_L");

        vmreg.l[ARG1]--;
        if (vmreg.l[ARG1] > vmreg.l[ARG2])
        {
            if (pthreads[threadnum].jumpreclist_ind < MAXJUMPRECLEN - 1)
            {
                pthreads[threadnum].jumpreclist_ind++;

                /* set jumppos for return */

                pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos = epos;

                epos = ARG3 - 1;
            }
            else
            {
                printerr (JUMPRECLIST, epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();


    /* DEC_GREATER_OR_EQ_JSR_L ------------------------------------------ */

    dec_greq_jsr_l:
        PRINTD("DEC_GREQ_JSR_L");

        vmreg.l[ARG1]--;
        if (vmreg.l[ARG1] >= vmreg.l[ARG2])
        {
            if (pthreads[threadnum].jumpreclist_ind < MAXJUMPRECLEN - 1)
            {
                pthreads[threadnum].jumpreclist_ind++;

                /* set jumppos for return */

                pthreads[threadnum].jumpreclist[pthreads[threadnum].jumpreclist_ind].pos = epos;

                epos = ARG3 - 1;
            }
            else
            {
                printerr (JUMPRECLIST, epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();

    /* HASH32_AB ---------------------------------------------------- */

    hash32_ab:
        PRINTD("HASH32_AB");

        if (exe_hash32_array_byte (varlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]) == FALSE)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    stack_set:
        PRINTD("STACK_SET");

        /* set vm sizes in registers */
        vmreg_vm.l[MEMBSIZE] = vmreg.l[MEMBSIZE];
        vmreg_vm.l[VMBSIZE] = vmreg.l[VMBSIZE];
        vmreg_vm.l[VMCACHESIZE] = vmreg.l[VMCACHESIZE];
        vmreg_vm.l[VMUSE] = vmreg.l[VMUSE];

        /* set stack size in register */
        vmreg_vm.l[STSIZE] = vmreg.l[STSIZE];

        EXE_NEXT();


    push_s:
        PRINTD("PUSH_S");

        if (strlen (varlist[ARG1].s_m) <= MAXSTRING_VAR)
        {
            strcpy (vmreg.s[ARG2], varlist[ARG1].s_m);
        }
        else
        {
            printerr (OVERFLOW_STR, epos, ST_EXE, varlist[ARG1].name);
            ERREXIT();
        }

        EXE_NEXT();

    pull_s:
        PRINTD("PULL_S");

        if (strlen (vmreg.s[ARG1]) <= varlist[ARG2].size)
        {
            strcpy (varlist[ARG2].s_m, vmreg.s[ARG1]);
        }
        else
        {
            printerr (OVERFLOW_STR, epos, ST_EXE, varlist[ARG2].name);
            ERREXIT();
        }

        EXE_NEXT();

    stpush_all_s:
        PRINTD("STPUSH_ALL_S");

        for (i = 0; i < MAXVMREG; i++)
        {
            if (exe_stpush_s (threadnum, vmreg.s[i]) != ERR_STACK_OK)
            {
                printerr (get_sterr (i), epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();

    stpull_all_s:
        PRINTD("STPULL_ALL_S");

        for (i = MAXVMREG - 1; i >= 0; i--)
        {
            if (exe_stpull_s (threadnum, (U1 *) &vmreg.s[i]) != ERR_STACK_OK)
            {
                printerr (get_sterr (i), epos, ST_EXE, "");
                ERREXIT();
            }
        }

        EXE_NEXT();

    not_l:
        PRINTD("NOT_L");

        vmreg.l[ARG2] = ! vmreg.l[ARG1];

        EXE_NEXT();

    var_lock:
        PRINTD("VAR_LOCK");

        #if OS_AROS
            LockMutex (varlist[ARG1].mutex);
        #else
            pthread_mutex_lock (&varlist[ARG1].mutex);
        #endif
            
        EXE_NEXT();

    var_unlock:
        PRINTD("VAR_UNLOCK");

        #if OS_AROS
            UnlockMutex (varlist[ARG1].mutex);
        #else
            pthread_mutex_unlock (&varlist[ARG1].mutex);
        #endif
            
        EXE_NEXT();

    thread_create:
        PRINTD("THREAD_CREATE");

        vmreg.l[ARG1] = create_new_thread (ARG2 - 1);

        EXE_NEXT();

    thread_join:
        PRINTD("THREAD_JOIN");

        join_threads (threadnum);

        EXE_NEXT();

    thread_state:
        PRINTD("THREAD_STATE");

        vmreg.l[ARG2] = get_thread_state (vmreg.l[ARG1]);

        EXE_NEXT();

    thread_num:
        PRINTD("THREAD_NUM");

        vmreg.l[ARG1] = threadnum;

        EXE_NEXT();

    thread_sync:
        PRINTD("THREAD_SYNC");

        wait_thread_running (vmreg.l[ARG1]);

        EXE_NEXT();

    /* THREAD STACK PUSH --------------------------------------------------- */
    /* ARG1: threadnum */

    thpush_l:
        PRINTD("THREAD_PUSH_L");

        i = exe_stpush_l (vmreg.l[ARG1], vmreg.l[ARG2]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    thpush_d:
        PRINTD("THREAD_PUSH_D");

        i = exe_stpush_d (vmreg.l[ARG1], vmreg.d[ARG2]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    thpush_s:
        PRINTD("THREAD_PUSH_S");

        i = exe_stpush_s (vmreg.l[ARG1], vmreg.s[ARG2]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    thpush_sync:
        PRINTD("THREAD_PUSH_SYNC");

        i = exe_stpush_sync (vmreg.l[ARG1]);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    thpull_sync:
        PRINTD("THREAD_PULL_SYNC");

        i = exe_stpull_sync (threadnum);
        if (i != ERR_STACK_OK)
        {
            printerr (get_sterr (i), epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();
        
        
    thread_exitcode:
        PRINTD("THREAD_EXITCODE");
        
        #if OS_AROS
            LockMutex (pthreads_mutex);
        #else
            pthread_mutex_lock (&pthreads_mutex);
        #endif
        
        vmreg.l[ARG2] = pthreads[vmreg.l[ARG1]].ret_val;

        #if OS_AROS
            UnlockMutex (pthreads_mutex);
        #else
            pthread_mutex_unlock (&pthreads_mutex);
        #endif
            
        EXE_NEXT();
        
    /* PALLOC -------------------------------------------------------- */

    palloc:
        PRINTD("PALLOC");

        #if DEBUG
            if (print_debug)
            {
                printf ("arg: %li\n", ARG1);
            }
        #endif

        if (! exe_alloc_var (pvarlist, ARG1, pvarlist[ARG1].type))
        {
            if (*pvarlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *pvarlist[ERR_ALLOC_R].i_m = FALSE;
            }
            else
            {
                printerr (MEMORY, epos, ST_EXE, pvarlist[ARG1].name);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = TRUE;
            }
        }

        EXE_NEXT();


    /* PDEALLOC ------------------------------------------------------ */

    pdealloc:
        PRINTD("PDEALLOC");

        if (! pvarlist[ARG1].vm)
        {
            /* dealloc if not a virtual memory variable */

            if (! exe_dealloc_var (pvarlist, ARG1))
            {
                printerr (NOTDEF_VAR, epos, ST_EXE, pvarlist[ARG1].name);
                ERREXIT();
            }
        }

        EXE_NEXT();


    /* PPUSH --------------------------------------------------------- */

    ppush_i:
        PRINTD("PPUSH_I");

        vmreg.l[ARG2] = (S8) *pvarlist[ARG1].i_m;

        EXE_NEXT();

    ppush_l:
        PRINTD("PPUSH_L");

        vmreg.l[ARG2] = (S8) *pvarlist[ARG1].li_m;

        EXE_NEXT();

    ppush_q:
        PRINTD("PPUSH_Q");

        vmreg.l[ARG2] = *pvarlist[ARG1].q_m;

        EXE_NEXT();

    ppush_d:
        PRINTD("PPUSH_D");

        vmreg.d[ARG2] = *pvarlist[ARG1].d_m;

        EXE_NEXT();

    ppush_b:
        PRINTD("PPUSH_B");

        vmreg.l[ARG2] = (S4) pvarlist[ARG1].s_m[0];

        EXE_NEXT();

    ppush_s:
        PRINTD("PPUSH_S");

        if (strlen (pvarlist[ARG1].s_m) <= MAXSTRING_VAR)
        {
            strcpy (vmreg.s[ARG2], pvarlist[ARG1].s_m);
        }
        else
        {
            printerr (OVERFLOW_STR, epos, ST_EXE, varlist[ARG1].name);
            ERREXIT();
        }

        EXE_NEXT();

    /* PULL --------------------------------------------------------- */

    ppull_i:
        PRINTD("PPULL_I");

        *pvarlist[ARG2].i_m = (S2) vmreg.l[ARG1];

        EXE_NEXT();

    ppull_l:
        PRINTD("PPULL_L");

        *pvarlist[ARG2].li_m = (S4) vmreg.l[ARG1];

        EXE_NEXT();

    ppull_q:
        PRINTD("PPULL_Q");

        *pvarlist[ARG2].q_m = vmreg.l[ARG1];

        EXE_NEXT();

    ppull_d:
        PRINTD("PPULL_D");

        *pvarlist[ARG2].d_m = vmreg.d[ARG1];

        EXE_NEXT();

    ppull_b:
        PRINTD("PPULL_B");

        pvarlist[ARG2].s_m[0] = (U1) vmreg.l[ARG1];

        EXE_NEXT();

    ppull_s:
        PRINTD("PPULL_S");

        if (strlen (vmreg.s[ARG1]) <= pvarlist[ARG2].size)
        {
            strcpy (pvarlist[ARG2].s_m, vmreg.s[ARG1]);
        }
        else
        {
            printerr (OVERFLOW_STR, epos, ST_EXE, varlist[ARG2].name);
            ERREXIT();
        }

        EXE_NEXT();

    /* LET_ARRAY2 PRIVATE --------------------------------------------------- */

    pmove_a_i:
        PRINTD("PMOVE_A_I");

        if (! exe_let_array2_int (pvarlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    pmove_a_l:
        PRINTD("PMOVE_A_L");

        if (! exe_let_array2_lint (pvarlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    pmove_a_q:
        PRINTD("PMOVE_A_Q");

        if (! exe_let_array2_qint (pvarlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    pmove_a_d:
        PRINTD("PMOVE_A_D");

        if (! exe_let_array2_double (pvarlist, ARG1, vmreg.d[ARG2], &vmreg.d[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    pmove_a_b:
        PRINTD("PMOVE_A_B");

        if (! exe_let_array2_byte (pvarlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    pmove_a_s:
        PRINTD("PMOVE_A_S");

        if (! exe_let_array2_string (pvarlist, ARG1, vmreg.l[ARG2], (U1 *) &vmreg.s[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();


    /* LET_2ARRAY PRIVATE --------------------------------------------------- */

    pmove_i_a:
        PRINTD("PMOVE_I_A");

        if (! exe_let_2array_int (pvarlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    pmove_l_a:
        PRINTD("PMOVE_L_A");

        if (! exe_let_2array_lint (pvarlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

   pmove_q_a:
        PRINTD("PMOVE_Q_A");

        if (! exe_let_2array_qint (pvarlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    pmove_d_a:
        PRINTD("PMOVE_D_A");

        if (! exe_let_2array_double (pvarlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    pmove_b_a:
        PRINTD("PMOVE_B_A");

        if (! exe_let_2array_byte (pvarlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    pmove_s_a:
        PRINTD("PMOVE_S_A");

        if (! exe_let_2array_string (pvarlist, vmreg.s[ARG1], ARG2, vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

    pfread_ab:
        PRINTD("PFREAD_AB");

        i = exe_fread_array_byte (vmreg.l[ARG1], pvarlist, ARG2, vmreg.l[ARG3]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    pfwrite_ab:
        PRINTD("PFWRITE_AB");

        i = exe_fwrite_array_byte (vmreg.l[ARG1], pvarlist, ARG2, vmreg.l[ARG3]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();

    /* SREAD_PARRAY_B ------------------------------------------------ */

    psread_ab:
        PRINTD("PSREAD_AB");

        vmreg.l[ERR_SOCK_R] = exe_sread_array_byte (vmreg.l[ARG1], pvarlist, ARG2, vmreg.l[ARG3]);

        EXE_NEXT();

    /* SWRITE_PARRAY_B ----------------------------------------------- */

    pswrite_ab:
        PRINTD("PSWRITE_AB");

        vmreg.l[ERR_SOCK_R] = exe_swrite_array_byte (vmreg.l[ARG1], pvarlist, ARG2, vmreg.l[ARG3]);

        EXE_NEXT();
        
    int2hex:
        PRINTD("INT2HEX");

        snprintf (varlist[STRING_TMP_1].s_m, varlist[STRING_TMP_1].size, "%X", vmreg.l[ARG1]);
        if (MAXSTRING_VAR + 1 >= strlen (varlist[STRING_TMP_1].s_m) + 1)
        {
            strcpy (vmreg.s[ARG2], varlist[STRING_TMP_1].s_m);
        }
        else
        {
            printerr (OVERFLOW_STR, epos, ST_EXE, "ARG2");
            ERREXIT();
        }

        EXE_NEXT();   
    
    phash32_ab:
        PRINTD("PHASH32_AB");
        
        if (exe_hash32_array_byte (pvarlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]) == FALSE)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();
        
    /* HASH64_AB ---------------------------------------------------- */

    hash64_ab:
        PRINTD("HASH64_AB");

        if (exe_hash64_array_byte (varlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]) == FALSE)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();
        
    phash64_ab:
        PRINTD("PHASH64_AB");

        if (exe_hash64_array_byte (pvarlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]) == FALSE)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();
        
        
    /* Dynamic arrays --------------------------------------------------------- */    
       
    move_adyn_q:
        PRINTD("MOVE_ADYN_Q");
        
        if (! exe_let_dyn_array2_qint (varlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
   move_adyn_d:
        PRINTD("MOVE_ADYN_D");
        
        if (! exe_let_dyn_array2_double (varlist, ARG1, vmreg.d[ARG2], &vmreg.d[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
   move_adyn_s:
        PRINTD("MOVE_ADYN_S");
   
        if (! exe_let_dyn_array2_string (varlist, ARG1, vmreg.l[ARG2], (U1 *) &vmreg.s[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

   /* ========================================================================= */
        
   move_q_adyn:
        PRINTD("MOVE_Q_ADYN");
        
        if (! exe_let_2_dyn_array_qint (varlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = FALSE;
            }
            else
            {
                printerr (MEMORY, epos, ST_EXE, varlist[ARG1].name);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = TRUE;
            }
        }

        EXE_NEXT();
        
    move_d_adyn:
        PRINTD("MOVE_D_ADYN");
    
        if (! exe_let_2_dyn_array_double (varlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = FALSE;
            }
            else
            {
                printerr (MEMORY, epos, ST_EXE, varlist[ARG1].name);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = TRUE;
            }
        }
        
        EXE_NEXT();
        
    move_s_adyn:
        PRINTD("MOVE_S_ADYN");
    
        if (! exe_let_2_dyn_array_string (varlist, vmreg.s[ARG1], ARG2, vmreg.l[ARG3]))
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = FALSE;
            }
            else
            {
                printerr (MEMORY, epos, ST_EXE, varlist[ARG1].name);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = TRUE;
            }
        }
        
        EXE_NEXT();
        
    /* Dynamic arrays private ------------------------------------------------- */
    
    pmove_adyn_q:
        PRINTD("PMOVE_ADYN_Q");
        
        if (! exe_let_dyn_array2_qint (pvarlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();
        
    pmove_adyn_d:
        PRINTD("PMOVE_ADYN_D");
        
        if (! exe_let_dyn_array2_double (pvarlist, ARG1, vmreg.d[ARG2], &vmreg.d[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
   pmove_adyn_s:
        PRINTD("PMOVE_ADYN_S");
   
        if (! exe_let_dyn_array2_string (pvarlist, ARG1, vmreg.l[ARG2], (U1 *) &vmreg.s[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();

   /* ========================================================================= */
        
   pmove_q_adyn:
        PRINTD("PMOVE_Q_ADYN");
        
        if (! exe_let_2_dyn_array_qint (pvarlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = FALSE;
            }
            else
            {
                printerr (MEMORY, epos, ST_EXE, varlist[ARG2].name);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = TRUE;
            }
        }

        EXE_NEXT();
        
    pmove_d_adyn:
        PRINTD("PMOVE_D_ADYN");
    
        if (! exe_let_2_dyn_array_double (pvarlist, vmreg.l[ARG1], ARG2, vmreg.l[ARG3]))
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = FALSE;
            }
            else
            {
                printerr (MEMORY, epos, ST_EXE, varlist[ARG2].name);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = TRUE;
            }
        }
        
        EXE_NEXT();
        
    pmove_s_adyn:
        PRINTD("PMOVE_S_ADYN");
    
        if (! exe_let_2_dyn_array_string (pvarlist, vmreg.s[ARG1], ARG2, vmreg.l[ARG3]))
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = FALSE;
            }
            else
            {
                printerr (MEMORY, epos, ST_EXE, varlist[ARG2].name);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_ALLOC].i_m == TRUE)
            {
                /* set _alloc */

                *varlist[ERR_ALLOC_R].i_m = TRUE;
            }
        }
        
        EXE_NEXT();
        
    dyngettype:
        PRINTD("DYNGETTYPE");
        
        if (! exe_dyngettype (varlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();
        
    pdyngettype:
        PRINTD("PDYNGETTYPE");
        
        if (! exe_dyngettype (pvarlist, ARG1, vmreg.l[ARG2], &vmreg.l[ARG3]))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();
        
        
    /* VECTOR ARRAY ENGINE ADD INTEGER TO ARRAY --------------------- */    
        
    vadd_l:
        PRINTD("VADD_L");
        
        if (! exe_vadd_l (varlist, varlist, ARG1, vmreg.l[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vadd_d:
        PRINTD("VADD_D");
        
        if (! exe_vadd_d (varlist, varlist, ARG1, vmreg.d[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vsub_l:
        PRINTD("VSUB_L");
        
        if (! exe_vsub_l (varlist, varlist, ARG1, vmreg.l[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vsub_d:
        PRINTD("VSUB_D");
        
        if (! exe_vsub_d (varlist, varlist, ARG1, vmreg.d[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vmul_l:
        PRINTD("VMUL_L");
        
        if (! exe_vmul_l (varlist, varlist, ARG1, vmreg.l[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vmul_d:
        PRINTD("VMUL_D");
        
        if (! exe_vmul_d (varlist, varlist, ARG1, vmreg.d[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vdiv_l:
        PRINTD("VDIV_L");
        
        if (! exe_vdiv_l (varlist, varlist, ARG1, vmreg.l[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vdiv_d:
        PRINTD("VDIV_D");
        
        if (! exe_vdiv_d (varlist, varlist, ARG1, vmreg.d[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
        
     /* VECTOR ARRAY ENGINE ADD ARRAY TO ARRAY ---------------------- */      
        
    vadd2_l:
        PRINTD("VADD2_L");
        
        if (! exe_vadd2_l (varlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vadd2_d:
        PRINTD("VADD2_D");
        
        if (! exe_vadd2_d (varlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vsub2_l:
        PRINTD("VSUB2_L");
        
        if (! exe_vsub2_l (varlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vsub2_d:
        PRINTD("VSUB2_D");
        
        if (! exe_vsub2_d (varlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vmul2_l:
        PRINTD("VMUL2_L");
        
        if (! exe_vmul2_l (varlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vmul2_d:
        PRINTD("VMUL2_D");
        
        if (! exe_vmul2_d (varlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vdiv2_l:
        PRINTD("VDIV2_L");
        
        if (! exe_vdiv2_l (varlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    vdiv2_d:
        PRINTD("VDIV2_D");
        
        if (! exe_vdiv2_d (varlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
        
        
    pvadd_l:
        PRINTD("PVADD_L");
        
        if (! exe_vadd_l (pvarlist, varlist, ARG1, vmreg.l[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvadd_d:
        PRINTD("PVADD_D");
        
        if (! exe_vadd_d (pvarlist, varlist, ARG1, vmreg.d[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvsub_l:
        PRINTD("PVSUB_L");
        
        if (! exe_vsub_l (pvarlist, varlist, ARG1, vmreg.l[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvsub_d:
        PRINTD("PVSUB_D");
        
        if (! exe_vsub_d (pvarlist, varlist, ARG1, vmreg.d[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvmul_l:
        PRINTD("PVMUL_L");
        
        if (! exe_vmul_l (pvarlist, varlist, ARG1, vmreg.l[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvmul_d:
        PRINTD("PVMUL_D");
        
        if (! exe_vmul_d (pvarlist, varlist, ARG1, vmreg.d[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvdiv_l:
        PRINTD("PVDIV_L");
        
        if (! exe_vdiv_l (pvarlist, varlist, ARG1, vmreg.l[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvdiv_d:
        PRINTD("PVDIV_D");
        
        if (! exe_vdiv_d (pvarlist, varlist, ARG1, vmreg.d[ARG2], ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
        
     /* VECTOR ARRAY ENGINE ADD ARRAY TO ARRAY ---------------------- */      
        
    pvadd2_l:
        PRINTD("PVADD2_L");
        
        if (! exe_vadd2_l (pvarlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvadd2_d:
        PRINTD("PVADD2_D");
        
        if (! exe_vadd2_d (pvarlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvsub2_l:
        PRINTD("PVSUB2_L");
        
        if (! exe_vsub2_l (pvarlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvsub2_d:
        PRINTD("PVSUB2_D");
        
        if (! exe_vsub2_d (pvarlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvmul2_l:
        PRINTD("PVMUL2_L");
        
        if (! exe_vmul2_l (pvarlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvmul2_d:
        PRINTD("PVMUL2_D");
        
        if (! exe_vmul2_d (pvarlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvdiv2_l:
        PRINTD("PVDIV2_L");
        
        if (! exe_vdiv2_l (pvarlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    pvdiv2_d:
        PRINTD("PVDIV2_D");
        
        if (! exe_vdiv2_d (pvarlist, varlist, ARG1, ARG2, ARG3, threadnum))
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
        
    /* ATAN --------------------------------------------------------- */

    atan2_d:
        PRINTD("ATAN2");

        vmreg.d[ARG3] = atan2 (vmreg.d[ARG1], vmreg.d[ARG2]);

        EXE_NEXT();
        
    /* POINTER ------------------------------------------------------ */

    ppointer:
        PRINTD("PPOINTER");

    ppointer_try:
        popcode = pcclist[ARG3 + poffset][0];

        /* check opcode */

        /*    printf ("exe_pointer: label: %li\n", label); */
    
        switch (popcode)
        {
            case LETARRAY2_I:
            case LETARRAY2_L:
            case LETARRAY2_Q:
            case LETARRAY2_D:
            case LETARRAY2_B:
            case LETARRAY2_S:

            case VAR_LOCK:
            case VAR_UNLOCK:
            case PMOVE_A_I:
            case PMOVE_A_L:
            case PMOVE_A_Q:
            case PMOVE_A_D:
            case PMOVE_A_B:
            case PMOVE_A_S:

            case MOVE_ADYN_Q:
            case MOVE_ADYN_D:
            case MOVE_ADYN_S:

            case PMOVE_Q_ADYN:
            case PMOVE_D_ADYN:
            case PMOVE_S_ADYN:

                /* setaddress */

                pcclist[ARG3 + poffset][1] = vmreg.l[ARG1];
                break;

            case LET2ARRAY_I:
            case LET2ARRAY_L:
            case LET2ARRAY_Q:
            case LET2ARRAY_D:
            case LET2ARRAY_B:
            case LET2ARRAY_S:

            case PMOVE_I_A:
            case PMOVE_L_A:
            case PMOVE_Q_A:
            case PMOVE_D_A:
            case PMOVE_B_A:
            case PMOVE_S_A:

            case MOVE_Q_ADYN:
            case MOVE_D_ADYN:
            case MOVE_S_ADYN:

            case PMOVE_ADYN_Q:
            case PMOVE_ADYN_D:
            case PMOVE_ADYN_S:

                /* setaddress */

                pcclist[ARG3 + poffset][2] = vmreg.l[ARG1];
                break;

            case SIZEOF:

                /* setaddress */

                pcclist[ARG3 + poffset][1] = vmreg.l[ARG1];
                break;

            default:
                /* wrong opcode */
            
                if (poffset == pmaxoffset)
                {
                    printf ("error: pointer wrong opcode!\n");

                    ERREXIT();
                }
            
                poffset++;
                goto ppointer_try;
        }

        EXE_NEXT();

        
    /* PGETTYPE ------------------------------------------------------ */

    pgettype:
        PRINTD("PGETTYPE");

        vmreg.l[ARG2] = pvarlist_obj[vmreg.l[ARG1]].type;

        EXE_NEXT();
        
     
    /* byte operations: byte = var[index] --------------------------- */
    
    move_ip_b:
        PRINTD("MOVE_IP_B");
        
        if (vmreg.l[ARG2] < 0 || vmreg.l[ARG2] > 2)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        vmreg.l[ARG3] = (U1) varlist[ARG1].i_m[vmreg.l[ARG2]];
        
        EXE_NEXT();
        
    move_lp_b:
        PRINTD("MOVE_LP_B");
        
        if (vmreg.l[ARG2] < 0 || vmreg.l[ARG2] > 4)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        vmreg.l[ARG3] = (U1) varlist[ARG1].li_m[vmreg.l[ARG2]];
        
        EXE_NEXT();

    move_qp_b:
       PRINTD("MOVE_QP_B");
       
       if (vmreg.l[ARG2] < 0 || vmreg.l[ARG2] > 8)
       {
           printerr (OVERFLOW_IND, epos, ST_EXE, "");
           ERREXIT();
       }
       
       vmreg.l[ARG3] = (U1) varlist[ARG1].q_m[vmreg.l[ARG2]];
        
       EXE_NEXT();
       
    move_dp_b:
       PRINTD("MOVE_DP_B");
       
       if (vmreg.l[ARG2] < 0 || vmreg.l[ARG2] > 8)
       {
           printerr (OVERFLOW_IND, epos, ST_EXE, "");
           ERREXIT();
       }
       
       vmreg.l[ARG3] = (U1) varlist[ARG1].d_m[vmreg.l[ARG2]];
        
       EXE_NEXT();
     
    move_b_ip:
        PRINTD("MOVE_B_IP");
        
        if (vmreg.l[ARG3] < 0 || vmreg.l[ARG3] > 2)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        varlist[ARG2].i_m[vmreg.l[ARG3]] = (U1) vmreg.l[ARG1];
        
        EXE_NEXT();
        
    move_b_lp:
        PRINTD("MOVE_B_LP");
        
        if (vmreg.l[ARG3] < 0 || vmreg.l[ARG3] > 4)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        varlist[ARG2].li_m[vmreg.l[ARG3]] = (U1) vmreg.l[ARG1];
        
        EXE_NEXT();
        
    move_b_qp:
        PRINTD("MOVE_B_QP");
        
        if (vmreg.l[ARG3] < 0 || vmreg.l[ARG3] > 8)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        varlist[ARG2].q_m[vmreg.l[ARG3]] = (U1) vmreg.l[ARG1];
        
        EXE_NEXT();
        
    move_b_dp:
        PRINTD("MOVE_B_DP");
        
        if (vmreg.l[ARG3] < 0 || vmreg.l[ARG3] > 8)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        varlist[ARG2].d_m[vmreg.l[ARG3]] = (U1) vmreg.l[ARG1];
        
        EXE_NEXT();
     
        
    /* PRIVATE VAR: ------------------------------------------------- */    
        
    pmove_ip_b:
        PRINTD("PMOVE_IP_B");
        
        if (vmreg.l[ARG2] < 0 || vmreg.l[ARG2] > 2)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        vmreg.l[ARG3] = (U1) pvarlist[ARG1].i_m[vmreg.l[ARG2]];
        
        EXE_NEXT();
        
    pmove_lp_b:
        PRINTD("PMOVE_LP_B");
        
        if (vmreg.l[ARG2] < 0 || vmreg.l[ARG2] > 4)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        vmreg.l[ARG3] = (U1) pvarlist[ARG1].li_m[vmreg.l[ARG2]];
        
        EXE_NEXT();

    pmove_qp_b:
       PRINTD("PMOVE_QP_B");
       
       if (vmreg.l[ARG2] < 0 || vmreg.l[ARG2] > 8)
       {
           printerr (OVERFLOW_IND, epos, ST_EXE, "");
           ERREXIT();
       }
       
       vmreg.l[ARG3] = (U1) pvarlist[ARG1].q_m[vmreg.l[ARG2]];
        
       EXE_NEXT();
       
    pmove_dp_b:
       PRINTD("PMOVE_DP_B");
       
       if (vmreg.l[ARG2] < 0 || vmreg.l[ARG2] > 8)
       {
           printerr (OVERFLOW_IND, epos, ST_EXE, "");
           ERREXIT();
       }
       
       vmreg.l[ARG3] = (U1) pvarlist[ARG1].d_m[vmreg.l[ARG2]];
        
       EXE_NEXT();
     
    pmove_b_ip:
        PRINTD("PMOVE_B_IP");
        
        if (vmreg.l[ARG3] < 0 || vmreg.l[ARG3] > 2)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        pvarlist[ARG2].i_m[vmreg.l[ARG3]] = (U1) vmreg.l[ARG1];
        
        EXE_NEXT();
        
    pmove_b_lp:
        PRINTD("PMOVE_B_LP");
        
        if (vmreg.l[ARG3] < 0 || vmreg.l[ARG3] > 4)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        pvarlist[ARG2].li_m[vmreg.l[ARG3]] = (U1) vmreg.l[ARG1];
        
        EXE_NEXT();
        
    pmove_b_qp:
        PRINTD("PMOVE_B_QP");
        
        if (vmreg.l[ARG3] < 0 || vmreg.l[ARG3] > 8)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        pvarlist[ARG2].q_m[vmreg.l[ARG3]] = (U1) vmreg.l[ARG1];
        
        EXE_NEXT();
        
    pmove_b_dp:
        PRINTD("PMOVE_B_DP");
        
        if (vmreg.l[ARG3] < 0 || vmreg.l[ARG3] > 8)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        pvarlist[ARG2].d_m[vmreg.l[ARG3]] = (U1) vmreg.l[ARG1];
        
        EXE_NEXT();
        
    password:
        PRINTD("PASSWORD");
        
        get_password (&vmreg.s[ARG1], vmreg.l[ARG2]);
        
        EXE_NEXT();
        
    makedir:
        PRINTD("MAKEDIR");
        
        i = exe_makedir (vmreg.s[ARG1]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, vmreg.s[ARG1]);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();
        
    /* MOD ---------------------------------------------------------- */

    mod_d:
        PRINTD("MOD_D");

        vmreg.d[ARG3] = fmod (vmreg.d[ARG1], vmreg.d[ARG2]);

        EXE_NEXT();
		
	/* HYPERSPACE --------------------------------------------------- */
		
	 hsvar:
        PRINTD("HSVAR");
        
        varlist[ARG1].hs_sock = vmreg.l[ARG2];
        
        EXE_NEXT();
		
    hsserver:
        PRINTD("HSSERVER");
        
        hsserver = vmreg.l[ARG1];
        
        #if OS_WINDOWS_CYGWIN	
        	if (pthread_create (&hs_thread, NULL, hyperspace_server, (void *) hsserver) != 0)
        	{
            		printf ("error: can't start hyperspace server thread!\n");
        	}
       	#else
       		#if OS_AROS
       			hs_thread = CreateThread (hyperspace_server, (void *) hsserver);
       		#else
       			if (pthread_create (&hs_thread, NULL, hyperspace_server, (void *) hsserver) != 0)
        		{
            			printf ("error: can't start hyperspace server thread!\n");
        		}
        	#endif
        #endif		
       			
        
        EXE_NEXT();
		
	hsload:
		PRINTD("HSLOAD");
		
		if (hs_load (varlist, ARG1, vmreg.l[ARG2]) != 0)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
        EXE_NEXT();
		
	hssave:
		PRINTD("HSSAVE");
		
		if (hs_save (varlist, ARG1, vmreg.l[ARG2]) != 0)
        {
            printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }
        
		EXE_NEXT();
		
	stpush_all_all:
		PRINTD("STPUSH_ALL_ALLL");

        for (i = 0; i < MAXVMREG; i++)
        {
            if (exe_stpush_l (threadnum, vmreg.l[i]) != ERR_STACK_OK)
            {
                printerr (get_sterr (i), epos, ST_EXE, "");
                ERREXIT();
            }
        }

        for (i = 0; i < MAXVMREG; i++)
        {
            if (exe_stpush_d (threadnum, vmreg.d[i]) != ERR_STACK_OK)
            {
                printerr (get_sterr (i), epos, ST_EXE, "");
                ERREXIT();
            }
        }
        
        for (i = 0; i < MAXVMREG; i++)
        {
            if (exe_stpush_s (threadnum, vmreg.s[i]) != ERR_STACK_OK)
            {
                printerr (get_sterr (i), epos, ST_EXE, "");
                ERREXIT();
            }
        }
        
        EXE_NEXT();
		
	stpull_all_all:
		PRINTD("STPULL_ALL_ALL");
	
		for (i = MAXVMREG - 1; i >= 0; i--)
        {
            if (exe_stpull_s (threadnum, (U1 *) &vmreg.s[i]) != ERR_STACK_OK)
            {
                printerr (get_sterr (i), epos, ST_EXE, "");
                ERREXIT();
            }
        }
        
        for (i = MAXVMREG - 1; i >= 0; i--)
        {
            if (exe_stpull_d (threadnum, &vmreg.d[i]) != ERR_STACK_OK)
            {
                printerr (get_sterr (i), epos, ST_EXE, "");
                ERREXIT();
            }
        }
        
        for (i = MAXVMREG - 1; i >= 0; i--)
        {
            if (exe_stpull_l (threadnum, &vmreg.l[i]) != ERR_STACK_OK)
            {
                printerr (get_sterr (i), epos, ST_EXE, "");
                ERREXIT();
            }
        }
        
        EXE_NEXT();
		
	jit:
		#if HAVE_JIT_COMPILER
			if (jit_on)
			{
				PRINTD("JIT");
		
				jit_compiler (&pcclist, &vmreg, &pvarlist, ARG1, ARG2 - 1);
				if (JIT_error == 1)
				{
					ERREXIT();
				}
			
				/* put NOP opcode here */
			
				// pcclist[epos][0] = NOP;
			
				/* put RUN_JIT_CODE opcode in at label ARG1 */
			
				// pcclist[ARG1][0] = RUN_JIT_CODE;
				// pcclist[ARG1][1] = JIT_code_ind;
				// pcclist[ARG1][2] = ARG2 - 1;
                
                /* copy opcodes to direct threading list */
                /*
                for (i = 0; i <= maxcclist; i++)
                {
                    dthread[i] = jumpt[pcclist[i][0]];
                }
                */
                
                epos = ARG2;
			}
		#endif
		
		EXE_NEXT();
		
	run_jit:
		#if HAVE_JIT_COMPILER
			if (jit_on)
			{
				PRINTD("RUNJIT");
			
				run_jit (ARG1, &vmreg);
			
				epos = ARG2;
			}
		#endif
			
		EXE_NEXT ();
		
	dobjects:
		PRINTD("DOBJECTS");
		
		if (exe_dobjects (vmreg.s[ARG1], &vmreg.l[ARG2]) != ERR_FILE_OK)
		{
			/* return -1 for directory objects on ERROR */
			
			vmreg.l[ARG2] = -1;
		}
		
		EXE_NEXT ();
		
	dnames:
		PRINTD("DNAMES");
		
		if (exe_dnames (vmreg.s[ARG1], varlist, ARG2) != ERR_FILE_OK)
		{
			printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();
		
	pdnames:
		PRINTD("PDNAMES");
		
		if (exe_dnames (vmreg.s[ARG1], pvarlist, ARG2) != ERR_FILE_OK)
		{
			printerr (OVERFLOW_IND, epos, ST_EXE, "");
            ERREXIT();
        }

        EXE_NEXT();	
		
	/* DLL / SO library load functions ======================================== */
	
	lopen:
        PRINTD("LIBRARY_OPEN");

        i = exe_lopen (&vmreg.l[ARG1], vmreg.s[ARG2]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();
		
	lclose:
        PRINTD("LIBRARY_CLOSE");

        i = exe_lclose (vmreg.l[ARG1]);
        if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();
		
	lfunc:
		PRINTD("LIBRARY_SET_FUNCTION");
		
		i = exe_lfunc (vmreg.l[ARG1], &vmreg.l[ARG2], vmreg.s[ARG3]);
		if (i != ERR_FILE_OK)
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
            else
            {
                printerr (get_ferr (i), epos, ST_EXE, filename);
                ERREXIT();
            }
        }
        else
        {
            if (*varlist[ERR_FILE].i_m == TRUE)
            {
                /* set _file */

                vmreg.l[ERR_FILE_R] = i;
            }
        }

        EXE_NEXT();
		
	lcall:
		PRINTD("LIBRARY_CALL_FUNCTION");
		
		exe_lcall (vmreg.l[ARG1], vmreg.l[ARG2], &pthreads, varlist, &vm_mem, threadnum, vmreg_vm.l[STSIZE]);
		
		EXE_NEXT ();
}
