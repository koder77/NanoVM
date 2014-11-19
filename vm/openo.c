/****************************************************************************
*
* Filename: openo.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  open object code
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

extern struct opcode opcode[];
extern struct varlist *varlist;
extern struct varlist_state varlist_state;

extern struct varlist *pvarlist_obj;                /* thread private varlist master obj */
extern struct varlist_state pvarlist_obj_state;

extern struct vm_mem vm_mem;

extern U1 open_err;
extern S4 **cclist;
extern S4 cclist_ind;
extern S4 maxcclist;

extern U1 *obj_buf;

extern U2 includes_size;
extern S2 includes_ind;
extern struct includes *includes;

extern struct jumplist *jumplist;
extern S4 maxjumplist;


#define OPEN_ERR();     printerr (OPEN, NOTDEF, ST_EXE, file); if (obj_open) { fclose (obj); } if (obj_buf) { free (obj_buf); } return (FALSE);
#define VERIFY_ERR();   printerr (BYTECODE_VERIFY, NOTDEF, ST_EXE, file); if (obj_open) { fclose (obj); } if (obj_buf) { free (obj_buf); } return (-1);

#define CHECK_POS();    if ((pos + size) > obj_size) { free (obj_buf); obj_buf = NULL; OPEN_ERR (); }

#if DEBUG
    extern U1 print_debug;
#endif

