/****************************************************************************
*
* Filename: exe_pointer.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  pointer, indirect array access functions
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2009
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
#include "arch.h"
#include "global_d.h"


/* set variable address at label line
 * for indirect addressing => like a C pointer
 * only used by array variables and array opcodes
 */

U1 exe_pointer (S4 **cclist, S4 reg, S4 var, S4 label)
{
    S2 opcode, offset = 0, maxoffset = 4;
    
try:    
    opcode = cclist[label + offset][0];

    /* check opcode */

/*    printf ("exe_pointer: label: %li\n", label); */
    
    switch (opcode)
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

            cclist[label + offset][1] = reg;
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

            cclist[label + offset][2] = reg;
            break;

        case SIZEOF:

            /* setaddress */

            cclist[label + offset][1] = reg;
            break;

        default:
            /* wrong opcode */
            
            if (offset == maxoffset)
            {
                printf ("error: pointer wrong opcode: %li\n", opcode);

                return (FALSE);
            }
            
 /*           printf ("exe_pointer: offset++\n"); */
            
            offset++;
            goto try;
    }

    return (TRUE);
}
