/****************************************************************************
*
* Filename: main.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  main function
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

#include "main_d.h"
#include "arch.h"

#if OS_ANDROID
	#include <android/log.h>

	#define  LOG_TAG    "nanovm-error"

	#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
	#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif
	

#if DEBUG
    #define PRINTD(s);  if (print_debug) { printf ("%s\n", s); }
#else
    #define PRINTD(s);
#endif

/* wait_sec -------------------------------------------------------------- */

#if OS_AMIGA || OS_AROS
    #define WAIT_SEC();  Delay (15 * NANO_TIMER_TICKS);
#endif

#if OS_WINDOWS
    #define WAIT_SEC();  Sleep (15 * CLOCKS_PER_SEC);
#endif

#if OS_LINUX || OS_WINDOWS_CYGWIN || OS_ANDROID
    #define WAIT_SEC();  sleep (15);
#endif

extern struct shell_args shell_args;

/*
void wait_key (void)
{
    U1 buf[256];
    U1 len;
    
    while (1 == 1)
    {
    	buf[0] = BINUL;
    	printf ("type n for next step: ");
        fgets (buf, MAXSTRING_VAR, stdin);
       
        len = strlen (buf);
        
        printf ("len: %i\n", len);
        
        if (buf[0] == 'n')
        {
            break;
        }
        	
    }
}
*/

void showinfo (void)
{
    printf (VM_START_TXT);
    printcompilermsg ();

	#if HAVE_JIT_COMPILER
		printf ("byte storm JIT compiler inside. powered by AsmJit.\n");
		
		#if JIT_X86_32
			printf ("32 bit JIT compiler.\n");
		#else
			printf ("64 bit JIT compiler.\n");
		#endif
	#endif
	
    printf ("\nnanovm <program> <option>\n");
    printf ("options:\n");
    printf ("-q      no messages\n");
    printf ("-stacks=stacksize[KB]\n\n");
}


