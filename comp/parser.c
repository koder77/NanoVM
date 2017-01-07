/****************************************************************************
*
* Filename: parser.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  N compiler: parser functions: token and source line handling
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
#include "parser.h"
#include "opcodes.h"
#include "comp_opcode.h"

struct vm_mem vm_mem;
struct memblock memblock[MAXMEMBLOCK];
struct vmreg_comp vmreg_comp;
struct vmreg_comp_info vmreg_comp_info;

/* dummy */
struct vmreg vmreg_vm;
struct file file[MAXFILES];
struct pthreads pthreads[MAXPTHREADS];

/* time-struct */

struct tm *tm;


struct assemb_set assemb_set;
struct src_line src_line;

struct function function[MAXFUNCTIONS];
struct if_comp if_comp[MAXIF];
struct for_comp for_comp[MAXFOR];

struct varlist *varlist;
struct varlist *pvarlist_obj;

struct varlist_state varlist_state =
{
    -1, -1, -1
};

struct varlist_state pvarlist_state =
{
    -1, -1, -1
};

S4 function_ind = -1;
S4 if_ind = -1;
S4 for_ind = -1;
S4 opcode_ind = -1;
S4 linenum;

// main function end, if sub function used
U1 function_main_end;

/* void *bclist[MAXCCLEN][MAXCCOMMARG];                 0 = opcode, 1 - 3 args, 4 = plist-line */
S4 **cclist = NULL;
S4 maxcclist;
S4 cclist_size = MAXCCLEN;
S4 cclist_ind = -1;


struct jumplist *jumplist;
S4 jumplist_ind = -1;
S4 maxjumplist;
S4 jumplist_size = MAXJUMPLEN;


struct vm_mem vm_mem;

struct vmreg vmreg;
struct vmreg_name vmreg_name;
struct t_op t_op;
struct arg arg;

/* plist - programm lines */
struct plist *plist;
S4 plist_ind;
S4 maxplist;
S4 plist_size;

struct t_var t_var;

/* include files */
struct includes *includes;
U2 includes_size = MAXINCLUDES;
S2 includes_ind = -1;

U1 programpath[MAXLINELEN + 1];

U1 assemb_inline = FALSE;            /* inline assembler */

U1 printmsg_no_msg = FALSE;         /* if true print no messages */
/* CLI -q option */

U1 nested_code = FALSE;             /* no goto used in function (opcode.c) */
U1 nested_code_global_off = FALSE;	/* nested code global off -> no nested code override automatically */

U1 atomic = FALSE;					/* atomic function call, no st_pull_all set automatically */

/* needed for setting the optimize flags from within code:
 * #OPTIMIZE_O
 * #OPTIMIZE_O2
 *
 */

U1 optimize_O = FALSE;				/* off */
U1 optimize_O2 = FALSE;				/* off */
U1 optimize_O3 = FALSE;             /* off */
U1 optimize_not = FALSE;            /* off */

#if DEBUG
    U1 print_debug = FALSE;         /* CLI -d option */
#endif

struct Library *ThreadBase = NULL;         /* thread.library descriptor */

U1 portable_install = FALSE;				/* dummy, used in VM */

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
    COMP_START_TXT,
    STATUS_OK_TXT,
    LOAD_PROG_TXT,
    SAVE_PROG_TXT,
};


S2 scan_token (U1 *buf)
{
    S2 i;

    for (i = 0; i < MAX_TOKENS; i++)
    {
        if (strcmp (buf, comp_opcode[i].op) == 0)
        {
            /* found opcode in opcode list */

            #if DEBUG
                printf ("scan token: found %li\n", i);
            #endif

            return (i);
        }
    }
    return (-1);    /* opcode not found */
}

