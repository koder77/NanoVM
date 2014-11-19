/****************************************************************************
*
* Filename: mem.c
*
* Author:   Stefan Pietzonke
* Project:  flow, the gfx plugin for nano VM
*
* Purpose:  memory allocation functions
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2014
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

extern struct app app[MAXAPPS];


void init_screens (void)
{
    S2 j, i;

	for (j = 0; j < MAXAPPS; j++)
	{
		app[j].in_use = FALSE;  /* in use flag, for free apps pool function */
		
		for (i = 0; i < MAXSCREEN; i++)
		{
			app[j].screen[i].window = NULL;
			app[j].screen[i].status = CLOSED;
			strcpy (app[j].screen[i].title, "");
			strcpy (app[j].screen[i].icon, "");
			strcpy (app[j].screen[i].text, "");
			strcpy (app[j].screen[i].fontname, "");

			app[j].screen[i].vx = NULL;
			app[j].screen[i].vy = NULL;
			app[j].screen[i].font_bitmap.buf = NULL;

			app[j].screen[i].font_ttf.font = NULL;
			app[j].screen[i].font_ttf.style_normal = TRUE;
			app[j].screen[i].font_ttf.style_bold = FALSE;
			app[j].screen[i].font_ttf.style_italic = FALSE;
			app[j].screen[i].font_ttf.style_underline = FALSE;
	
			app[j].screen[i].gadget = NULL;
			app[j].screen[i].gadgets = 0;
			app[j].screen[i].gadget_index = 0;

			app[j].screen[i].gadget_cycle_text = NULL;
			app[j].screen[i].gadget_cycle_menu_entries = 0;
			app[j].screen[i].gadget_cycle_text_index = 0;
		}
    }
}

void close_app_screens (S2 appnum)
{
	S2 i;
	
	for (i = 0; i < MAXSCREEN; i++)
	{
		if (app[appnum].screen[i].window != NULL)
		{
			SDL_DestroyWindow (app[appnum].screen[i].window);
		}
	}
}

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

void free_memory (S2 appnum)
{
    S2 i;

    for (i = 0; i < MAXSCREEN - 1; i++)
    {
        if (app[appnum].screen[i].vx)
        {
            free (app[appnum].screen[i].vx);
        }

        if (app[appnum].screen[i].vy)
        {
            free (app[appnum].screen[i].vy);
        }

        if (app[appnum].screen[i].font_bitmap.buf)
        {
            free (app[appnum].screen[i].font_bitmap.buf);
        }

        if (app[appnum].screen[i].font_ttf.font)
        {
            TTF_CloseFont (app[appnum].screen[i].font_ttf.font);
        }

        if (app[appnum].screen[i].gadget_cycle_text)
        {
            dealloc_array_U1 (app[appnum].screen[i].gadget_cycle_text, app[appnum].screen[i].gadget_cycle_menu_entries);
        }
	}

    free_gadgets (appnum);
}
