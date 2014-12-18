/****************************************************************************
*
* Filename: main_d.h
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  struct definitions, global variables
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
#include "opcodes.h"


struct plist *plist;
struct memblock memblock[MAXMEMBLOCK];
struct varlist *varlist;
struct jumplist *jumplist;
struct t_var t_var;
struct t_op t_op;
struct file file[MAXFILES];
struct sock sock[MAXSOCKETS];
struct vm_mem vm_mem;
struct vmreg vmreg_vm;
struct includes *includes;

struct pthreads pthreads[MAXPTHREADS];

struct varlist *pvarlist_obj;

S4 **cclist = NULL;


/* stack */
U1 *stack = NULL;


/* time-struct */

struct tm *tm;

struct shell_args shell_args;


U1 filename[MAXSTRING_VAR + 1];            /* current filename */

U1 *errstr[] =
{
    OVERFLOW_VAR_TXT,
    OVERFLOW_STR_TXT,
    OVERFLOW_IND_TXT,
    WRONG_VARTYPE_TXT,
    NOTDEF_VAR_TXT,
    DOUBDEF_VAR_TXT,
    OPEN_TXT,
    SAVE_TXT,
    MEOF_TXT,
    BRNOT_OK_TXT,
    SYNTAX_TXT,
    MEMORY_TXT,
    VARLIST_TXT,
    PLIST_TXT,
    JUMPLIST_TXT,
    JUMPRECLIST_TXT,
    NOTDEF_LAB_TXT,
    DOUBDEF_LAB_TXT,
    PREP_FAIL_TXT,
    DIV_BY_ZERO_TXT,
    VM_FILE_TXT,
    OPERATOR_TXT,
    READ_TXT,
    WRITE_TXT,
    CLOSE_TXT,
    FNUMBER_TXT,
    FPOS_TXT,
    CCLIST_TXT,
    PEXIT_FAIL_TXT,
    UNKN_OPCODE_TXT,
    ENV_NOT_SET_TXT,
    WRONG_VERSION_TXT,
    BYTECODE_VERIFY_TXT,
    STACK_OVERFLOW_TXT,
    LINE_END_NOTSET_TXT,
    ERR_POINTER_TXT
};

U1 *msgstr[] =
{
    VM_START_TXT,
    STATUS_OK_TXT,
    LOAD_PROG_TXT,
    SAVE_PROG_TXT,
    VM_INIT_TXT
};

struct varlist_state varlist_state =
{
    -1, -1, -1
};

struct varlist_state pvarlist_obj_state =
{
    -1, -1, -1
};

S4 cclist_ind = -1, plist_ind = -1;
S4 maxcclist = -1, maxplist = -1, maxjumplist = -1;
S4 labnum = -1;

/* stack index */
S4 stack_ind = 0;

/* stack cli setting */
S4 stack_cli = 0;

/* stack elements */
S4 stack_elements = 0;

/* used by assembler */
S4 varlist_size;
S4 cclist_size = -1;
S4 plist_size = -1;

U1 printmsg_no_msg = FALSE;     /* if true print no messages */
/* CLI -q option */

#if DEBUG
    U1 print_debug = FALSE;     /* CLI -d option */
#endif

/* memory */

struct vm_mem vm_mem =
{
    FALSE, FALSE, 0, "", NULL, 0, -1, -1
};


U1 *obj_buf;                    /* openo.c, object file buffer */

U1 *version = VERSION_TXT;

/* include files */
U2 includes_size = MAXINCLUDES;
S2 includes_ind = -1;

/* hyperspace */
S4 hsserver;
#if OS_AROS
    uint32_t hs_thread;
#else
    pthread_t hs_thread;
#endif

#if OS_AROS
    struct Library *ThreadBase = NULL;         /* thread.library descriptor */
    
    void *cclist_mutex;
    void *pthreads_mutex;
    void *create_pthread_mutex;
    
    void *socket_mutex;
    
    void *file_mutex;
#else
    pthread_mutex_t cclist_mutex;	
    pthread_mutex_t pthreads_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t create_pthread_mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

S4 JIT_code_ind = -1;
S4 JIT_label_ind = -1;


/* file access = on, other rights switched off! */
/* see vm/rights.c */

struct rights rights =
{
	FALSE, TRUE, FALSE, FALSE
};

struct dlls dlls[MAXDLLS];

/* END */

