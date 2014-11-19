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

struct varlist *pvarlist_obj;       /* thread private varlist */

struct jumplist *jumplist;
struct t_var t_var;
struct t_op t_op;
struct vmreg vmreg;

/* dummy */
struct vmreg vmreg_vm;

struct vmreg_name vmreg_name;
struct src_line src_line;
struct arg arg;
struct assemb_set assemb_set;
struct vm_mem vm_mem;
struct includes *includes;
struct file file[MAXFILES];

/* stack (dummy) */
U1 *stack = NULL;


/* void *bclist[MAXCCLEN][MAXCCOMMARG];                 0 = opcode, 1 - 3 args, 4 = plist-line */
S4 **cclist = NULL;


/* time-struct */

struct tm *tm;


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
    A_START_TXT,
    STATUS_OK_TXT,
    LOAD_PROG_TXT,
    SAVE_PROG_TXT,
    VM_INIT_TXT
};

struct varlist_state varlist_state =
{
    -1, -1, -1
};

/* thread private varlist state */
struct varlist_state pvarlist_state =
{
    -1, -1, -1
};

S4 cclist_ind = -1, plist_ind = -1, jumplist_ind = -1;
S4 maxcclist = -1, maxplist = -1, maxjumplist = -1;

/* assembler only */
S4 plist_size;
S4 cclist_size;
S4 jumplist_size;

S4 labnum = -1;

U1 printmsg_no_msg = FALSE;         /* if true print no messages */
/* CLI -q option */

#if DEBUG
    U1 print_debug = FALSE;         /* CLI -d option */
#endif

/* memory */

struct vm_mem vm_mem =
{
    FALSE, FALSE, 0, "", NULL, -1
};


U1 *obj_buf;                        /* saveo.c, object file buffer */

U1 *version = VERSION_TXT;

/* include files */
U2 includes_size = MAXINCLUDES;
S2 includes_ind = -1;

U1 programpath[MAXLINELEN + 1];

struct pthreads pthreads[MAXPTHREADS];

/* nano compiler: inline assembler defs */
#define COMP_ASSEMB_SB                  "#ASSEMB"
#define COMP_ASSEMB_END_SB              "#ASSEMB_END"

#if OS_AROS
    struct Library *ThreadBase = NULL;         /* thread.library descriptor */
#endif

/* END */

