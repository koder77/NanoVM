/****************************************************************************
*
* Filename: loadp.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  program loading functions
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

extern struct plist *plist;

extern S4 plist_ind;
extern S4 plist_size;
extern S4 maxplist;

extern U2 includes_size;
extern S2 includes_ind;
extern struct includes *includes;

/* the full programpath: /home/stefan/nano/foo/foobar.na */
/* taken from invocation commandline */
extern U1 programpath[MAXLINELEN + 1];

#if DEBUG
    extern U1 print_debug;
#endif

U1 exe_load_prog (U1 *file)
{
    /* loading script-file */

    FILE *prog;
    U1 rbuf[MAXLINELEN + 1];                        /* read-buffer for one line */
    U1 argstr[MAXLINELEN + 1];
    U1 include_file[MAXLINELEN + 1];
    U1 ok, rem = FALSE, inc = FALSE;
    U1 *read;
    S2 size, pos, pos_start, pos_end, inc_type;     /* size (length + 1) of one line */
    S4 srcline = 1;
    S2 includes_ind_private;
	
	/* iniintialize plist */
	

    if (includes_ind < includes_size - 1)
    {
        includes_ind++;
        includes_ind_private = includes_ind;
        includes[includes_ind_private].name = malloc (strlen (file) + 1);
        if (includes[includes_ind_private].name == NULL)
        {
            printerr (MEMORY, NOTDEF, ST_PRE, "");
            return (FALSE);
        }

        strcpy (includes[includes_ind_private].name, file);
    }
    else
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "increase include files: use '-includes' option!");
        return (FALSE);
    }

    /* load a program into plist: */
	
    if ((prog = fopen (file, "r")) != NULL)
    {
        printmsg (LOAD_PROG, file);

        ok = TRUE;
        while (ok)
        {
            read = fgets_uni (rbuf, MAXLINELEN, prog);
            if (read != NULL && (plist_ind < plist_size - 1))
            {
                convtabs (rbuf);                    /* convert the funny tabs into spaces! */
                size = strlen (rbuf) + 1;

                pos = searchstr (rbuf, REM_SB, 0, 0, TRUE);
                if (pos != -1)
                {
                    if (pos == 0)
                    {

                        /* at the beginning of the line = comment */
                        /* skip this */

                        rem = TRUE;
                    }
                }
                else
                {
                    rem = FALSE;
                }

                pos = searchstr (rbuf, INCLUDE_SB, 0, 0, TRUE);
                if (pos != -1 && rem == FALSE)
                {
                    pos_start = pos + strlen (INCLUDE_SB) + 1;
                    pos_end = strlen (rbuf) - 1;

                    partstr (rbuf, argstr, pos_start, pos_end);
                    if (strlen (argstr) > 0)
                    {
                        inc = FALSE;

                        for (inc_type = 1; inc_type <= 6; inc_type++)
                        {
                            if (conv_include_name (argstr, include_file, inc_type))
                            {
                                if (strcmp (include_file, "") != 0)
                                {
                                    if (exe_load_prog (include_file))
                                    {
                                        inc = TRUE;
                                        break;
                                    }
                                }
                            }
                        }
                        if (! inc)
                        {
                            printerr (SYNTAX, NOTDEF, ST_PRE, rbuf);

                            #if DEBUG
                                if (print_debug)
                                {
                                    printf ("exe_load_prog: syntax\n");
                                }
                            #endif

                            fclose (prog);
                            return (FALSE);
                        }
                    }
                }

                if (strlen (rbuf) > 0)
                {
                    if (rbuf[0] != '\n')
                    {
                        /* put the line in plist */

                        plist_ind++;

                        if (plist_ind >= plist_size)
                        {
                            printerr (MEMORY, NOTDEF, ST_PRE, "increase source lines: use '-lines' option!");
                            fclose (prog);
                            return (FALSE);
                        }

                        plist[plist_ind].memptr = (U1 *) malloc (size * sizeof (U1));
                        if (plist[plist_ind].memptr != NULL)
                        {
                            plist[plist_ind].line_len = strlen (rbuf);
                            strcpy (plist[plist_ind].memptr, rbuf);
                            plist[plist_ind].srcline = srcline;             /* save real source-linenumber */
                            plist[plist_ind].include = includes_ind_private;        /* save include file-index */
                        }
                        else
                        {
                            printerr (MEMORY, NOTDEF, ST_EXE, "");
                            fclose (prog);
                            return (FALSE);
                        }
                    }
                    else
                    {
                        /* empty */

                        plist_ind++;

                        if (plist_ind >= plist_size)
                        {
                            printerr (MEMORY, NOTDEF, ST_PRE, "increase source lines: use '-lines' option!");
                            fclose (prog);
                            return (FALSE);
                        }

                        plist[plist_ind].memptr = NULL;
                        plist[plist_ind].line_len = 0;
                    }
                }
                else
                {
                    /* empty */

                    plist_ind++;

                    if (plist_ind >= plist_size)
                    {
                        printerr (MEMORY, NOTDEF, ST_PRE, "increase source lines: use '-lines' option!");
                        fclose (prog);
                        return (FALSE);
                    }

                    plist[plist_ind].memptr = NULL;
                    plist[plist_ind].line_len = 0;
                }
                srcline++;
            }
            else
            {
                ok = FALSE;
            }
        }
        fclose (prog);
        maxplist = plist_ind;
        printmsg (STATUS_OK, "");
        return (TRUE);
    }
    else
    {
        printerr (OPEN, NOTDEF, ST_EXE, file);
        return (FALSE);
    }
}

