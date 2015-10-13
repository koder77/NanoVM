/****************************************************************************
*
* Filename: main.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  misc stuff
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

#define CC_NEXT();  if (! cclist_gonext ()) {return (FALSE);}

/* set 1, 2, or 3 argument command ---------------------------------------------------------- */

U1 set0 (S4 comm)
{
    CC_NEXT ();

    cclist[cclist_ind][0] = comm;

    return (TRUE);
}

U1 set1 (S4 comm, S4 vind1)
{
    CC_NEXT ();

    cclist[cclist_ind][0] = comm;
    cclist[cclist_ind][1] = vind1;

    return (TRUE);
}

U1 set2 (S4 comm, S4 vind1, S4 vind2)
{
    CC_NEXT ();

    cclist[cclist_ind][0] = comm;
    cclist[cclist_ind][1] = vind1;
    cclist[cclist_ind][2] = vind2;

    return (TRUE);
}

U1 set3 (S4 comm, S4 vind1, S4 vind2, S4 vind3)
{
    CC_NEXT ();

    cclist[cclist_ind][0] = comm;
    cclist[cclist_ind][1] = vind1;
    cclist[cclist_ind][2] = vind2;
    cclist[cclist_ind][3] = vind3;

    return (TRUE);
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

                    /* search nano COMPILER INLINE ASSEMBLY */

                    pos = searchstr (linestr, COMP_ASSEMB_SB, 0, 0, TRUE);
                    if (pos != -1)
                    {
                        if (pos == 0)
                        {
                            #if DEBUG
                                if (print_debug)
                                {
                                    printf ("exe_assemble: N INLINE ASSEMBLY START\n");
                                }
                            #endif

                            ok = TRUE;
                            break;
                        }
                    }

                    pos = searchstr (linestr, COMP_ASSEMB_END_SB, 0, 0, TRUE);
                    if (pos != -1)
                    {
                        if (pos == 0)
                        {
                            #if DEBUG
                                if (print_debug)
                                {
                                    printf ("exe_assemble: N INLINE ASSEMBLY END\n");
                                }
                            #endif

                            ok = TRUE;
                            break;
                        }
                    }

                    /* check for a end of line mark (semicolon) */

                    if (! checksemicol (linestr, 0, 0))
                    {
                        /* check if line is not a #include */

                        if (searchstr (linestr, INCLUDE_SB, 0, 0, TRUE) == -1)
                        {
							if (checkchars (linestr) == 1)
							{
								/* no semicolon, print a warning message */
								
								printerr (LINE_END_NOTSET, plist_ind, ST_PRE, "");
								ret_ok = FALSE;
							}
                        }
                    }

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

                    skipspaces (prepstr, prepstr);

                    #if DEBUG
                        if (print_debug)
                        {
                            printf ("exe_assemble: prepstr: '%s'\n", prepstr);
                        }
                    #endif

                    get_opcode (prepstr);
                    set_op = set_opcode ();

                    if (set_op != TRUE)
                    {
                        #if DEBUG
                            if (print_debug)
                            {
                                printf ("exe_assemble: set_opcode ERROR!!\n\n");
                            }
                        #endif

                        ret_ok = FALSE;

                        if (set_op == MEMORY)
                        {
                            #if DEBUG
                                if (print_debug)
                                {
                                    printf ("exe_assemble: set_opcode MEMORY!!\n\n");
                                }
                            #endif

                            ok = TRUE;
                            break;
                        }
                    }
                    else
                    {
                        set_pline (i);
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
    printf ("\nnanoa <source> -lines=<source lines> -ops=<opcodes> -vars=<variables> -labs=<labels> -includes=<files> -objs=<object bufsize[KB]> -s -d\n\n");
}

NINT main (NINT ac, char *av[])
{
    U1 lab_err = FALSE, found_exit = FALSE;
    U1 save_debug = TRUE;
    S4 i, j, obj_size = OBJBUFSIZE * 1024;
    U1 source[256], object[256], arg[256];

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

    printf (A_START_TXT);
    printcompilermsg ();

    /* arguments check */

    if (ac == 2)
    {
        if (av[1][0] == '?')
        {
            showinfo ();
            exit (WARN);
        }
    }
    if ((ac < 2) || (ac > 9))
    {
        showinfo ();
        exit (WARN);
    }

    /* source / object filename */

    if (strlen (av[1]) > 255 - 3)
    {
        printf ("error: filename to long!\n");
        exit (WARN);
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
		strcat (source, ".na");
		strcat (object, ".no");
	}
    else
	{
    
	#if OS_ANDROID
		/* because typing the full pathnames on mobile devices isn't fun! */

	#if ANDROID_DEBUG
		strcpy (source, "/data/local/tmp/nanovm/prog/");
	#else
		strcpy (source, ANDROID_SDCARD);
		strcat (source, "nanovm/prog/");
	#endif
	
	strcat (source, av[1]);
	strcpy (object, source);
	strcat (source, ".na");
	strcat (object, ".no");
	
	#else
	
    strcpy (source, av[1]);
    strcpy (object, source);
    strcat (source, ".na");
    strcat (object, ".no");
	
	#endif
	
	}
	
    if (init_files () == FALSE)
    {
        exe_shutdown (WARN);
    }

    if (ac > 2)
    {
        for (i = 2; i <= ac - 1; i++)
        {
            if (sscanf (av[i], "-lines=%s", arg) > 0)
            {
                if (! checkdigit (arg))
                {
                    printf ("lines must be a number!\n");
                    exe_shutdown (WARN);
                }

                plist_size = atoi (t_var.digitstr);
                printf ("lines: %li\n", plist_size);
            }

            if (sscanf (av[i], "-ops=%s", arg) > 0)
            {
                if (! checkdigit (arg))
                {
                    printf ("opcodes must be a number!\n");
                    exe_shutdown (WARN);
                }

                cclist_size = atoi (t_var.digitstr);
                printf ("opcodes: %li\n", cclist_size);
            }

            if (sscanf (av[i], "-vars=%s", arg) > 0)
            {
                if (! checkdigit (arg))
                {
                    printf ("variables must be a number!\n");
                    exe_shutdown (WARN);
                }

                varlist_state.varlist_size = atoi (t_var.digitstr);
                pvarlist_state.varlist_size = atoi (t_var.digitstr);
                printf ("variables: %li\n", varlist_state.varlist_size);
            }

            if (sscanf (av[i], "-labs=%s", arg) > 0)
            {
                if (! checkdigit (arg))
                {
                    printf ("labels must be a number!\n");
                    exe_shutdown (WARN);
                }

                jumplist_size = atoi (t_var.digitstr);
                printf ("labels: %li\n", jumplist_size);
            }

            if (sscanf (av[i], "-objs=%s", arg) > 0)
            {
                if (! checkdigit (arg))
                {
                    printf ("object bufsize must be a number!\n");
                    exe_shutdown (WARN);
                }

                obj_size = atoi (t_var.digitstr) * 1024;
                printf ("object bufsize: %li bytes\n", obj_size);
            }

            if (sscanf (av[i], "-includes=%s", arg) > 0)
            {
                if (! checkdigit (arg))
                {
                    printf ("includes must be a number!\n");
                    exe_shutdown (WARN);
                }

                includes_size = atoi (t_var.digitstr);
                printf ("includes: %li bytes\n", includes_size);
            }

            if (av[i][0] == CLI_PREFIX && av[i][1] == CLI_STRIP_DEBUG_SB)
            {
                save_debug = FALSE;
                printf ("strip debug symbols\n");
            }

            #if DEBUG
                if (av[i][0] == CLI_PREFIX && av[i][1] == CLI_DEBUG_SB)
                {
                    print_debug = TRUE;
                    printf ("print debug information\n");
                }
            #endif
        }
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

    jumplist = (struct jumplist *) malloc ((jumplist_size) * sizeof (struct jumplist));
    if (jumplist == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "labels");
        exe_shutdown (WARN);
    }

    init_jumplist ();
    init_regname_all ();


    /* includes */

    includes = NULL;

    includes = (struct includes *) malloc ((includes_size) * sizeof (struct includes));
    if (includes == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "includes");
        exe_shutdown (WARN);
    }

    /* used in loadp.c, includes */
    strcpy (programpath, source);

    /* load program */

    if (! exe_load_prog (source))
    {
        printerr (PREP_FAIL, NOTDEF, ST_PRE, "");
        exe_shutdown (WARN);
    }

    if (! exe_assemble (0, maxplist))
    {
        printerr (PREP_FAIL, NOTDEF, ST_PRE, "");
        exe_shutdown (WARN);
    }

    #if DEBUG
        if (print_debug)
        {
            printf ("assembler done.\n");
        }
    #endif

    /* insert labels, into opcode arguments with labels... */

    for (i = 0; i <= cclist_ind; i++)
    {
        for (j = 0; j <= opcode[cclist[i][0]].args; j++)
        {
            if (opcode[cclist[i][0]].type[j - 1] == LABEL)
            {
                if (jumplist[cclist[i][j]].pos != NOTDEF)
                {
                    cclist[i][j] = jumplist[cclist[i][j]].pos;
                }
                else
                {
                    printerr (NOTDEF_LAB, i, ST_EXE, jumplist[cclist[i][j]].lab);
                    lab_err = TRUE;
                }
            }
        }
    }

    if (lab_err)
    {
        exe_shutdown (WARN);
    }

    #if DEBUG
        if (print_debug)
        {
            printf ("labels inserted.\n");
        }
    #endif

    /* check if exit-opcode is set */

    for (i = 0; i <= cclist_ind; i++)
    {
        if (cclist[i][0] == PEXIT)
        {
            found_exit = TRUE;
            break;
        }
    }

    if (! found_exit)
    {
        printerr (PEXIT_FAIL, NOTDEF, ST_PRE, "");
        exe_shutdown (WARN);
    }

    #if DEBUG
        if (print_debug)
        {
            printf ("exit opcode found.\n");
        }
    #endif

    if (! exe_save_object (object, obj_size, save_debug))
    {
        printerr (PREP_FAIL, NOTDEF, ST_PRE, "");
        exe_shutdown (WARN);
    }

    #if DEBUG
        if (print_debug)
        {
            debug_varlist (varlist, varlist_state.maxvarlist);
            debug_varlist (pvarlist_obj, pvarlist_state.maxvarlist);
            debug_exelist ();
            debug_jumplist ();
        }
    #endif

    exe_shutdown ((S2) NULL);
}
