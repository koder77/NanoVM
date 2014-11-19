/****************************************************************************
*
* Filename: optimizer.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  N compiler: compiler code optimizer
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

extern S4 **cclist;
extern S2 cclist_ind;
extern struct jumplist *jumplist;

U1 optimize (void)
{
    S4 ind, nind = 0, jumpind, i, j, k, reg, opcode, var;

    S4 **newcclist = NULL;

    newcclist = alloc_array_lint (cclist_ind + 1, MAXCCOMMARG);
    if (newcclist == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "opcodes optimizing list");
        return (FALSE);
    }

    for (i = 0; i <= cclist_ind; i++)
    {
        if (cclist[i][0] == JMP_L)
        {
            jumpind = cclist[i][2];
            if (jumplist[jumpind].lab[0] == 'f' && jumplist[jumpind].lab[1] == 'o' && jumplist[jumpind].lab[2] == 'r')
            {
                /* found for jump => do optimize */

                if ((i - 8) >= 0)
                {
                    if ((cclist[i - 8][0] == PUSH_I || cclist[i - 8][0] == PUSH_L) && (cclist[i - 7][0] == INC_L || cclist[i - 7][0] == DEC_L) &&  (cclist[i - 6][0] == PULL_I || cclist[i - 6][0] == PULL_L))
                    {
                        if ((cclist[i - 8][2] == cclist[i - 7][1]) && (cclist[i - 6][1] == cclist[i - 8][2]))
                        {
                            /* do PUSH_L and PULL_L optimization */

                            printf ("optimize: for loop and inc_l or dec_l.\n");

                            nind = nind - 8;

                            reg = cclist[i - 3][1];
                            opcode = cclist[i - 8][0];
                            var = cclist[i - 8][1];

                            newcclist[nind][2] = reg;
                            nind++;

                            newcclist[nind][1] = reg;
                            nind++;

                            if (opcode == PUSH_I)
                            {
                                newcclist[nind][0] = PULL_I;
                                newcclist[nind][1] = reg;
                                newcclist[nind][2] = var;
                                newcclist[nind][3] = 0;
                                newcclist[nind][4] = 0;
                                newcclist[nind][5] = 0;
                                newcclist[nind][6] = 0;

                                nind++;
                            }
                            else
                            {
                                newcclist[nind][0] = PULL_L;
                                newcclist[nind][1] = reg;
                                newcclist[nind][2] = var;
                                newcclist[nind][3] = 0;
                                newcclist[nind][4] = 0;
                                newcclist[nind][5] = 0;
                                newcclist[nind][6] = 0;

                                nind++;
                            }

                            for (j = 0; j <= MAXCCOMMARG; j++)
                            {
                                newcclist[nind][j] = cclist[i - 4][j];
                            }
                            nind++;

                            switch (cclist[i - 3][0])
                            {
                                case GREATER_L:
                                    newcclist[nind][0] = GREATER_JMP_L;
                                    break;

                                case LESS_L:
                                    newcclist[nind][0] = LESS_JMP_L;
                                    break;

                                case GREATER_OR_EQ_L:
                                    newcclist[nind][0] = GREATER_OR_EQ_JMP_L;
                                    break;

                                case LESS_OR_EQ_L:
                                    newcclist[nind][0] = LESS_OR_EQ_JMP_L;
                                    break;
                            }

                            newcclist[nind][1] = cclist[i - 3][1];
                            newcclist[nind][2] = cclist[i - 3][2];
                            newcclist[nind][3] = jumpind;
                            newcclist[nind][4] = cclist[i][4];
                            newcclist[nind][5] = cclist[i][5];
                            newcclist[nind][6] = cclist[i][6];
                            nind++;
                        }
                    }
                    else
                    {
                        printf ("optimize: for loop.\n");

                        nind = nind - 3;

                        switch (cclist[i - 3][0])
                        {
                            case GREATER_L:
                                newcclist[nind][0] = GREATER_JMP_L;
                                break;

                            case LESS_L:
                                newcclist[nind][0] = LESS_JMP_L;
                                break;

                            case GREATER_OR_EQ_L:
                                newcclist[nind][0] = GREATER_OR_EQ_JMP_L;
                                break;

                            case LESS_OR_EQ_L:
                                newcclist[nind][0] = LESS_OR_EQ_JMP_L;
                                break;
                        }

                        newcclist[nind][1] = cclist[i - 3][1];
                        newcclist[nind][2] = cclist[i - 3][2];
                        newcclist[nind][3] = jumpind;
                        newcclist[nind][4] = cclist[i][4];
                        newcclist[nind][5] = cclist[i][5];
                        newcclist[nind][6] = cclist[i][6];
                        nind++;
                    }
                }
            }
            else
            {
                for (j = 0; j < MAXCCOMMARG; j++)
                {
                    newcclist[nind][j] = cclist[i][j];
                }
                nind++;
             }
         }
         else
         {
            for (j = 0; j < MAXCCOMMARG; j++)
            {
                newcclist[nind][j] = cclist[i][j];
            }
            nind++;
        }
    }

    /* copy newcclist to cclist */

    for (i = 0; i < nind; i++)
    {
        for (j = 0; j < MAXCCOMMARG; j++)
        {
            cclist[i][j] = newcclist[i][j];
        }
    }
    cclist_ind = nind - 1;

    free (newcclist);
    
    /* set jumps */
    
    for (i = 0; i <= cclist_ind; i++)
    {
        if (cclist[i][0] == VM_LAB)
        {
            jumplist[cclist[i][1]].pos = i;
        }
    }

    return (TRUE);
}