NINT main (NINT ac, char *av[])
{
    S2 exe_retval = 0, i, arg_ind = -1;
    U1 prog[MAXLINELEN + 1], prog_no[MAXLINELEN + 1], progpath[MAXLINELEN + 1], buf[MAXLINELEN + 1], arg[256];
	U1 rightspath[MAXLINELEN + 1];
    S4 threadnum, startpos;

	S2 vmname_start = 0;
	
    jumplist = NULL;
    includes = NULL;

    /* set vm sizes in registers */
    vmreg_vm.l[MEMBSIZE] = MAXMEMBSIZE;
    vmreg_vm.l[VMBSIZE] = MAXVMBSIZE;
    vmreg_vm.l[VMCACHESIZE] = MAXVMCACHESIZE;
    vmreg_vm.l[VMUSE] = FALSE;

    /* set stack size in register */
    vmreg_vm.l[STSIZE] = MAXSTSIZE;

    /* memory block index, normal variables */
    vm_mem.mblock_ind = 0;

    /* Cygwin posix threads stack size setting variables
     * original: Corinna Vinschen, found via the web
     */

    #if OS_WINDOWS_CYGWIN
        pthread_attr_t attr;
        pthread_attr_init (&attr);
        pthread_attr_setstacksize (&attr, 1024 * 1024);
    #endif

    #if OS_AROS
        /* open thread library */
        ThreadBase = (struct ThreadBase*) OpenLibrary ("thread.library", 0);
        if (ThreadBase == NULL)
        {
            printf ("error: can't open thread.library!\n");
            exe_shutdown (WARN);
        }
    #else   
    	/* INITIALIZE CCLIST MUTEX-STATE */
    	pthread_mutex_init (&cclist_mutex, NULL);
    #endif
    
	#if OS_ANDROID
		LOGD("nanovm running...\n");
	#endif
    
    /* check for no arguments or '?' */

    if (ac == 1)
    {
        showinfo ();
        exe_shutdown (WARN);
    }
    if (ac == 2)
    {
        if (av[1][0] == '?')
        {
            showinfo ();
            exe_shutdown (WARN);
        }
    }


    if (av[0][0] == '.') vmname_start = 2;	/* Linux: ./portnanovm */
	i = vmname_start;
    if (av[0][i] == 'p' && av[0][i + 1] == 'o' && av[0][i + 2] == 'r' && av[0][i + 3] == 't')
	{
		/* programname starts with "port" -> portable install on USB stick 
		 * 
		 * portnanovm or portnanovm.exe
		 * 
		 */
		
		portable_install = TRUE;
	}
	
	
	/* save arguments */

    if (ac > 2)
    {
        i = 2;
        while (i < ac)
        {
            if (av[i][0] == CLI_PREFIX)
            {
                if (av[i][1] == CLI_NO_MSG_SB)
                {
                    printmsg_no_msg = TRUE;
                }

                #if DEBUG
                    if (av[i][1] == CLI_DEBUG_SB)
                    {
                        print_debug = TRUE;
                    }
                #endif

                if (sscanf (av[i], "-stacks=%s", arg) > 0)
                {
                    if (! checkdigit (arg))
                    {
                        printf ("stacksize must be a number!\n");
                        exe_shutdown (WARN);
                    }

                    if (atoi (t_var.digitstr) > 0)
                    {
                        stack_cli = atoi (t_var.digitstr) * 1024;
                        printf ("stacksize: %li bytes\n", stack_cli);
                    }
                }
            }
            else
            {
                if (arg_ind < MAXSHELLARG - 1)
                {
                    if (strlen (av[i]) <= MAXLINELEN)
                    {
                        arg_ind++;
                        strcpy (shell_args.str[arg_ind], av[i]);
                    }
                }
            }
            i++;
        }
    }
    shell_args.args = arg_ind;

    if (! printmsg_no_msg)
    {
        printf (VM_START_TXT);
        printcompilermsg ();
		
		#if HAVE_JIT_COMPILER
		printf ("byte storm JIT compiler inside. powered by AsmJit.\n");
		
		#if JIT_X86_32
			printf ("32 bit JIT compiler.\n");
		#else
			printf ("64 bit JIT compiler.\n");
		#endif
	#endif
    }

    if (strlen (av[1]) > MAXLINELEN - 3)
    {
        printf ("error: filename to long!\n");
        exe_shutdown (WARN);
    }

    strcpy (prog, av[1]);
   

	/* DEBUG */
	/*
	S8 datahost = 12345678;
	S8 datanet, datahost_rec;
	
	datanet = htonq (datahost);
	
	printf ("datanet: %lli\n", datanet);
	
	datahost_rec = ntohq (datanet);
	
	printf ("data sent htonq/ntohq: %lli, data received: %lli\n", datahost, datahost_rec);
	if (datahost_rec != datahost) printf ("ERROR htonq/ntohq!!! data corrupt\n");
	*/
	/* DEBUG */
	
	
	
run_prog:
    PRINTD("memblock_gonext\n");

    memblock_gonext ();

    vm_mem.obj_maxvarlist = -1;     /* varlist not allocated */

    if (init_stack () != 0)
    {
        printf ("can't init stack!\n");
        exe_shutdown (WARN);
    }
    
    PRINTD("init_files\n");

    if (init_files () == FALSE)
    {
        printf ("init_files: error out of memory!\n");
        exe_shutdown (WARN);
    }
    
    PRINTD("init_sockets\n");

	init_dlls ();
    init_sockets ();
	
    /* load program */

    PRINTD("exe_open_object\n");

	strcpy (prog_no, prog);
	strcat (prog_no, ".no");
	
    i = exe_open_object (prog_no);
    if (i != TRUE)
    {
        if (i == -1)
        {
            /* bytecode verify during load failed, exit */

            printerr (PREP_FAIL, NOTDEF, ST_PRE, "");
            exe_shutdown (WARN);
        }

        PRINTD("checking ENV variables, doing autoconfig...\n");
        
        if (getenv (NANOVM_ROOT_SB) == NULL)
        {
			if (! portable_install)
			{
				printerr (ENV_NOT_SET, NOTDEF, ST_PRE, NANOVM_ROOT_SB);
				printf ("using default program path!\n");
			}
			
            if (OS_AMIGA || OS_AROS)
            {
                strcpy (buf, "Work:nanovm/prog/");
            }

            if (OS_LINUX)
            {
                strcpy (buf, "~/nanovm/prog/");
            }

            if (OS_ANDROID)
			{
				#if ANDROID_DEBUG
					strcpy (buf, "/data/local/tmp/nanovm/prog/");
				#else
					strcpy (buf, ANDROID_SDCARD);
					strcat (buf, "nanovm/prog/");
				#endif
			}
            
            if (OS_WINDOWS)
            {
                strcpy (buf, "C:/nanovm/prog/");
            }
            
            if (portable_install)
			{
				strcpy (buf, "../../prog/");
			}
        }
        else
        {
            strcpy (buf, getenv (NANOVM_ROOT_SB));
			strcat (buf, "/prog/");
        }

        if (strlen (buf) + strlen (prog) + 3 > MAXLINELEN)
        {
            printf ("error: filename to long!\n");
            exe_shutdown (WARN);
        }
        else
        {
            strcpy (progpath, buf);
            strcat (progpath, prog);
        }

        strcpy (rightspath, progpath);
		strcat (rightspath, ".nr");
        
		strcat (progpath, ".no");
		
        if (exe_open_object (progpath) != TRUE)
        {
            /* search portable install prog/ */
            
            strcpy (buf, "prog/");
            
            if (strlen (buf) + strlen (prog) + 3 > MAXLINELEN)
            {
                printf ("error: filename to long!\n");
                exe_shutdown (WARN);
            }
            else
            {
                strcpy (progpath, buf);
                strcat (progpath, prog);
            }
            
            strcpy (rightspath, progpath);
			strcat (rightspath, ".nr");
            
            strcat (progpath, ".no");
            
            if (exe_open_object (progpath) != TRUE)
            {
                printerr (OPEN, NOTDEF, ST_PRE, progpath);
                printerr (PREP_FAIL, NOTDEF, ST_PRE, "");
                exe_shutdown (WARN);
            }
        }
    }
    else
	{
		strcpy (rightspath, prog);
		strcat (rightspath, ".nr");
	}
    
	if (load_rights (rightspath) != TRUE)
	{
		printerr (OPEN, NOTDEF, ST_PRE, progpath);
        printerr (PREP_FAIL, NOTDEF, ST_PRE, "");
        exe_shutdown (WARN);
	}
    
    PRINTD("exe_verify_object...\n");
    
    if (! exe_verify_object ())
    {
        printerr (BYTECODE_VERIFY, NOTDEF, ST_PRE, prog);
        printerr (PREP_FAIL, NOTDEF, ST_PRE, "");
        exe_shutdown (WARN);
    }

    #if DEBUG
        if (print_debug)
        {
            printf ("\n\n");
             /* debug_varlist (varlist, varlist_state.maxvarlist); */
             
            debug_varlist (varlist, varlist_state.varlist_size - 1);
            debug_varlist (pvarlist_obj, pvarlist_obj_state.varlist_size - 1);
            debug_exelist ();
            printf ("\n\n");
        }
    #endif

    if (init_pthreads () != 0)
    {
        printf ("can't init pthreads!\n");
        exe_shutdown (WARN);
    }

    PRINTD("creating main thread...\n");
    
    if (maxcclist >= 0)
    {
        /* create main thread */

        threadnum = get_new_thread ();
        if (threadnum == -1)
        {
            printerr (PREP_FAIL, NOTDEF, ST_PRE, "");
            printf ("can't start thread!\n");
            exe_shutdown (WARN);
        }
        pthreads[threadnum].startpos = -1;

		PRINTD("got main thread number.\n");
		
        #if OS_WINDOWS_CYGWIN
            exe_retval = pthread_create (&pthreads[threadnum].thread, &attr, exe_elist, (void*) threadnum);
        #else
            #if OS_AROS
                pthreads[threadnum].thread = CreateThread (exe_elist_trampoline, (void *) threadnum);
            #else
				#if HAVE_THREADING
					exe_retval = pthread_create (&pthreads[threadnum].thread, NULL, exe_elist, (void*) threadnum);
				#else
					exe_retval = exe_elist (threadnum);
				#endif
            #endif
        #endif

        set_thread_running (threadnum);

        /* pthread_join (pthreads[threadnum].thread, NULL); */

        while (1)
        {
            if (get_thread_state (0) == PTHREAD_STOPPED)
            {
                /*  main thread stopped => EXIT */
                break;
            }

            threadnum = get_new_thread_startpos ();
            if (threadnum != -1)
            {
                #if OS_AROS
                    pthreads[threadnum].thread = CreateThread (exe_elist_trampoline, (void *) threadnum);
                #else
                    exe_retval = pthread_create (&pthreads[threadnum].thread, NULL, (void *) exe_elist, (void*) threadnum);
                #endif

                if (exe_retval == 0)
                {
                    set_thread_running (threadnum);
                }
                else
                {
                    set_thread_stopped (threadnum);
                }
            }

            PWAIT_TICK();
        }
    }
    else
    {
        printerr (PREP_FAIL, NOTDEF, ST_PRE, "");
    }

cleanup:
    #if DEBUG
        if (print_debug)
        {
            debug_varlist (varlist, varlist_state.maxvarlist);
            debug_exelist ();
        }
    #endif
    
	#if HAVE_JIT_COMPILER
		free_jit_code ();
	#endif
	
	close_dll_handles ();
		
    exe_shutdown (pthreads[0].ret_val);
}

