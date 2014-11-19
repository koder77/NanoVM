/****************************************************************************
*
* Filename: mem.c
*
* Author:   Stefan Pietzonke
* Project:  flow, the gfx plugin for nano VM
*
* Purpose:  memory allocation functions
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

#include "global_d.h"

U1 **alloc_array_U1 (S4 x, S4 y)
{
    S4 i, alloc;
    U1 **array = NULL;
    U1 err = FALSE;

    array = (U1 **) malloc (x * sizeof (*array));
    if (array != NULL)
    {
        for (i = 0; i < x; i++)
        {
            array[i] = (U1 *) malloc (y * sizeof (*array));
            if (array[i] == NULL)
            {
                alloc = i - 1;
                err = TRUE;
                break;
            }
        }
        if (err)
        {
            if (alloc >= 0)
            {
                for (i = 0; i <= alloc; i++)
                {
                    free (array[i]);
                }
            }
            free (array);
            array = NULL;
        }
    }
    return (array);
}

void dealloc_array_U1 (U1 **array, S4 x)
{
    S4 i;

    for (i = 0; i < x; i++)
    {
        if (array[i])
        {
            free (array[i]);
        }
    }
    free (array);
}