U1 optimize_2 (void)
{
    /* optimize for loops even better */
    
    U1 optimized;
    S4 ind, nind = 0, i, j, opcode;

    S4 **newcclist = NULL;

optimize:
    newcclist = alloc_array_lint (cclist_ind + 1, MAXCCOMMARG);
    if (newcclist == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "opcodes optimizing list");
        return (FALSE);
    }

    printf ("optimizer: START\n");
    optimized = FALSE; nind = 0;
    for (i = 0; i <= cclist_ind; i++)
    {
        if ((cclist[i][0] == GREATER_JMP_L || cclist[i][0] == LESS_JMP_L || cclist[i][0] == GREATER_OR_EQ_JMP_L || cclist[i][0] == LESS_OR_EQ_JMP_L) && optimized == FALSE)
        {
            /* found for loop footer -> do optimization */
            
            if (cclist[i - 2][0] == INC_L || cclist[i - 2][0] == DEC_L)
            {
                /* found INC_L OR DEC_L */
                
                printf ("optimize: for loop: -O2.\n");
                
                optimized = TRUE;
                
                nind = i - 2;
				
				opcode = cclist[i - 2][0];
				
				if (opcode == INC_L)
                {
                    if (cclist[i][0] == LESS_JMP_L)
                    {
                        opcode = INC_LESS_JMP_L;
                    }
                        
                    if (cclist[i][0] == LESS_OR_EQ_JMP_L)
                    {
                        opcode = INC_LESS_OR_EQ_JMP_L;
                    }
                }
                    
                if (opcode == DEC_L)
                {
                    if (cclist[i][0] == GREATER_JMP_L)
                    {
                        opcode = DEC_GREATER_JMP_L;
                    }
                        
                    if (cclist[i][0] == GREATER_OR_EQ_JMP_L)
                    {
                        opcode = DEC_GREATER_OR_EQ_JMP_L;
                    }
                }
            
                newcclist[nind][0] = opcode;
                newcclist[nind][1] = cclist[i][1];
                newcclist[nind][2] = cclist[i][2];
                newcclist[nind][3] = cclist[i][3];
                newcclist[nind][4] = cclist[i][4];
                newcclist[nind][5] = cclist[i][5];
                newcclist[nind][6] = cclist[i][6];
            
                nind++;
				
				for (j = i + 1; j <= cclist_ind; j++)
                {
                    newcclist[nind][0] = cclist[j][0];
                    newcclist[nind][1] = cclist[j][1];
                    newcclist[nind][2] = cclist[j][2];
                    newcclist[nind][3] = cclist[j][3];
                    newcclist[nind][4] = cclist[j][4];
                    newcclist[nind][5] = cclist[j][5];
                    newcclist[nind][6] = cclist[j][6];
                    
                    nind++;
                }
				
                goto copy;
            }
            else
            {
                for (j = 0; j < MAXCCOMMARG; j++)
                {
                    newcclist[nind][j] = cclist[i][j];
                }
                nind++;
            }
        }
        else
        {
            for (j = 0; j < MAXCCOMMARG; j++)
            {
                newcclist[nind][j] = cclist[i][j];
            }
            nind++;
        }
    }
 
 copy:
 /* copy newcclist to cclist */

    for (i = 0; i < nind; i++)
    {
        for (j = 0; j < MAXCCOMMARG; j++)
        {
            cclist[i][j] = newcclist[i][j];
        }
    }
    cclist_ind = nind - 1;
    
    free (newcclist);
    
    /* set jumps */
    
    for (i = 0; i <= cclist_ind; i++)
    {
        if (cclist[i][0] == VM_LAB)
        {
            jumplist[cclist[i][1]].pos = i;
        }
    }
    
    if (optimized == TRUE)
    {
        goto optimize;
    }

    return (TRUE);
}