void conv_programpath (U1 *outputstr)
{
    S2 i;
    S2 len;
    S2 endpos;
    U1 ch;
    
    len = strlen (programpath);
    
    /* get path: */
    /* /home/stefan/prog/foo/foobar.na -> /home/stefan/prog/foo/ */
    
    for (i = len - 1; i >= 0; i--)
    {
        ch = programpath[i];
        
        if (ch == '/')
        {
            endpos = i;
            break;
        }
    }

    for (i = 0; i <= endpos; i++)
    {
        outputstr[i] = programpath[i];
    }
    outputstr[i] = BINUL;
}

U1 conv_include_name (U1 *str, U1 *file, S2 type)
{
    U1 comm[2], buf[MAXLINELEN + 1];
    S2 size, pos, pos_start, pos_end;

    comm[0] = LESS_SB;
    comm[1] = BINUL;
    pos_start = 0;
    pos_end = strlen (str);

    pos = searchstr (str, comm, pos_start, pos_end, TRUE);
    if (pos == -1)
    {
        return (FALSE);
    }

    pos_start = pos + 1;
    comm[0] = GREATER_SB;
    comm[1] = BINUL;

    pos = searchstr (str, comm, pos_start, pos_end, TRUE);
    if (pos == -1)
    {
        return (FALSE);
    }
    partstr (str, buf, pos_start, pos - 1);

    /* put the assign-name at the begin of file */

    switch (type)
    {
        case 1:
            if (getenv (NANOVM_ROOT_SB) == NULL)
            {
                /* NO env variable set, use defaults! */

                printerr (ENV_NOT_SET, NOTDEF, ST_EXE, NANOVM_ROOT_SB);
                printf ("using default include path!\n");

                if (OS_AMIGA || OS_AROS)
                {
                    strcpy (file, "Work:nanovm/include/");
                }

                if (OS_LINUX)
                {
                    strcpy (file, "~/nanovm/include/");
                }
                
                if (OS_ANDROID)
				{
					#if ANDROID_DEBUG
						strcpy (file, "/data/local/tmp/nanovm/include/");
					#else
						strcpy (file, ANDROID_SDCARD);
						strcat (file, "nanovm/include/");
					#endif
				}

                if (OS_WINDOWS)
                {
                    strcpy (file, "C:/nanovm/include/");
                }
            }
            else
            {

                /* check length of env variable */

                if (strlen (getenv (NANOVM_ROOT_SB)) > MAXLINELEN)
                {
                    /* path too long!!! */

                    return (FALSE);
                }

                strcpy (file, getenv (NANOVM_ROOT_SB));
				strcat (file, "/include/");
            }

            size = strlen (file) + strlen (buf);
            if (size > MAXLINELEN)
            {
                /* filename to long!!! */

                return (FALSE);
            }
            else
            {
                strcat (file, buf);
            }
            break;

        case 2:
            if (getenv (NANOVM_ROOT_SB) == NULL)
            {
                printerr (ENV_NOT_SET, NOTDEF, ST_EXE, NANOVM_ROOT_SB);
                printf ("using default program path!\n");

                if (OS_AMIGA || OS_AROS)
                {
                    strcpy (file, "Work:nanovm/prog/");
                }

                if (OS_LINUX)
                {
                    strcpy (file, "~/nanovm/prog/");
                }

                if (OS_WINDOWS)
                {
                    strcpy (file, "C:/nanovm/prog/");
                }
            }
            else
            {
                /* check length of env variable */

                if (strlen (getenv (NANOVM_ROOT_SB)) > MAXLINELEN)
                {
                    /* path too long!!! */

                    return (FALSE);
                }

                strcpy (file, getenv (NANOVM_ROOT_SB));
				strcat (file, "/prog/");
            }

            size = strlen (file) + strlen (buf);
            if (size > MAXLINELEN)
            {
                /* filename to long!!! */

                return (FALSE);
            }
            else
            {
                strcat (file, buf);
            }
            break;
            
        case 3:
            conv_programpath (file);
            strcat (file, buf);
            break;
            
        case 4:
            strcpy (file, buf);
            break;
            
        case 5:
            // portable install: includes
            
            strcpy (file, "include/");
            
            size = strlen (file) + strlen (buf);
            if (size > MAXLINELEN)
            {
                /* filename to long!!! */

                return (FALSE);
            }
            else
            {
                strcat (file, buf);
            }
            break;

        case 6:
            // portable install: prog
            
            strcpy (file, "prog/");
            
            size = strlen (file) + strlen (buf);
            if (size > MAXLINELEN)
            {
                /* filename to long!!! */

                return (FALSE);
            }
            else
            {
                strcat (file, buf);
            }
            break;
    }
    return (TRUE);
}