S2 exe_open_object (U1 *file)
{
    FILE *obj;
    U1 buf[MAXSTRING_VAR + 1], obj_open = FALSE;
    S4 i, j, dim_ind, version, n, li_var, obj_size, read_size, pos = 0;
    S2 size, i_var, ch, debug = NOTDEF, wordcode;
    S8 q_var;
    F8 d_var;

    if ((obj = fopen (file, "rb")) != NULL)
    {
        obj_open = TRUE;
        printmsg (LOAD_PROG, file);

        /* get filesize */

        fseek (obj, (long) NULL, SEEK_END);
        fgetpos (obj, (fpos_t *) &obj_size);
        fseek (obj, (long) NULL, SEEK_SET);

        obj_buf = NULL;
        obj_buf = (U1 *) malloc (obj_size * sizeof (U1));
        if (obj_buf == NULL)
        {
            /* no mem */

            OPEN_ERR ();
        }

        /* read file to buf */

        read_size = fread (obj_buf, sizeof (U1), obj_size, obj);
        if (read_size == EOF || read_size == 0 || read_size != obj_size)
        {
            OPEN_ERR ();
        }
        fclose (obj);
        obj_open = FALSE;

        /* read header ------------------------------------------------------- */

        size = OBJECTHD_LEN;
        CHECK_POS ();

        read_string (obj_buf, buf, pos, size);
        pos = pos + OBJECTHD_LEN;

        if (strcmp (buf, DEBUG_OBJECTHD_SB) == 0)
        {
            debug = TRUE;
        }

        if (strcmp (buf, NORM_OBJECTHD_SB) == 0)
        {
            debug = FALSE;
        }

        if (debug == NOTDEF)
        {
            /* unknown header */

            OPEN_ERR ();
        }

        /* read version number ----------------------------------------------- */

        size = sizeof (S4);
        CHECK_POS ();

        n = read_lint (obj_buf, pos);
        pos = pos + size;

        #if BS_LITTLE_ENDIAN
            version = conv_lint (n);
        #else
            version = n;
        #endif

        if (version != VERSION_OBJECT)
        {
            printerr (WRONG_VERSION, NOTDEF, ST_PRE, "");
            OPEN_ERR ();
        }

        /* read opcode number ------------------------------------------------ */

        CHECK_POS ();

        n = read_lint (obj_buf, pos);
        pos = pos + size;

        #if BS_LITTLE_ENDIAN
            maxcclist = conv_lint (n);
        #else
            maxcclist = n;
        #endif

        #if DEBUG
            if (print_debug)
            {
                printf ("opcodes: %li\n", maxcclist);
            }
        #endif

        /* read variable number ---------------------------------------------- */

        CHECK_POS ();

        n = read_lint (obj_buf, pos);
        pos = pos + size;

        #if BS_LITTLE_ENDIAN
            vm_mem.obj_maxvarlist = conv_lint (n);
        #else
            vm_mem.obj_maxvarlist = n;
        #endif

        varlist_state.varlist_size = vm_mem.obj_maxvarlist;

        #if DEBUG
            if (print_debug)
            {
                printf ("variables: %li\n", vm_mem.obj_maxvarlist);
            }
        #endif

        /* read private variable number ---------------------------------------------- */

        CHECK_POS ();

        n = read_lint (obj_buf, pos);
        pos = pos + size;

        #if BS_LITTLE_ENDIAN
            vm_mem.obj_maxpvarlist = conv_lint (n);
        #else
            vm_mem.obj_maxpvarlist = n;
        #endif

        pvarlist_obj_state.varlist_size = vm_mem.obj_maxpvarlist;

        #if DEBUG
            if (print_debug)
            {
                printf ("thread private variables: %li\n", vm_mem.obj_maxpvarlist);
            }
        #endif

		/* read maxjumplist ----------------------------------------------------- */
		
		CHECK_POS ();

        n = read_lint (obj_buf, pos);
        pos = pos + size;

        #if BS_LITTLE_ENDIAN
            maxjumplist = conv_lint (n);
        #else
            maxjumplist = n;
        #endif

		#if DEBUG
            if (print_debug)
            {
                printf ("maxjumplist: %li\n", maxjumplist);
            }
        #endif
        
        jumplist = (struct jumplist *) malloc ((maxjumplist + 1) * sizeof (struct jumplist));
        if (jumplist == NULL)
        {
            printerr (MEMORY, NOTDEF, ST_PRE, "");
            OPEN_ERR ();
        }

        #if DEBUG
            if (print_debug)
            {
                printf ("jumplist alloced\n");
            }
        #endif
        
        /* alloc var list ------------------------------------------------------- */

        varlist = (struct varlist *) malloc ((vm_mem.obj_maxvarlist + 1) * sizeof (struct varlist));
        if (varlist == NULL)
        {
            printerr (MEMORY, NOTDEF, ST_PRE, "");
            OPEN_ERR ();
        }

        #if DEBUG
            if (print_debug)
            {
                printf ("init_varlist\n");
            }
        #endif

        init_varlist (varlist, vm_mem.obj_maxvarlist);

        #if DEBUG
            if (print_debug)
            {
                printf ("init_tmp_var\n");
            }
        #endif

        if (! init_int_var (varlist, &varlist_state))
        {
            OPEN_ERR ();
        }


        /* alloc thread private varlist master -------------------------------- */

        pvarlist_obj = (struct varlist *) malloc ((vm_mem.obj_maxpvarlist + 1) * sizeof (struct varlist));
        if (pvarlist_obj == NULL)
        {
            printerr (MEMORY, NOTDEF, ST_PRE, "");
            OPEN_ERR ();
        }

        #if DEBUG
            if (print_debug)
            {
                printf ("init_varlist\n");
            }
        #endif

        init_varlist (pvarlist_obj, vm_mem.obj_maxpvarlist);


        /* alloc programm list ------------------------------------------------ */

        cclist = (S4 **) alloc_array_lint (maxcclist + 1, MAXCCOMMARG);
        if (cclist == NULL)
        {
            printerr (MEMORY, NOTDEF, ST_PRE, "");
            OPEN_ERR ();
        }

        /* read opcodes ------------------------------------------------------ */

        if (maxcclist < 0)
        {
            goto finish;
        }

        #if DEBUG
            if (print_debug)
            {
                printf ("object reading opcodes...\n");
            }
        #endif

        for (i = 0; i <= maxcclist; i++)
        {
            /* read WORDCODE: OPCODE = 2 bytes */
            size = sizeof (S2);
            CHECK_POS ();

            wordcode = read_int (obj_buf, pos);
            pos = pos + size;

            #if BS_LITTLE_ENDIAN
                cclist[i][0] = conv_int (wordcode);
            #else
                cclist[i][0] = wordcode;
            #endif

            /* check wordcode */

            if (cclist[i][0] < 0 || cclist[i][0] > MAXOPCODE)
            {
                /* opcode not in legal range */

                VERIFY_ERR ();
            }

            if (opcode[cclist[i][0]].args > 0)
            {
                for (j = 1; j <= opcode[cclist[i][0]].args; j++)
                {
                    if (opcode[cclist[i][0]].type[j - 1] == L_REG || opcode[cclist[i][0]].type[j - 1] == D_REG)
                    {
                        size = 1;
                        CHECK_POS ();

                        cclist[i][j] = obj_buf[pos];
                        pos = pos + size;
                    }
                    else
                    {
                        size = sizeof (S4);
                        CHECK_POS ();

                        n = read_lint (obj_buf, pos);
                        pos = pos + size;

                        #if BS_LITTLE_ENDIAN
                            cclist[i][j] = conv_lint (n);
                        #else
                            cclist[i][j] = n;
                        #endif
                    }
                }
            }
        }

        /* read varlist ------------------------------------------------------ */

        #if DEBUG
            if (print_debug)
            {
                printf ("obj_buf pos: %li\n", pos);
                printf ("object reading varlist, maxvarlist: %li\n", vm_mem.obj_maxvarlist);
            }
        #endif

        if (vm_mem.obj_maxvarlist < PROG_VARLIST_START)
        {
            goto finish;
        }

        for (i = PROG_VARLIST_START; i <= vm_mem.obj_maxvarlist; i++)
        {
            #if DEBUG
                if (print_debug)
                {
                    printf ("reading variable %li...\n", i);
                }
            #endif

            size = 1;
            CHECK_POS ();

            varlist[i].type = obj_buf[pos];

            #if DEBUG
                if (print_debug)
                {
                    printf ("pos: %li\n", pos);
                }
            #endif

            pos = pos + size;

            #if DEBUG
                if (print_debug)
                {
                    printf ("variable type: ");

                    switch (varlist[i].type)
                    {
                        case INT_VAR:
                            printf ("INT\n");
                            break;

                        case LINT_VAR:
                            printf ("LONGINT\n");
                            break;

                        case QINT_VAR:
                            printf ("QINT\n");
                            break;

                        case DOUBLE_VAR:
                            printf ("DOUBLE\n");
                            break;

                        case STRING_VAR:
                            printf ("STRING\n");
                            break;

                        case BYTE_VAR:
                            printf ("BYTE\n");
                            break;
                            
                        case DYNAMIC_VAR:
                            printf ("DYNAMIC\n");
                            break;

                        default:
                            printf ("%i\n", varlist[i].type);
                            break;
                    }
                }
            #endif


            /* check variable type */

            switch (varlist[i].type)
            {
                case INT_VAR:
                case LINT_VAR:
                case QINT_VAR:
                case DOUBLE_VAR:
                case STRING_VAR:
                case BYTE_VAR:
                case DYNAMIC_VAR:
                    break;

                default:
                    /* wrong variable type */

                    VERIFY_ERR ();
                    break;
            }


            size = sizeof (S4);
            CHECK_POS ();

            n = read_lint (obj_buf, pos);
            pos = pos + size;

            #if BS_LITTLE_ENDIAN
                varlist[i].dims = conv_lint (n);
            #else
                varlist[i].dims = n;
            #endif

            if (varlist[i].dims != NODIMS)
            {
                /* read dimensions */

                #if DEBUG
                    if (print_debug)
                    {
                        printf ("variable dims: %li\n", varlist[i].dims);
                    }
                #endif

                size = sizeof (S4);

                for (j = 0; j <= varlist[i].dims; j++)
                {
                    CHECK_POS ();

                    n = read_lint (obj_buf, pos);
                    pos = pos + size;

                    #if BS_LITTLE_ENDIAN
                        dim_ind = conv_lint (n);
                    #else
                        dim_ind = n;
                    #endif

                    varlist[i].dimens[j] = dim_ind;
                }
            }

            /* read type: VAR, CONST_VAR ------------------------------------- */

            size = 1;
            CHECK_POS ();

            ch = obj_buf[pos];
            pos = pos + size;

            varlist[i].constant = ch;
            
            if (ch == CONST_VAR)
            {
                #if DEBUG
                    if (print_debug)
                    {
                        printf ("allocating var: %li\n", i);
                    }
                #endif

                if (! exe_alloc_var (varlist, i, varlist[i].type))
                {
                    printerr (MEMORY, NOTDEF, ST_PRE, "");
                    OPEN_ERR ();
                }

                switch (varlist[i].type)
                {
                    case INT_VAR:
                        size = sizeof (S2);
                        CHECK_POS ();

                        i_var = read_int (obj_buf, pos);
                        pos = pos + size;

                        #if BS_LITTLE_ENDIAN
                            *varlist[i].i_m = conv_int (i_var);
                        #else
                            *varlist[i].i_m = i_var;
                        #endif

                        break;

                    case LINT_VAR:
                        size = sizeof (S4);
                        CHECK_POS ();

                        li_var = read_lint (obj_buf, pos);
                        pos = pos + size;

                        #if BS_LITTLE_ENDIAN
                            *varlist[i].li_m = conv_lint (li_var);
                        #else
                            *varlist[i].li_m = li_var;
                        #endif

                        break;

                   case QINT_VAR:
                        size = sizeof (S8);
                        CHECK_POS ();

                        q_var = read_qint (obj_buf, pos);
                        pos = pos + size;

                        #if BS_LITTLE_ENDIAN
                            *varlist[i].q_m = conv_qint (q_var);
                        #else
                            *varlist[i].q_m = q_var;
                        #endif

                        break;

                    case DOUBLE_VAR:
                        size = sizeof (F8);
                        CHECK_POS ();

                        d_var = read_double (obj_buf, pos);
                        pos = pos + size;

                        #if BS_LITTLE_ENDIAN
                            *varlist[i].d_m = conv_double (d_var);
                        #else
                            *varlist[i].d_m = d_var;
                        #endif

                        break;

                    case STRING_VAR:
                        size = read_stringbn (obj_buf, buf, pos, obj_size);
                        pos = pos + size + 1;
                        strcpy (varlist[i].s_m, buf);
                        break;

                    case BYTE_VAR:
                        size = sizeof (U1);
                        read_char (obj_buf, varlist[i].s_m, pos);
                        pos = pos + size;
                        break;
                }
            }

            #if DEBUG
                if (print_debug)
                {
                    printf ("readed var: %li\n", i);
                }
            #endif
        }


        /* read thread private varlist master --------------------------------- */

        #if DEBUG
            if (print_debug)
            {
                printf ("obj_buf pos: %li\n", pos);
                printf ("object reading thread private varlist, maxvarlist: %li\n", vm_mem.obj_maxpvarlist);
            }
        #endif

        for (i = 0; i <= vm_mem.obj_maxpvarlist; i++)
        {
            #if DEBUG
                if (print_debug)
                {
                    printf ("reading thread private variable %li...\n", i);
                }
            #endif

            size = 1;
            CHECK_POS ();

            pvarlist_obj[i].type = obj_buf[pos];

            #if DEBUG
                if (print_debug)
                {
                    printf ("pos: %li\n", pos);
                }
            #endif

            pos = pos + size;

            #if DEBUG
                if (print_debug)
                {
                    printf ("variable type: ");

                    switch (pvarlist_obj[i].type)
                    {
                        case INT_VAR:
                            printf ("INT\n");
                            break;

                        case LINT_VAR:
                            printf ("LONGINT\n");
                            break;

                       case QINT_VAR:
                            printf ("QINT\n");
                            break;

                        case DOUBLE_VAR:
                            printf ("DOUBLE\n");
                            break;

                        case STRING_VAR:
                            printf ("STRING\n");
                            break;

                        case BYTE_VAR:
                            printf ("BYTE\n");
                            break;

                        case DYNAMIC_VAR:
                            printf ("DYNAMIC\n");
                            break;
                            
                        default:
                            printf ("%i\n", pvarlist_obj[i].type);
                            break;
                    }
                }
            #endif


            /* check variable type */

            switch (pvarlist_obj[i].type)
            {
                case INT_VAR:
                case LINT_VAR:
                case QINT_VAR:
                case DOUBLE_VAR:
                case STRING_VAR:
                case BYTE_VAR:
                case DYNAMIC_VAR:
                    break;

                default:
                    /* wrong variable type */

                    VERIFY_ERR ();
                    break;
            }


            size = sizeof (S4);
            CHECK_POS ();

            n = read_lint (obj_buf, pos);
            pos = pos + size;

            #if BS_LITTLE_ENDIAN
                pvarlist_obj[i].dims = conv_lint (n);
            #else
                pvarlist_obj[i].dims = n;
            #endif

            if (pvarlist_obj[i].dims != NODIMS)
            {
                /* read dimensions */

                #if DEBUG
                    if (print_debug)
                    {
                        printf ("variable dims: %li\n", pvarlist_obj[i].dims);
                    }
                #endif

                size = sizeof (S4);

                for (j = 0; j <= pvarlist_obj[i].dims; j++)
                {
                    CHECK_POS ();

                    n = read_lint (obj_buf, pos);
                    pos = pos + size;

                    #if BS_LITTLE_ENDIAN
                        dim_ind = conv_lint (n);
                    #else
                        dim_ind = n;
                    #endif

                    pvarlist_obj[i].dimens[j] = dim_ind;
                }
            }

            /* read type: VAR, CONST_VAR ------------------------------------- */

            size = 1;
            CHECK_POS ();

            ch = obj_buf[pos];
            pos = pos + size;

            pvarlist_obj[i].constant = ch;
            
            if (ch == CONST_VAR)
            {
                #if DEBUG
                    if (print_debug)
                    {
                        printf ("allocating var: %li\n", i);
                    }
                #endif

                if (! exe_alloc_var (pvarlist_obj, i, pvarlist_obj[i].type))
                {
                    printerr (MEMORY, NOTDEF, ST_PRE, "");
                    OPEN_ERR ();
                }

                switch (pvarlist_obj[i].type)
                {
                    case INT_VAR:
                        size = sizeof (S2);
                        CHECK_POS ();

                        i_var = read_int (obj_buf, pos);
                        pos = pos + size;

                        #if BS_LITTLE_ENDIAN
                            *pvarlist_obj[i].i_m = conv_int (i_var);
                        #else
                            *pvarlist_obj[i].i_m = i_var;
                        #endif

                        break;

                    case LINT_VAR:
                        size = sizeof (S4);
                        CHECK_POS ();

                        li_var = read_lint (obj_buf, pos);
                        pos = pos + size;

                        #if BS_LITTLE_ENDIAN
                            *pvarlist_obj[i].li_m = conv_lint (li_var);
                        #else
                            *pvarlist_obj[i].li_m = li_var;
                        #endif

                        break;

                    case QINT_VAR:
                        size = sizeof (S8);
                        CHECK_POS ();

                        q_var = read_qint (obj_buf, pos);
                        pos = pos + size;

                        #if BS_LITTLE_ENDIAN
                            *pvarlist_obj[i].q_m = conv_qint (q_var);
                        #else
                            *pvarlist_obj[i].q_m = q_var;
                        #endif

                        break;

                    case DOUBLE_VAR:
                        size = sizeof (F8);
                        CHECK_POS ();

                        d_var = read_double (obj_buf, pos);
                        pos = pos + size;

                        #if BS_LITTLE_ENDIAN
                            *pvarlist_obj[i].d_m = conv_double (d_var);
                        #else
                            *pvarlist_obj[i].d_m = d_var;
                        #endif

                        break;

                    case STRING_VAR:
                        size = read_stringbn (obj_buf, buf, pos, obj_size);
                        pos = pos + size + 1;
                        strcpy (pvarlist_obj[i].s_m, buf);
                        break;

                    case BYTE_VAR:
                        size = sizeof (U1);
                        read_char (obj_buf, pvarlist_obj[i].s_m, pos);
                        pos = pos + size;
                        break;
                }
            }

            #if DEBUG
                if (print_debug)
                {
                    printf ("readed thread private var: %li\n", i);
                }
            #endif
        }

        /* read jumplist ------------------------------------------------------ */
		
		for (i = 0; i <= maxjumplist; i++)
		{
			/* read islabel */
			size = 1;
            CHECK_POS ();

            ch = obj_buf[pos];
            pos = pos + size;
			
			jumplist[i].islabel = ch;
			
			/* read position */
			size = sizeof (S4);
			CHECK_POS ();

            li_var = read_lint (obj_buf, pos);
            pos = pos + size;

            #if BS_LITTLE_ENDIAN
				jumplist[i].pos = conv_lint (li_var);
            #else
                jumplist[i].pos = li_var;
            #endif
				
			/* read name */
			size = read_stringbn (obj_buf, buf, pos, obj_size);
            pos = pos + size + 1;
            strcpy (jumplist[i].lab, buf);
		}

        /* check if debug information is in object file ----------------------- */

        if (debug)
        {
            /* read debug information -------------------------------------------- */
            /* read opcode line-numbers */

            size = sizeof (S4);

            for (i = 0; i <= maxcclist; i++)
            {
                CHECK_POS ();

                li_var = read_lint (obj_buf, pos);
                pos = pos + size;

                #if BS_LITTLE_ENDIAN
                    cclist[i][BCELIST] = conv_lint (li_var);
                #else
                    cclist[i][BCELIST] = li_var;
                #endif


                CHECK_POS ();

                li_var = read_lint (obj_buf, pos);
                pos = pos + size;

                #if BS_LITTLE_ENDIAN
                    cclist[i][INCLUDELIST] = conv_lint (li_var);
                #else
                    cclist[i][INCLUDELIST] = li_var;
                #endif

            }

            /* get number of includes and filenames */

            CHECK_POS ();

            li_var = read_lint (obj_buf, pos);
            pos = pos + size;

            #if BS_LITTLE_ENDIAN
                includes_size = conv_lint (li_var);
            #else
                includes_size = li_var;
            #endif

            includes = (struct includes *) malloc ((includes_size) * sizeof (struct includes));
            if (includes == NULL)
            {
                printerr (MEMORY, NOTDEF, ST_PRE, "includes");
                exe_shutdown (WARN);
            }

            for (i = 0; i < includes_size; i++)
            {
                size = read_stringbn (obj_buf, buf, pos, obj_size);
                pos = pos + size + 1;

                includes[i].name = malloc (strlen (buf) + 1);
                if (includes[i].name == NULL)
                {
                    printerr (MEMORY, NOTDEF, ST_PRE, "includes");
                    exe_shutdown (WARN);
                }

                strcpy (includes[i].name, buf);
            }

            includes_ind = includes_size - 1;

            /* read variable names */

            for (i = 0; i <= vm_mem.obj_maxvarlist; i++)
            {
                size = read_stringbn (obj_buf, buf, pos, obj_size);
                pos = pos + size + 1;
                strcpy (varlist[i].name, buf);
            }


            /* read thread private variable names */

            for (i = 0; i <= vm_mem.obj_maxpvarlist; i++)
            {
                size = read_stringbn (obj_buf, buf, pos, obj_size);
                pos = pos + size + 1;
                strcpy (pvarlist_obj[i].name, buf);
            }
        }
        else
        {
            for (i = 0; i <= maxcclist; i++)
            {
                cclist[i][BCELIST] = 0;
            }

            for (i = 0; i <= vm_mem.obj_maxvarlist; i++)
            {
                strcpy (varlist[i].name, "");
            }
        }

        finish:

/* DEBUG ------------------------------------------------------------ */

    #if DEBUG
        printf ("openo.c: cclist: %li\n", maxcclist);

        for (i = 0; i <= maxcclist; i++)
        {
            printf ("opcode: %li: %li, r1: %li,  r2: %li,  r3: %li\n", i, cclist[i][0], cclist[i][1], cclist[i][2], cclist[i][3]);
        }
    #endif

/* DEBUG ------------------------------------------------------------ */



            free (obj_buf);
            printmsg (STATUS_OK, "");
            return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}

