/****************************************************************************
*
* Filename: saveo.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  save object code
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

extern struct varlist *pvarlist_obj;                /* thread private varlist */
extern struct varlist_state pvarlist_state;


extern S4 **cclist;
extern S4 maxcclist;

extern U1 *obj_buf;

extern U2 includes_size;
extern S2 includes_ind;
extern struct includes *includes;

extern struct jumplist *jumplist;
extern S4 maxjumplist;


#define SAVE_ERR();     printerr (MEMORY, NOTDEF, ST_EXE, "increase object buffer: use '-objs' option!"); printerr (SAVE, NOTDEF, ST_EXE, file); fclose (obj); if (obj_buf) { free (obj_buf); } return (FALSE);
#define CHECK_POS();    if ((pos + size) > obj_size) { SAVE_ERR (); }

#if DEBUG
    extern U1 print_debug;
#endif

U1 exe_save_object (U1 *file, S4 obj_size, U1 save_debug)
{
    FILE *obj = NULL;
    S4 i, j, dim_ind, pos = 0, write_size;
    S2 size;
    U1 ch;

    obj_buf = NULL;
    /* obj_buf = (U1 *) malloc (obj_size * sizeof (U1)); */

    obj_buf = (U1 *) calloc (obj_size, sizeof (U1));
    if (obj_buf == NULL)
    {
        /* no mem */

        printerr (MEMORY, NOTDEF, ST_EXE, "object buffer");

        SAVE_ERR ();
    }

    if ((obj = fopen (file, "wb")) != NULL)
    {
        printmsg (SAVE_PROG, file);

        /* write header ------------------------------------------------------ */

        size = OBJECTHD_LEN;
        CHECK_POS ();

        if (save_debug)
        {
            write_string (obj_buf, DEBUG_OBJECTHD_SB, pos);
        }
        else
        {
            write_string (obj_buf, NORM_OBJECTHD_SB, pos);
        }
        pos = pos + OBJECTHD_LEN;

        /* write version number ---------------------------------------------- */

        size = sizeof (S4);
        CHECK_POS ();

        #if BS_LITTLE_ENDIAN
            write_lint (obj_buf, conv_lint (VERSION_OBJECT), pos);
        #else
            write_lint (obj_buf, VERSION_OBJECT, pos);
        #endif

        pos = pos + size;

        /* write opcode number ----------------------------------------------- */

        CHECK_POS ();

        #if BS_LITTLE_ENDIAN
            write_lint (obj_buf, conv_lint (maxcclist), pos);
        #else
            write_lint (obj_buf, maxcclist, pos);
        #endif

        pos = pos + size;

        /* write variable number --------------------------------------------- */

        CHECK_POS ();

        #if BS_LITTLE_ENDIAN
            write_lint (obj_buf, conv_lint (varlist_state.maxvarlist), pos);
        #else
            write_lint (obj_buf, varlist_state.maxvarlist, pos);
        #endif

        pos = pos + size;

        /* write private variable number --------------------------------------------- */

        CHECK_POS ();

        #if BS_LITTLE_ENDIAN
            write_lint (obj_buf, conv_lint (pvarlist_state.maxvarlist), pos);
        #else
            write_lint (obj_buf, pvarlist_state.maxvarlist, pos);
        #endif

        pos = pos + size;

		/* write maxjumplist ------------------------------------------------- */
		
		CHECK_POS ();
		
		 #if BS_LITTLE_ENDIAN
            write_lint (obj_buf, conv_lint (maxjumplist), pos);
        #else
            write_lint (obj_buf, maxjumplist, pos);
        #endif
			
		pos = pos + size;
		
        /* write opcodes ----------------------------------------------------- */

        for (i = 0; i <= maxcclist; i++)
        {
            /* save WORDCODE (OPCODE = 2 bytes) */

            size = sizeof (S2);
            CHECK_POS ();

            #if BS_LITTLE_ENDIAN
                write_int (obj_buf, conv_int (cclist[i][0]), pos);
            #else
                write_int (obj_buf, cclist[i][0], pos);
            #endif

            pos = pos + size;

            if (opcode[cclist[i][0]].args > 0)
            {
                for (j = 1; j <= opcode[cclist[i][0]].args; j++)
                {
                    if (opcode[cclist[i][0]].type[j - 1] == L_REG || opcode[cclist[i][0]].type[j - 1] == D_REG)
                    {
                        ch = cclist[i][j];

                        size = sizeof (U1);
                        CHECK_POS ();

                        write_char (obj_buf, ch, pos);
                        pos = pos + size;
                    }
                    else
                    {
                        size = sizeof (S4);
                        CHECK_POS ();

                        #if BS_LITTLE_ENDIAN
                            write_lint (obj_buf, conv_lint (cclist[i][j]), pos);
                        #else
                            write_lint (obj_buf, cclist[i][j], pos);
                        #endif

                        pos = pos + size;
                    }
                }
            }
        }

        /* write varlist ----------------------------------------------------- */

        for (i = PROG_VARLIST_START; i <= varlist_state.maxvarlist; i++)
        {
            size = sizeof (U1);
            CHECK_POS ();

            write_char (obj_buf, varlist[i].type, pos);
            pos = pos + size;

            #if DEBUG
                if (print_debug)
                {
                    switch (varlist[i].type)
                    {
                        case INT_VAR:
                            printf ("INT_VAR\n");
                            break;

                        case LINT_VAR:
                            printf ("LONGINT_VAR\n");
                            break;

                        case QINT_VAR:
                            printf ("QINT_VAR (64 bit)\n");
                            break;

                        case DOUBLE_VAR:
                            printf ("DOUBLE_VAR\n");
                            break;

                        case STRING_VAR:
                            printf ("STRING_VAR\n");
                            break;

                        case BYTE_VAR:
                            printf ("BYTE_VAR\n");
                            break;
                           
                        case DYNAMIC_VAR:
                            printf ("DYNAMIC_VAR\n");
                            break;
                    }
                }
            #endif

            size = sizeof (S4);
            CHECK_POS ();

            #if BS_LITTLE_ENDIAN
                write_lint (obj_buf, conv_lint (varlist[i].dims), pos);
            #else
                write_lint (obj_buf, varlist[i].dims, pos);
            #endif

            pos = pos + size;

            if (varlist[i].dims != NODIMS)
            {
                /* save dimensions */

                for (j = 0; j <= varlist[i].dims; j++)
                {
                    dim_ind = varlist[i].dimens[j];

                    CHECK_POS ();

                    #if BS_LITTLE_ENDIAN
                        write_lint (obj_buf, conv_lint (dim_ind), pos);
                    #else
                        write_lint (obj_buf, dim_ind, pos);
                    #endif

                    pos = pos + size;
                }
            }

            /* write type: VAR, CONST_VAR ------------------------------------ */

            if (strcmp (varlist[i].name, "") == 0 && varlist[i].type != UNDEF_VAR)
            {
                ch = CONST_VAR;

                size = sizeof (U1);
                CHECK_POS ();

                write_char (obj_buf, ch, pos);
                pos = pos + size;

                switch (varlist[i].type)
                {
                    case INT_VAR:
                        size = sizeof (S2);
                        CHECK_POS ();

                        #if BS_LITTLE_ENDIAN
                            write_int (obj_buf, conv_int (*varlist[i].i_m), pos);
                        #else
                            write_int (obj_buf, *varlist[i].i_m, pos);
                        #endif

                        pos = pos + size;
                        break;

                    case LINT_VAR:
                        size = sizeof (S4);
                        CHECK_POS ();

                        #if BS_LITTLE_ENDIAN
                            write_lint (obj_buf, conv_lint (*varlist[i].li_m), pos);
                        #else
                            write_lint (obj_buf, *varlist[i].li_m, pos);
                        #endif

                        pos = pos + size;
                        break;

                    case QINT_VAR:
                        size = sizeof (S8);
                        CHECK_POS ();

                        #if BS_LITTLE_ENDIAN
                            write_qint (obj_buf, conv_qint (*varlist[i].q_m), pos);
                        #else
                            write_qint (obj_buf, *varlist[i].q_m, pos);
                        #endif

                        pos = pos + size;
                        break;

                    case DOUBLE_VAR:
                        size = sizeof (F8);
                        CHECK_POS ();

                        #if BS_LITTLE_ENDIAN
                            write_double (obj_buf, conv_double (*varlist[i].d_m), pos);
                        #else
                            write_double (obj_buf, *varlist[i].d_m, pos);
                        #endif

                        pos = pos + size;
                        break;

                    case STRING_VAR:
                        size = strlen (varlist[i].s_m);
                        CHECK_POS ();

                        write_string (obj_buf, varlist[i].s_m, pos);
                        pos = pos + size;

                        size = sizeof (U1);
                        CHECK_POS ();

                        ch = BINUL;

                        write_char (obj_buf, ch, pos);
                        pos = pos + size;
                        break;

                    case BYTE_VAR:
                        size = sizeof (U1);
                        CHECK_POS();

                        write_char (obj_buf, varlist[i].s_m[0], pos);
                        pos = pos + size;
                        break;
                }
            }
            else
            {
                size = sizeof (U1);
                CHECK_POS ();

                ch = VAR;

                write_char (obj_buf, ch, pos);
                pos = pos + size;
            }
        }



        /* write private varlist ----------------------------------------------------- */

        for (i = 0; i <= pvarlist_state.maxvarlist; i++)
        {
            size = sizeof (U1);
            CHECK_POS ();

            write_char (obj_buf, pvarlist_obj[i].type, pos);
            pos = pos + size;

            #if DEBUG
                if (print_debug)
                {
                    switch (pvarlist_obj[i].type)
                    {
                        case INT_VAR:
                            printf ("INT_VAR\n");
                            break;

                        case LINT_VAR:
                            printf ("LONGINT_VAR\n");
                            break;

                        case QINT_VAR:
                            printf ("QINT_VAR (64 bit)\n");
                            break;

                        case DOUBLE_VAR:
                            printf ("DOUBLE_VAR\n");
                            break;

                        case STRING_VAR:
                            printf ("STRING_VAR\n");
                            break;

                        case BYTE_VAR:
                            printf ("BYTE_VAR\n");
                            break;
                            
                        case DYNAMIC_VAR:
                            printf ("DYNAMIC_VAR\n");
                            break;
                    }
                }
            #endif

            size = sizeof (S4);
            CHECK_POS ();

            #if BS_LITTLE_ENDIAN
                write_lint (obj_buf, conv_lint (pvarlist_obj[i].dims), pos);
            #else
                write_lint (obj_buf, pvarlist_obj[i].dims, pos);
            #endif

            pos = pos + size;

            if (pvarlist_obj[i].dims != NODIMS)
            {
                /* save dimensions */

                for (j = 0; j <= pvarlist_obj[i].dims; j++)
                {
                    dim_ind = pvarlist_obj[i].dimens[j];

                    CHECK_POS ();

                    #if BS_LITTLE_ENDIAN
                        write_lint (obj_buf, conv_lint (dim_ind), pos);
                    #else
                        write_lint (obj_buf, dim_ind, pos);
                    #endif

                    pos = pos + size;
                }
            }

            /* write type: VAR, CONST_VAR ------------------------------------ */

            if (strcmp (pvarlist_obj[i].name, "") == 0 && varlist[i].type != UNDEF_VAR)
            {
                ch = CONST_VAR;

                size = sizeof (U1);
                CHECK_POS ();

                write_char (obj_buf, ch, pos);
                pos = pos + size;

                switch (pvarlist_obj[i].type)
                {
                    case INT_VAR:
                        size = sizeof (S2);
                        CHECK_POS ();

                        #if BS_LITTLE_ENDIAN
                            write_int (obj_buf, conv_int (*pvarlist_obj[i].i_m), pos);
                        #else
                            write_int (obj_buf, *pvarlist_obj[i].i_m, pos);
                        #endif

                        pos = pos + size;
                        break;

                    case LINT_VAR:
                        size = sizeof (S4);
                        CHECK_POS ();

                        #if BS_LITTLE_ENDIAN
                            write_lint (obj_buf, conv_lint (*pvarlist_obj[i].li_m), pos);
                        #else
                            write_lint (obj_buf, *pvarlist_obj[i].li_m, pos);
                        #endif

                        pos = pos + size;
                        break;

                    case QINT_VAR:
                        size = sizeof (S8);
                        CHECK_POS ();

                        #if BS_LITTLE_ENDIAN
                            write_qint (obj_buf, conv_qint (*pvarlist_obj[i].q_m), pos);
                        #else
                            write_qint (obj_buf, *pvarlist_obj[i].q_m, pos);
                        #endif

                        pos = pos + size;
                        break;

                    case DOUBLE_VAR:
                        size = sizeof (F8);
                        CHECK_POS ();

                        #if BS_LITTLE_ENDIAN
                            write_double (obj_buf, conv_double (*pvarlist_obj[i].d_m), pos);
                        #else
                            write_double (obj_buf, *pvarlist_obj[i].d_m, pos);
                        #endif

                        pos = pos + size;
                        break;

                    case STRING_VAR:
                        size = strlen (pvarlist_obj[i].s_m);
                        CHECK_POS ();

                        write_string (obj_buf, pvarlist_obj[i].s_m, pos);
                        pos = pos + size;

                        size = sizeof (U1);
                        CHECK_POS ();

                        ch = BINUL;

                        write_char (obj_buf, ch, pos);
                        pos = pos + size;
                        break;

                    case BYTE_VAR:
                        size = sizeof (U1);
                        CHECK_POS();

                        write_char (obj_buf, pvarlist_obj[i].s_m[0], pos);
                        pos = pos + size;
                        break;
                }
            }
            else
            {
                size = sizeof (U1);
                CHECK_POS ();

                ch = VAR;

                write_char (obj_buf, ch, pos);
                pos = pos + size;
            }
        }

        /* save jumplist ----------------------------------------------------- */
		
		for (i = 0; i <= maxjumplist; i++)
		{
			/* save islabel */
			size = sizeof (U1);
            CHECK_POS ();

            write_char (obj_buf, jumplist[i].islabel, pos);

            pos = pos + size;
			
			/* save position */
			size = sizeof (S4);
			CHECK_POS ();
			
			#if DEBUG
				printf ("saveo: jumplist pos: %li\n", jumplist[i].pos);
			#endif
			
            #if BS_LITTLE_ENDIAN
                write_lint (obj_buf, conv_lint (jumplist[i].pos), pos);
            #else
                write_lint (obj_buf, jumplist[i].pos, pos);
            #endif

            pos = pos + size;
			
			/* save label name */
			size = strlen (jumplist[i].lab);
            CHECK_POS ();

            write_string (obj_buf, jumplist[i].lab, pos);
            pos = pos + size;

            size = sizeof (U1);
            CHECK_POS ();

            ch = BINUL;

            write_char (obj_buf, ch, pos);
            pos = pos + size;
		}
        
        /* write debug information ------------------------------------------- */

        if (save_debug)
        {
            /* write opcode line-numbers */

            size = sizeof (S4);

            for (i = 0; i <= maxcclist; i++)
            {
                CHECK_POS ();

                #if BS_LITTLE_ENDIAN
                    write_lint (obj_buf, conv_lint (cclist[i][BCELIST]), pos);
                #else
                    write_lint (obj_buf, cclist[i][BCELIST], pos);
                #endif

                pos = pos + size;


                CHECK_POS ();

                #if BS_LITTLE_ENDIAN
                    write_lint (obj_buf, conv_lint (cclist[i][INCLUDELIST]), pos);
                #else
                    write_lint (obj_buf, cclist[i][INCLUDELIST], pos);
                #endif

                pos = pos + size;
            }

            /* write number of includes and filenames */

            CHECK_POS ();

            #if BS_LITTLE_ENDIAN
                write_lint (obj_buf, conv_lint (includes_ind + 1), pos);
            #else
                write_lint (obj_buf, includes_ind + 1, pos);
            #endif

            pos = pos + size;


            for (i = 0; i <= includes_ind; i++)
            {
                size = strlen (includes[i].name);
                CHECK_POS ();

                write_string (obj_buf, includes[i].name, pos);
                pos = pos + size;

                size = sizeof (U1);
                CHECK_POS ();

                ch = BINUL;

                write_char (obj_buf, ch, pos);
                pos = pos + size;
            }


            /* write variable names */

            for (i = 0; i <= varlist_state.maxvarlist; i++)
            {
                size = strlen (varlist[i].name);
                CHECK_POS ();

                write_string (obj_buf, varlist[i].name, pos);
                pos = pos + size;

                size = sizeof (U1);
                CHECK_POS ();

                ch = BINUL;

                write_char (obj_buf, ch, pos);
                pos = pos + size;
            }


            /* write thread private variable names */

            for (i = 0; i <= pvarlist_state.maxvarlist; i++)
            {
                size = strlen (pvarlist_obj[i].name);
                CHECK_POS ();

                write_string (obj_buf, pvarlist_obj[i].name, pos);
                pos = pos + size;

                size = sizeof (U1);
                CHECK_POS ();

                ch = BINUL;

                write_char (obj_buf, ch, pos);
                pos = pos + size;
            }
        }

        write_size = fwrite (obj_buf, sizeof (U1), pos + 1, obj);
        if (write_size != pos + 1)
        {
            SAVE_ERR ();
        }

        free (obj_buf);
        fclose (obj);
        printmsg (STATUS_OK, "");
        return (TRUE);
    }
    else
    {
        printerr (OPEN, NOTDEF, ST_EXE, file);
        return (FALSE);
    }
}