S2 parse_line (U1 *str)
{
    S2 line_len, start = 0, i, pos, end_pos, search_end, comma_pos, equal_pos, arg_ind = 0;
    U1 ok, comma[2];
    U1 buf[MAXLINELEN + 1];
    S2 opcode;

    comma[0] = assemb_set.separ;
    comma[1] = BINUL;

    src_line.opcode_n = -1;

    line_len = strlen (str) - 1;

    if (line_len == 0)
    {
        return (-1);
    }

    #if DEBUG
        printf ("'%s'\n", str);
    #endif

    if (assemb_inline == TRUE)
    {
        /*  inline assembler */

        #if DEBUG
            printf ("parse_line: inline assembler!\n");
        #endif

        src_line.opcode_n = ASSEMB_INLINE;
        return (0);
    }

    if (strcmp (str, COMP_NESTED_CODE_OFF_SB) == 0)
    {
        src_line.opcode_n = COMP_NESTED_CODE_OFF;
        return (0);
    }

    if (strcmp (str, COMP_NESTED_CODE_ON_SB) == 0)
    {
        src_line.opcode_n = COMP_NESTED_CODE_ON;
        return (0);
    }

    if (strcmp (str, COMP_NESTED_CODE_GLOBAL_OFF_SB) == 0)
    {
        src_line.opcode_n = COMP_NESTED_CODE_GLOBAL_OFF;
        return (0);
    }

    if (strcmp (str, COMP_ATOMIC_START_SB) == 0)
	{
		src_line.opcode_n = COMP_ATOMIC_START;
        return (0);
    }

    if (strcmp (str, COMP_ATOMIC_END_SB) == 0)
	{
		src_line.opcode_n = COMP_ATOMIC_END;
        return (0);
    }

    if (strcmp (str, COMP_OPTIMIZE_O_SB) == 0)
	{
		src_line.opcode_n = COMP_OPTIMIZE_O;
        return (0);
    }

    if (strcmp (str, COMP_OPTIMIZE_O2_SB) == 0)
	{
		src_line.opcode_n = COMP_OPTIMIZE_O2;
        return (0);
    }

    if (strcmp (str, COMP_OPTIMIZE_O3_SB) == 0)
	{
		src_line.opcode_n = COMP_OPTIMIZE_O3;
        return (0);
    }

    if (strcmp (str, COMP_OPTIMIZE_NOT_START_SB) == 0)
	{
		src_line.opcode_n = COMP_OPTIMIZE_NOT_START;
        return (0);
    }
    
    if (strcmp (str, COMP_OPTIMIZE_NOT_END_SB) == 0)
	{
		src_line.opcode_n = COMP_OPTIMIZE_NOT_END;
        return (0);
    }
    
    if (str[0] == 'f')
    {
        /* check for func or funcend at beginning of a line */

        getstr (str, buf, MAXLINELEN, start, line_len);

        #if DEBUG
            printf ("'%s'\n", buf);
        #endif
        if (strcmp (buf, COMP_FUNC_SB) == 0)
        {
            #if DEBUG
                printf ("parse_line: FUNC\n");
            #endif

            src_line.opcode_n = COMP_FUNC;
        }
        if (strcmp (buf, COMP_FUNC_END_SB) == 0)
        {
            #if DEBUG
                printf ("parse_line: FUNC END\n");
            #endif

            src_line.opcode_n = COMP_FUNC_END;
            return (0);
        }

        /* check if func... */

        if (src_line.opcode_n != COMP_FUNC && src_line.opcode_n != COMP_FUNC_END)
        {
            goto check_function_call;
        }

        if (src_line.opcode_n == COMP_FUNC)
        {
            start = start + strlen (COMP_FUNC_SB) + 1;
        }
        if (src_line.opcode_n == COMP_FUNC_END)
        {
            start = start + strlen (COMP_FUNC_END_SB) + 1;
        }

        /* get function name */
        getstr (str, buf, MAXLINELEN, start, line_len);

        #if DEBUG
            printf ("'%s'\n", buf);
        #endif
        if (strlen (buf) == 0 || strcmp (buf, "()") == 0)
        {
            printf ("parse_line: ERROR no function name!\n");
            return (-1);
        }
        strcpy (src_line.arg[0], buf);      /* save function name */
        start = start + strlen (buf) + 1;

        /* get bracket ( */
        pos = searchstr (str, NBRACKET_OPEN_SB, start, line_len, TRUE);
        if (pos == -1)
        {
            printf ("parse_line: ERROR no ( found!\n");
            return (-1);
        }

        start = pos + 1;
        end_pos = searchstr (str, NBRACKET_CLOSE_SB, start, line_len, TRUE);
        if (end_pos == -1)
        {
            printf ("parse_line: ERROR no ) found!\n");
            return (-1);
        }

        /* check for () => empty arguments */
        if (start == end_pos)
        {
            #if DEBUG
                printf ("parse_line: EMPTY ARGUMENTS\n");
            #endif

            src_line.args = 0;
            return (0);
        }

        /* reading arguments */
        ok = TRUE;
        while (ok)
        {
            comma_pos = searchstr (str, comma, start, end_pos, TRUE);
            if (comma_pos > 0)
            {
                search_end = comma_pos -1;
            }
            else
            {
                search_end = end_pos - 1;
                ok = FALSE;
            }

            getstr (str, buf, MAXLINELEN, start, search_end);
            if (strlen (buf) == 0)
            {
                printf ("parse_line: ERROR no argument type!\n");
                return (-1);
            }

            if (arg_ind < MAXCOMMARG - 1)
            {
                arg_ind++;
            }
            strcpy (src_line.arg[arg_ind], buf);      /* save argument type */
            start = start + strlen (buf) + 1;

            #if DEBUG
                printf ("parse_line: arg type: '%s'\n", buf);
            #endif

            if (start == end_pos)
            {
                printf ("parse_line: ERROR no argument name!\n");
                return (-1);
            }

            getstr (str, buf, MAXLINELEN, start, search_end);
            if (strlen (buf) == 0)
            {
                printf ("parse_line: ERROR no argument name!\n");
                return (-1);
            }

            if (arg_ind < MAXCOMMARG - 1)
            {
                arg_ind++;
            }
            strcpy (src_line.arg[arg_ind], buf);      /* save argument name */
            start = comma_pos + 1;

            #if DEBUG
                printf ("parse_line: arg: '%s'\n", buf);
            #endif
        }
        src_line.args = arg_ind;

        if (function_ind < MAXFUNCTIONS && src_line.opcode_n == COMP_FUNC)
        {
            function_ind++;
            strcpy (function[function_ind].name, src_line.arg[0]);
            function[function_ind].start = linenum;
        }
        if (function_ind < MAXFUNCTIONS && src_line.opcode_n == COMP_FUNC_END)
        {
            function_ind++;
            function[function_ind].end = linenum;
        }
        return (0);
    }

    check_function_call:

    /* check for expression */
    equal_pos = searchstr (str, EQUAL_SB, start, line_len, TRUE);
    if (equal_pos != -1)
    {
        /* not a function call => get all tokens... */

        src_line.opcode_n = COMP_TOKEN; arg_ind = -1;

        ok = TRUE;
        while (ok)
        {
            getstr (str, buf, MAXLINELEN, start, line_len);
            if (strlen (buf) == 0)
            {
                /* no more token: end loop */
                ok = FALSE;
            }
            else
            {
                if (arg_ind < MAXCOMMARG - 1)
                {
                    arg_ind++;
                }
                strcpy (src_line.arg[arg_ind], buf);      /* save argument name */
                start = start + strlen (buf) + 1;

                opcode = scan_token (buf);
                if (opcode > -1)
                {
                    src_line.opcode_n = opcode;
                }

                #if DEBUG
                    printf ("parse_line: token %li: '%s'\n", arg_ind, buf);
                #endif
            }

            if (start > line_len)
            {
                ok = FALSE;
                /* no more token */
            }
        }
        src_line.args = arg_ind;
        return (0);
    }
    else
    {
        /* check for ( */

        pos = searchstr (str, NBRACKET_OPEN_SB, start, line_len, TRUE);
        if (pos == -1)
        {
            goto get_definition;
        }

        end_pos = searchstr (str, NBRACKET_CLOSE_SB, start, line_len, TRUE);
        if (end_pos == -1)
        {
            printf ("parse_line: ERROR NO ) found!\n");
            return (-1);
        }

        #if DEBUG
            printf ("parse_line: FUNC CALL\n");
        #endif

        arg_ind = 0;

        /* get function name */
        getstr (str, buf, MAXLINELEN, start, end_pos - 1);
        if (strlen (buf) == 0 || strcmp (buf, "()") == 0)
        {
            printf ("parse_line: ERROR no function name!\n");
            return (-1);
        }

		#if DEBUG
			printf ("'%s'\n", buf);
		#endif

		strcpy (src_line.arg[0], buf);      /* save function name */
        src_line.opcode_n = COMP_FUNC_CALL;

        /* check for () => empty arguments */
        if (end_pos == pos + 1)
        {
            #if DEBUG
                printf ("parse_line: EMPTY ARGUMENTS\n");
            #endif

            src_line.args = 0;
            return (0);
        }

        /* reading arguments */
        ok = TRUE, start = pos + 1;
        while (ok)
        {
            comma_pos = searchstr (str, comma, start, end_pos, TRUE);
            if (comma_pos > 0)
            {
                search_end = comma_pos -1;
            }
            else
            {
                search_end = end_pos - 1;
                ok = FALSE;
            }

            getstr (str, buf, MAXLINELEN, start, search_end);
            if (strlen (buf) == 0)
            {
                printf ("parse_line: ERROR no argument name!\n");
                return (-1);
            }

            if (arg_ind < MAXCOMMARG - 1)
            {
                arg_ind++;
            }
            strcpy (src_line.arg[arg_ind], buf);      /* save argument name */
            start = comma_pos + 1;

            #if DEBUG
                printf ("parse_line: arg: '%s'\n", buf);
            #endif
        }
        src_line.args = arg_ind;
        return (0);
    }

    get_definition:
        /* not a function call => get all tokens... (definitions: int x / byte x ... */

        src_line.opcode_n = COMP_TOKEN; arg_ind = -1;

        ok = TRUE;
        while (ok)
        {
            getstr (str, buf, MAXLINELEN, start, line_len);
            if (strlen (buf) == 0)
            {
                /* no more token: end loop */
                ok = FALSE;
            }
            else
            {
                if (arg_ind < MAXCOMMARG - 1)
                {
                    arg_ind++;
                    #if DEBUG
                        printf ("argind++: %li\n", arg_ind);
                    #endif
                }
                strcpy (src_line.arg[arg_ind], buf);      /* save argument name */
                start = start + strlen (buf) + 1;

                opcode = scan_token (buf);
                if (opcode > -1)
                {
                    src_line.opcode_n = opcode;
                }

                #if DEBUG
                    printf ("parse_line2: token: '%s'\n", buf);
                #endif
            }

            if (start > line_len)
            {
                ok = FALSE;
                /* no more token */
            }
        }
        src_line.args = arg_ind;

        #if DEBUG
            printf ("src_line.args: %li\n", src_line.args);
        #endif
    return (0);
}