U1 optimize_remove_double_pull (void)
{
    /* optimize for loops even better */
    
    S4 ind, nind = 0, i, j;

    S4 **newcclist = NULL;

    newcclist = alloc_array_lint (cclist_ind + 1, MAXCCOMMARG);
    if (newcclist == NULL)
    {
        printerr (MEMORY, NOTDEF, ST_PRE, "opcodes optimizing list");
        return (FALSE);
    }

    printf ("optimizer: remove double pull START\n");
    
	for (i = 0; i <= cclist_ind; i++)
    {
        if (cclist[i][0] == PULL_I || cclist[i][0] == PULL_L || cclist[i][0] == PULL_Q || cclist[i][0] == PULL_D  || cclist[i][0] == PULL_S || cclist[i][0] == PULL_B || cclist[i][0] == PPULL_I || cclist[i][0] == PPULL_L || cclist[i][0] == PPULL_Q || cclist[i][0] == PPULL_D  || cclist[i][0] == PPULL_S || cclist[i][0] == PPULL_B)
        {
            /* found PULL opcode: check for double PULL of same variable */
			
			if (cclist[i][0] == cclist[i + 1][0] && cclist[i][1] == cclist[i + 1][1])
			{
				printf ("optimizer: removing double PULL\n");
			}
			else
			{
				newcclist[nind][0] = cclist[i][0];
                newcclist[nind][1] = cclist[i][1];
                newcclist[nind][2] = cclist[i][2];
                newcclist[nind][3] = cclist[i][3];
                newcclist[nind][4] = cclist[i][4];
                newcclist[nind][5] = cclist[i][5];
                newcclist[nind][6] = cclist[i][6];
				nind++;
			}
		}
		else
		{
			newcclist[nind][0] = cclist[i][0];
            newcclist[nind][1] = cclist[i][1];
            newcclist[nind][2] = cclist[i][2];
            newcclist[nind][3] = cclist[i][3];
            newcclist[nind][4] = cclist[i][4];
            newcclist[nind][5] = cclist[i][5];
            newcclist[nind][6] = cclist[i][6];
			nind++;
		}
	}
	
	/* copy newcclist to cclist */

    for (i = 0; i < nind; i++)
    {
        for (j = 0; j < MAXCCOMMARG; j++)
        {
            cclist[i][j] = newcclist[i][j];
        }
    }
    cclist_ind = nind - 1;
    
    free (newcclist);
    
    /* set jumps */
    
    for (i = 0; i <= cclist_ind; i++)
    {
        if (cclist[i][0] == VM_LAB)
        {
            jumplist[cclist[i][1]].pos = i;
        }
    }
		
	return (TRUE);
}