void set_pline (S4 linenumber)
{
    if (cclist_ind > -1)
    {
        cclist[cclist_ind][BCELIST] = plist[linenumber].srcline;
        cclist[cclist_ind][INCLUDELIST] = plist[linenumber].include;
        cclist[cclist_ind][PLINELIST] = plist_ind;
    }
}

U1 exe_assemble (S4 start, S4 end)
{
    S2 line_len, pos;
    S2 ppos_start, ppos_end;       /* prep_pos */
    S4 i;

    U1 linestr[MAXLINELEN + 1], prepstr[MAXLINELEN + 1];
    U1 ok;
    U1 set_op;
    U1 ret_ok = TRUE;    /* return value */

    #if DEBUG
        if (print_debug)
        {
            printf ("\tprep_plist: start\n");
        }
    #endif

    for (i = start; i <= end; i++)
    {
        plist_ind = i;
        line_len = plist[i].line_len;
        ok = TRUE;

        #if DEBUG
            printf ("compile: [%li] %s\n", i, plist[i].memptr);
        #endif

        if (line_len > 0)
        {
            strcpy (linestr, plist[i].memptr);
            ppos_start = 0;
            ok = FALSE;

            #if DEBUG
                if (print_debug)
                {
                    printf ("exe_assemble: linestr: '%s'\n", linestr);
                }
            #endif

            while (! ok)
            {
                while (1 == 1)
                {
                    strcpy (prepstr, "");

                    /* search comment: */

                    pos = searchstr (linestr, REM_SB, 0, 0, TRUE);
                    if (pos != -1)
                    {
                        if (pos == 0)
                        {

                            /* at the beginning of the line = comment */
                            /* skip this */

                            #if DEBUG
                                if (print_debug)
                                {
                                    printf ("exe_assemble: comment\n");
                                }
                            #endif

                            ok = TRUE;
                            break;
                        }
                    }

                    pos = searchstr (linestr, COMP_ASSEMB_END_SB, 0, 0, TRUE);
                    if (pos != -1)
                    {
                        /* inline assembler end */

                        assemb_inline = FALSE;
                        ok = TRUE;
                        break;
                    }

                    pos = searchstr (linestr, COMP_ASSEMB_SB, 0, 0, TRUE);
                    if (pos != -1)
                    {
                        /* inline assembler start */

                        #if DEBUG
                            printf ("INLINE ASSEMBLER START\n");
                        #endif

                        assemb_inline = TRUE;
                        ok = TRUE;
                        break;
                    }

                    if (assemb_inline == FALSE)
                    {
                        if (linestr[0] == 'f')
                        {
                            if (searchstr (linestr, COMP_FUNC_END_SB, 0, strlen (linestr - 1), TRUE) != -1)
                            {
                                /* funcend */
                                strcpy (prepstr, COMP_FUNC_END_SB);
                            }
                            else
                            {
                                strcpy (prepstr, linestr);
                            }

                            parse_line (prepstr);
                            if (! compile ())
							{
								printf ("ERROR compile!\n");
								printf ("%s\n", prepstr);
								ret_ok = FALSE;
							}

                            set_pline (i);

                            ok = TRUE;
                            break;
                        }
                        else
                        {
                            ppos_end = searchpart (linestr, ppos_start, line_len - 1);      /* war line_len */
                            if (ppos_end != -1)
                            {
                                /* get the part before the ';' */

                                partstr (linestr, prepstr, ppos_start, ppos_end - 1);
                                ppos_start = ppos_end + 1;
                            }
                            else
                            {
                                ok = TRUE;
                                break;
                            }
                        }

                        skipspaces (prepstr, prepstr);

                        #if DEBUG
                            if (print_debug)
                            {
                                printf ("exe_assemble: prepstr: '%s'\n", prepstr);
                            }
                        #endif

                        parse_line (prepstr);
                        if (! compile ())
						{
							printf ("ERROR compile!\n");
							printf ("%s\n", prepstr);
							ret_ok = FALSE;
						}

                        set_pline (i);
                    }
                    else
                    {
                        /* inline assembler */
                        parse_line (linestr);
                        if (! compile ())
						{
							printf ("ERROR compile!\n");
							printf ("%s\n", linestr);
							ret_ok = FALSE;
						}

                        set_pline (i);
                        ok = TRUE;
                        break;
                    }
                }
            }
        }

        #if DEBUG
            if (print_debug)
            {
                if (ret_ok == FALSE)
                {
                    printf ("\nexe_assemble: ERROR RET_OK = FALSE!\n");
                }
            }
        #endif
    }
    return (ret_ok);
}

void showinfo (void)
{
    printf ("\nnanoc <source>\n\n");
}

NINT main (NINT ac, char *av[])
{
    U1 source[256], source_suffix[256], object[256], arg[256];
    U1 do_optimize = FALSE;
    U1 do_optimize_2 = FALSE;
	U1 do_optimize_3 = FALSE;
	U1 do_optimize_stack_only = FALSE;

	S4 vmname_start = 0, i;

    plist_size = MAXLINES;
    varlist_state.varlist_size = MAXVAR;
    pvarlist_state.varlist_size = MAXVAR;       /* thread private varlist */
    cclist_size = MAXCCLEN;
    jumplist_size = MAXJUMPLEN;

    /* set vm sizes in registers */
    vmreg_vm.l[MEMBSIZE] = MAXMEMBSIZE;
    vmreg_vm.l[VMBSIZE] = MAXVMBSIZE;
    vmreg_vm.l[VMCACHESIZE] = MAXVMCACHESIZE;
    vmreg_vm.l[VMUSE] = FALSE;

    /* set stack size in register */
    vmreg_vm.l[STSIZE] = MAXSTSIZE;

    /* memory block index, normal variables */
    vm_mem.mblock_ind = -1;

    /* quote and separator settings */

    assemb_set.quote = QUOTE_SB;
    assemb_set.separ = COMMA_SB;
    assemb_set.semicol = SEMICOL_SB;

    #if OS_AROS
        /* open thread library */
        ThreadBase = (struct ThreadBase*) OpenLibrary ("thread.library", 0);
        if (ThreadBase == NULL)
        {
            printf ("error: can't open thread.library!\n");
            exe_shutdown (WARN);
        }
    #endif

    printf (COMP_START_TXT);
    printcompilermsg ();

    if (ac == 2)
    {
        if (av[1][0] == '?')
        {
            showinfo ();
            exit (WARN);
        }
    }
    if (ac < 2)
    {
        showinfo ();
        exit (WARN);
    }


    if (av[0][0] == '.') vmname_start = 2;	/* Linux: ./portnanoa */
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


    if (strlen (av[1]) > 255 - 3)
    {
        printf ("error: filename to long!\n");
        exit (WARN);
    }

    if (ac == 3)
    {
        if (strcmp (av[2], "-O") == 0)
        {
            printf ("optimizer = ON\n");
            do_optimize = TRUE;
        }

        if (strcmp (av[2], "-O2") == 0)
        {
            printf ("optimizer = ON: O2\n");
            do_optimize = TRUE;
            do_optimize_2 = TRUE;
        }

        if (strcmp (av[2], "-O3") == 0)
        {
            printf ("optimizer = ON: O3\n");
            do_optimize = TRUE;
            do_optimize_2 = TRUE;
			do_optimize_3 = TRUE;
        }

        if (strcmp (av[2], "-OS") == 0)
        {
            printf ("optimizer = ON: OS\n");
            do_optimize_stack_only = TRUE;
        }
    }

	if (av[1][0] == '/' && av[1][1] == 'p' && av[1][2] == 'r' && av[1][3] == 'o' && av[1][4] == 'g')
	{
		/* /prog access: add nano root path */

		#if OS_ANDROID
			strcpy (source, ANDROID_SDCARD);
			strcat (source, "nanovm");
		#else
			/* check ENV variable */

			if (getenv (NANOVM_ROOT_SB) == NULL)
			{
				/* ENV not set use defaults */

				if (OS_AMIGA || OS_AROS)
				{
					strcpy (source, "Work:nanovm");
				}

				if (OS_LINUX)
				{
					strcpy (source, "~/nanovm");
				}

				if (OS_WINDOWS)
				{
					strcpy (source, "C:/nanovm");
				}
			}
			else
			{
				/* use ENV variable setting */
				strcpy (source, getenv (NANOVM_ROOT_SB));
			}

		#endif

		strcat (source, av[1]);
		strcpy (object, source);
		strcat (object, ".na");
	}
    else
	{

	#if OS_ANDROID
		/* because typing the full pathnames on mobile devices isn't fun! */

	#if	 ANDROID_DEBUG
		strcpy (source, "/data/local/tmp/nanovm/prog/");
	#else
		strcpy (source, ANDROID_SDCARD);
		strcat (source, "nanovm/prog/");
	#endif

	strcat (source, av[1]);
	strcpy (object, source);
	strcat (object, ".na");

	#else

    if (portable_install)
	{
		strcpy (source, "../../prog/");
		strcat (source, av[1]);

		strcpy (object, source);
		strcat (object, ".na");
	}
	else
	{
		strcpy (source, av[1]);
		strcpy (object, source);
		strcat (object, ".na");
	}

	#endif

	}

    if (init_files () == FALSE)
    {
        exe_shutdown (WARN);
    }

    memblock_gonext ();

    /* alloc lists ------------------------------------------------------- */

    plist = (struct plist *) malloc ((plist_size) * sizeof (struct plist));
    if (plist == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "lines");
        exe_shutdown (WARN);
    }

    init_plist (plist_size - 1);

    varlist = (struct varlist *) malloc ((varlist_state.varlist_size) * sizeof (struct varlist));
    if (varlist == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "variables");
        exe_shutdown (WARN);
    }

    vm_mem.obj_maxvarlist = varlist_state.varlist_size - 1;     /* for exe_dealloc_varlist */

    init_varlist (varlist, varlist_state.varlist_size - 1);

    if (! init_int_var (varlist, &varlist_state))
    {
        printerr (PREP_FAIL, NOTDEF, ST_PRE, "");
        exe_shutdown (WARN);
    }

    /* thread private varlist */

    pvarlist_obj = (struct varlist *) malloc ((pvarlist_state.varlist_size) * sizeof (struct varlist));
    if (pvarlist_obj == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "p-variables");
        exe_shutdown (WARN);
    }

    vm_mem.obj_maxpvarlist = pvarlist_state.varlist_size - 1;     /* for exe_dealloc_varlist */

    init_varlist (pvarlist_obj, pvarlist_state.varlist_size - 1);



    cclist = alloc_array_lint (cclist_size, MAXCCOMMARG);
    if (cclist == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "opcodes");
        exe_shutdown (WARN);
    }

    /* initialize NOOPTIM flag */
    for (i = 0; i < cclist_size; i++)
    {
        cclist[i][NOOPTIM] = 0;
    }
    
    jumplist = (struct jumplist *) malloc ((jumplist_size) * sizeof (struct jumplist));
    if (jumplist == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "labels");
        exe_shutdown (WARN);
    }

    init_jumplist ();

    init_if ();
    init_for ();

    /* includes */

    includes = NULL;

    includes = (struct includes *) malloc ((includes_size) * sizeof (struct includes));
    if (includes == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "includes");
        exe_shutdown (WARN);
    }

    strcpy (programpath, source);

    /* load program */

	strcpy (source_suffix, source);
	strcat (source_suffix, ".nanoc");

    if (! exe_load_prog (source_suffix))
    {
		printf ("error: no source with file extension .nanoc found\ntry old .nc extension...\n");
		strcpy (source_suffix, source);
		strcat (source_suffix, ".nc");

		if (! exe_load_prog (source_suffix))
		{
			printf ("ERROR can't load source file!\n");
			printerr (PREP_FAIL, NOTDEF, ST_PRE, "");
			exe_shutdown (WARN);
		}
    }

    if (! exe_assemble (0, maxplist))
    {
        printerr (PREP_FAIL, NOTDEF, ST_PRE, "");
        exe_shutdown (WARN);
    }

    #if DEBUG
        debug_varlist (varlist, varlist_state.varlist_size - 1);
        debug_exelist ();
    #endif

	if (do_optimize_stack_only)
	{
		printf ("optimizing stack only...\n");

		if (! optimize_stack ())
		{
			exe_shutdown (WARN);
		}
	}

    if (do_optimize || optimize_O)
    {
        printf ("optimizing...\n");

        if (! optimize ())
		{
			exe_shutdown (WARN);
		}

		if (! optimize_stack ())
		{
			exe_shutdown (WARN);
		}
    }

    if (do_optimize_2 || optimize_O2)
    {
        printf ("optimizing O2...\n");

        if (! optimize_2 ())
		{
			exe_shutdown (WARN);
		}

		if (! optimize_remove_double_pull ())
		{
			exe_shutdown (WARN);
		}
    }

    if (do_optimize_3 || optimize_O3)
	{
		printf ("optimizing O3...\n");

		if (! optimize_remove_pull_push ())
		{
			exe_shutdown (WARN);
		}
	}

    emit_code (object);
    exe_shutdown ((S2) NULL);
}
