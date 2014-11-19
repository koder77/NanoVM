/****************************************************************************
*
* Filename: gadgets.c
*
* Author:   Stefan Pietzonke
* Project:  flow, the gfx plugin for nano VM
*
* Purpose:  gadget functions
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

struct app app[MAXAPPS];

extern S2 mouse_x, mouse_y;
extern SDL_Surface *cursor;

extern Uint32 active_window_id;
extern U1 active_window_id_set;


void free_gadgets (S2 appnum);

void init_gadgets (S2 appnum, S2 screennum, S2 gadgets)
{
    /* allocate space for gadgets */

    S2 i;

    if (app[appnum].screen[screennum].gadget)
    {
        free_gadgets (appnum);
    }

    app[appnum].screen[screennum].gadget = (struct gadget *) malloc (gadgets * sizeof (struct gadget));
    if (app[appnum].screen[screennum].gadget == NULL)
    {
        printf ("error: can't allocate gadget list!\n");
    }

    app[appnum].screen[screennum].gadgets = gadgets;

    for (i = 0; i < gadgets; i++)
    {
        app[appnum].screen[screennum].gadget[i].gptr = NULL;
        app[appnum].screen[screennum].gadget[i].type = 0;
    }
}

void free_gadget (S2 appnum, S2 screennum, S2 gadget_index)
{
    struct gadget_button *button;
    struct gadget_checkbox *checkbox;
    struct gadget_cycle *cycle;
    struct gadget_string *string;

    if (app[appnum].screen[screennum].gadget[gadget_index].gptr)
    {
        switch (app[appnum].screen[screennum].gadget[gadget_index].type)
        {
            case GADGET_BUTTON:
                button = (struct gadget_button *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

                if (button->text)
                {
                    free (button->text);
                    button->text = NULL;
                    printf ("free_gadget: %i button->text freed.\n", gadget_index);
                }
                break;

            case GADGET_CHECKBOX:
                checkbox = (struct gadget_checkbox *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

                if (checkbox->text)
                {
                    free (checkbox->text);
                    checkbox->text = NULL;
                    printf ("free_gadget: %i checkbox->text freed.\n", gadget_index);
                }
                break;

            case GADGET_CYCLE:
                cycle = (struct gadget_cycle *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

                if (cycle->text)
                {
                    dealloc_array_U1 (cycle->text, cycle->menu_entries);
                    cycle->text = NULL;
                    printf ("free_gadget: %i cycle->text freed.\n", gadget_index);
                }
                break;

            case GADGET_STRING:
                string = (struct gadget_string *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

                if (string->text)
                {
                    free (string->text);
                    string->text = NULL;
                    printf ("free_gadget: %i string->text freed.\n", gadget_index);
                }

                if (string->value)
                {
                    free (string->value);
                    string->value = NULL;
                    printf ("free_gadget: %i string->value freed.\n", gadget_index);
                }

                if (string->display)
                {
                    free (string->display);
                    string->display = NULL;
                    printf ("free_gadget: %i string->display freed.\n", gadget_index);
                }
                break;
        }

        free (app[appnum].screen[screennum].gadget[gadget_index].gptr);
        app[appnum].screen[screennum].gadget[gadget_index].gptr = NULL;
        app[appnum].screen[screennum].gadget[gadget_index].type = 0;
        printf ("free_gadget: %i gadget freed.\n", gadget_index);
   }
}

void free_gadgets (S2 appnum)
{
    S2 i, j;

    for (i = 0; i < MAXSCREEN; i++)
    {
        if (app[appnum].screen[i].gadget)
        {
            for (j = 0; j < app[appnum].screen[i].gadgets; j++)
            {
                free_gadget (appnum, i, j);
            }
        }
    }
}


/* border colors */

void set_gadget_color_border_light (S2 appnum, S2 screennum)
{
    app[appnum].screen[screennum].gadget_color.border_light.r = app[appnum].screen[screennum].r;
    app[appnum].screen[screennum].gadget_color.border_light.g = app[appnum].screen[screennum].g;
    app[appnum].screen[screennum].gadget_color.border_light.b = app[appnum].screen[screennum].b;
}

void set_gadget_color_border_shadow (S2 appnum, S2 screennum)
{
    app[appnum].screen[screennum].gadget_color.border_shadow.r = app[appnum].screen[screennum].r;
    app[appnum].screen[screennum].gadget_color.border_shadow.g = app[appnum].screen[screennum].g;
    app[appnum].screen[screennum].gadget_color.border_shadow.b = app[appnum].screen[screennum].b;
}

/* background colors */

void set_gadget_color_backgr_light (S2 appnum, S2 screennum)
{
    app[appnum].screen[screennum].gadget_color.backgr_light.r = app[appnum].screen[screennum].r;
    app[appnum].screen[screennum].gadget_color.backgr_light.g = app[appnum].screen[screennum].g;
    app[appnum].screen[screennum].gadget_color.backgr_light.b = app[appnum].screen[screennum].b;
}

void set_gadget_color_backgr_shadow (S2 appnum, S2 screennum)
{
    app[appnum].screen[screennum].gadget_color.backgr_shadow.r = app[appnum].screen[screennum].r;
    app[appnum].screen[screennum].gadget_color.backgr_shadow.g = app[appnum].screen[screennum].g;
    app[appnum].screen[screennum].gadget_color.backgr_shadow.b = app[appnum].screen[screennum].b;
}

/* text colors */

void set_gadget_color_text_light (S2 appnum, S2 screennum)
{
    app[appnum].screen[screennum].gadget_color.text_light.r = app[appnum].screen[screennum].r;
    app[appnum].screen[screennum].gadget_color.text_light.g = app[appnum].screen[screennum].g;
    app[appnum].screen[screennum].gadget_color.text_light.b = app[appnum].screen[screennum].b;
}

void set_gadget_color_text_shadow (S2 appnum, S2 screennum)
{
    app[appnum].screen[screennum].gadget_color.text_shadow.r = app[appnum].screen[screennum].r;
    app[appnum].screen[screennum].gadget_color.text_shadow.g = app[appnum].screen[screennum].g;
    app[appnum].screen[screennum].gadget_color.text_shadow.b = app[appnum].screen[screennum].b;
}


void draw_gadget_light (SDL_Renderer *renderer, S2 appnum, S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
{
    lineRGBA (renderer, x, y, x2, y, app[appnum].screen[screennum].gadget_color.border_light.r, app[appnum].screen[screennum].gadget_color.border_light.g, app[appnum].screen[screennum].gadget_color.border_light.b, 255);

    lineRGBA (renderer, x2, y, x2, y2, app[appnum].screen[screennum].gadget_color.border_light.r, app[appnum].screen[screennum].gadget_color.border_light.g, app[appnum].screen[screennum].gadget_color.border_light.b, 255);

    lineRGBA (renderer, x, y + 1, x, y2, app[appnum].screen[screennum].gadget_color.border_shadow.r, app[appnum].screen[screennum].gadget_color.border_shadow.g, app[appnum].screen[screennum].gadget_color.border_shadow.b, 255);

    lineRGBA (renderer, x, y2, x2 - 1, y2, app[appnum].screen[screennum].gadget_color.border_shadow.r, app[appnum].screen[screennum].gadget_color.border_shadow.g, app[appnum].screen[screennum].gadget_color.border_shadow.b, 255);

    boxRGBA (renderer, x + 1, y + 1, x2 - 1, y2 - 1, app[appnum].screen[screennum].gadget_color.backgr_light.r, app[appnum].screen[screennum].gadget_color.backgr_light.g, app[appnum].screen[screennum].gadget_color.backgr_light.b, 255);
}

void draw_gadget_shadow (SDL_Renderer *renderer, S2 appnum, S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
{
    lineRGBA (renderer, x, y, x2, y, app[appnum].screen[screennum].gadget_color.border_shadow.r, app[appnum].screen[screennum].gadget_color.border_shadow.g, app[appnum].screen[screennum].gadget_color.border_shadow.b, 255);

    lineRGBA (renderer, x2, y, x2, y2, app[appnum].screen[screennum].gadget_color.border_shadow.r, app[appnum].screen[screennum].gadget_color.border_shadow.g, app[appnum].screen[screennum].gadget_color.border_shadow.b, 255);

    lineRGBA (renderer, x, y + 1, x, y2, app[appnum].screen[screennum].gadget_color.border_light.r, app[appnum].screen[screennum].gadget_color.border_light.g, app[appnum].screen[screennum].gadget_color.border_light.b, 255);

    lineRGBA (renderer, x, y2, x2 - 1, y2, app[appnum].screen[screennum].gadget_color.border_light.r, app[appnum].screen[screennum].gadget_color.border_light.g, app[appnum].screen[screennum].gadget_color.border_light.b, 255);

    boxRGBA (renderer, x + 1, y + 1, x2 - 1, y2 - 1, app[appnum].screen[screennum].gadget_color.backgr_shadow.r, app[appnum].screen[screennum].gadget_color.backgr_shadow.g, app[appnum].screen[screennum].gadget_color.backgr_shadow.b, 255);
}

void draw_gadget_input (SDL_Renderer *renderer, S2 appnum, S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
{
    /* outer line */

    lineRGBA (renderer, x, y, x2, y, app[appnum].screen[screennum].gadget_color.border_light.r, app[appnum].screen[screennum].gadget_color.border_light.g, app[appnum].screen[screennum].gadget_color.border_light.b, 255);

    lineRGBA (renderer, x2, y, x2, y2, app[appnum].screen[screennum].gadget_color.border_light.r, app[appnum].screen[screennum].gadget_color.border_light.g, app[appnum].screen[screennum].gadget_color.border_light.b, 255);

    lineRGBA (renderer, x, y + 1, x, y2, app[appnum].screen[screennum].gadget_color.border_shadow.r, app[appnum].screen[screennum].gadget_color.border_shadow.g, app[appnum].screen[screennum].gadget_color.border_shadow.b, 255);

    lineRGBA (renderer, x, y2, x2 - 1, y2, app[appnum].screen[screennum].gadget_color.border_shadow.r, app[appnum].screen[screennum].gadget_color.border_shadow.g, app[appnum].screen[screennum].gadget_color.border_shadow.b, 255);

    /* inner line */

    x++;
    y++;
    x2--;
    y2--;

    lineRGBA (renderer, x, y, x2, y, app[appnum].screen[screennum].gadget_color.border_shadow.r, app[appnum].screen[screennum].gadget_color.border_shadow.g, app[appnum].screen[screennum].gadget_color.border_shadow.b, 255);

    lineRGBA (renderer, x2, y, x2, y2, app[appnum].screen[screennum].gadget_color.border_shadow.r, app[appnum].screen[screennum].gadget_color.border_shadow.g, app[appnum].screen[screennum].gadget_color.border_shadow.b, 255);

    lineRGBA (renderer, x, y + 1, x, y2, app[appnum].screen[screennum].gadget_color.border_light.r, app[appnum].screen[screennum].gadget_color.border_light.g, app[appnum].screen[screennum].gadget_color.border_light.b, 255);

    lineRGBA (renderer, x, y2, x2 - 1, y2, app[appnum].screen[screennum].gadget_color.border_light.r, app[appnum].screen[screennum].gadget_color.border_light.g, app[appnum].screen[screennum].gadget_color.border_light.b, 255);

    /* clear inside */

    x++;
    y++;
    x2--;
    y2--;

    boxRGBA (renderer, x, y, x2, y2, app[appnum].screen[screennum].gadget_color.backgr_light.r, app[appnum].screen[screennum].gadget_color.backgr_light.g, app[appnum].screen[screennum].gadget_color.backgr_light.b, 255);
}

void draw_ghost_gadget (SDL_Renderer *renderer, S2 appnum, S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
{
    Sint16 dx, dy;

    for (dy = y + 2; dy <= y2 - 2; dy += 2)
    {
        for (dx = x + 2; dx <= x2 - 2; dx += 2)
        {
            pixelRGBA (renderer, dx, dy, app[appnum].screen[screennum].gadget_color.backgr_shadow.r, app[appnum].screen[screennum].gadget_color.backgr_shadow.g, app[appnum].screen[screennum].gadget_color.backgr_shadow.b, 255);
        }
    }
}

void update_rect (S2 appnum, S2 screennum, Sint16 x, Sint16 y, Sint16 w, Sint16 h)
{
	SDL_Rect rect;
	
	rect.x = x; rect.y = y;
	rect.w = w; rect.h = h;
	
	SDL_UpdateWindowSurfaceRects (app[appnum].screen[screennum].window, &rect, 1);
}
	

U1 draw_gadget_button (SDL_Renderer *renderer, S2 appnum, S2 screennum, U2 gadget_index, U1 selected)
{
    struct gadget_button *button;

    button = (struct gadget_button *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    if (selected == GADGET_NOT_SELECTED)
    {
        draw_gadget_light (renderer, appnum, screennum, button->x, button->y, button->x2, button->y2);

        if (! draw_text_ttf (app[appnum].screen[screennum].surface, appnum, screennum, button->text, button->text_x, button->text_y, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b))
        {
            return (FALSE);
        }
    }
    else
    {
        draw_gadget_shadow (renderer, appnum, screennum, button->x, button->y, button->x2, button->y2);

        if (! draw_text_ttf (app[appnum].screen[screennum].surface, appnum, screennum, button->text, button->text_x, button->text_y, app[appnum].screen[screennum].gadget_color.text_shadow.r, app[appnum].screen[screennum].gadget_color.text_shadow.g, app[appnum].screen[screennum].gadget_color.text_shadow.b))
        {
            return (FALSE);
        }
    }

    if (button->status == GADGET_NOT_ACTIVE)
    {
        draw_ghost_gadget (renderer, appnum, screennum, button->x, button->y, button->x2, button->y2);
    }

    update_rect (appnum, screennum, button->x, button->y, button->x2 - button->x + 1, button->y2 - button->y + 1);
    return (TRUE);
}

void draw_checkmark_light (SDL_Renderer *renderer, S2 appnum, S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
{
    Sint16 dx, dy, dx2, dy2, dx3, dy3, dx4, dy4, height, width;

    height = y2 - y;
    width = x2 - x;

    dx = x + width / 4;
    dy = y + height / 2;

    dx2 = x + width / 2;
    dy2 = y2 - height / 4;

    dx3 = dx2 + width / 10;
    dy3 = dy2 - height / 15;

    dx4 = x2 - width / 4;
    dy4 = y + height / 4;

    filledTrigonRGBA (renderer, dx, dy, dx2, dy2, dx3, dy3, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b, 255);

    filledTrigonRGBA (renderer, dx2, dy2, dx3, dy3, dx4, dy4, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b, 255);
}

void draw_checkmark_shadow (SDL_Renderer *renderer, S2 appnum, S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
{
    Sint16 dx, dy, dx2, dy2, dx3, dy3, dx4, dy4, height, width;

    height = y2 - y;
    width = x2 - x;

    dx = x + width / 4;
    dy = y + height / 2;

    dx2 = x + width / 2;
    dy2 = y2 - height / 4;

    dx3 = dx2 + width / 10;
    dy3 = dy2 - height / 15;

    dx4 = x2 - width / 4;
    dy4 = y + height / 4;

    filledTrigonRGBA (renderer, dx, dy, dx2, dy2, dx3, dy3, app[appnum].screen[screennum].gadget_color.text_shadow.r, app[appnum].screen[screennum].gadget_color.text_shadow.g, app[appnum].screen[screennum].gadget_color.text_shadow.b, 255);

    filledTrigonRGBA (renderer, dx2, dy2, dx3, dy3, dx4, dy4, app[appnum].screen[screennum].gadget_color.text_shadow.r, app[appnum].screen[screennum].gadget_color.text_shadow.g, app[appnum].screen[screennum].gadget_color.text_shadow.b, 255);
}

U1 draw_gadget_checkbox (SDL_Renderer *renderer, S2 appnum, S2 screennum, U2 gadget_index, U1 selected, U1 value)
{
    struct gadget_checkbox *checkbox;

    checkbox = (struct gadget_checkbox *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    if (selected == GADGET_NOT_SELECTED)
    {
        draw_gadget_light (renderer, appnum, screennum, checkbox->x, checkbox->y, checkbox->x2, checkbox->y2);

        if (value == GADGET_CHECKBOX_TRUE)
        {
            draw_checkmark_light (renderer, appnum, screennum, checkbox->x, checkbox->y, checkbox->x2, checkbox->y2);
        }
    }
    else
    {
        draw_gadget_shadow (renderer, appnum, screennum, checkbox->x, checkbox->y, checkbox->x2, checkbox->y2);

        if (value == GADGET_CHECKBOX_TRUE)
        {
            draw_checkmark_shadow (renderer, appnum, screennum, checkbox->x, checkbox->y, checkbox->x2, checkbox->y2);
        }
    }

    if (checkbox->status == GADGET_NOT_ACTIVE)
    {
        draw_ghost_gadget (renderer, appnum, screennum, checkbox->x, checkbox->y, checkbox->x2, checkbox->y2);
    }

    boxRGBA (renderer, checkbox->text_x, checkbox->text_y, checkbox->text_x2, checkbox->text_y2, app[appnum].screen[screennum].gadget_color.backgr_light.r, app[appnum].screen[screennum].gadget_color.backgr_light.g, app[appnum].screen[screennum].gadget_color.backgr_light.b, 255);

    if (! draw_text_ttf (app[appnum].screen[screennum].surface, appnum, screennum, checkbox->text, checkbox->text_x, checkbox->text_y, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b))
    {
        return (FALSE);
    }

    update_rect (appnum, screennum, checkbox->x, checkbox->y, checkbox->x2 - checkbox->x + 1, checkbox->y2 - checkbox->y + 1);
    update_rect (appnum, screennum, checkbox->text_x, checkbox->text_y, checkbox->text_x2 - checkbox->text_x + 1, checkbox->text_y2 - checkbox->text_y + 1);
    return (TRUE);
}

void draw_cycle_arrow_light (SDL_Renderer *renderer, S2 appnum, S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
{
    Sint16 dx, dy, dx2, dy2, dx3, dy3, dx4, dy4, dx5, dy5, dx6, dy6, dx7, dy7, dx8, dy8, dx9, dy9, height, width;

    height = y2 - y;
    width = x2 - x;

    /* arrow */

    dx = x + width / 4;
    dy = y2 - height / 4;

    dx2 = dx - width / 8;
    dy2 = dy - height / 8;

    dx3 = dx + width / 8;
    dy3 = dy2;

    /* box left */

    dx4 = dx - width / 24;
    dy4 = y + height / 4;
    dx5 = dx + width / 24;
    dy5 = dy2;

    /* box top */

    dx6 = dx5;
    dy6 = dy4;
    dx7 = x2 - width / 4;
    dy7 = dy4 + height / 24;

    /* box right */

    dx8 = dx7 - width / 24;
    dy8 = dy7;
    dx9 = dx7;
    dy9 = dy2;

    filledTrigonRGBA (renderer, dx, dy, dx2, dy2, dx3, dy3, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b, 255);

    boxRGBA (renderer, dx4, dy4, dx5, dy5, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b, 255);

    boxRGBA (renderer, dx6, dy6, dx7, dy7, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b, 255);

    boxRGBA (renderer, dx8, dy8, dx9, dy9, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b, 255);
}

void draw_cycle_arrow_shadow (SDL_Renderer *renderer, S2 appnum, S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
{
    Sint16 dx, dy, dx2, dy2, dx3, dy3, dx4, dy4, dx5, dy5, dx6, dy6, dx7, dy7, dx8, dy8, dx9, dy9, height, width;

    height = y2 - y;
    width = x2 - x;

    /* arrow */

    dx = x + width / 4;
    dy = y2 - height / 4;

    dx2 = dx - width / 8;
    dy2 = dy - height / 8;

    dx3 = dx + width / 8;
    dy3 = dy2;

    /* box left */

    dx4 = dx - width / 24;
    dy4 = y + height / 4;
    dx5 = dx + width / 24;
    dy5 = dy2;

    /* box top */

    dx6 = dx5;
    dy6 = dy4;
    dx7 = x2 - width / 4;
    dy7 = dy4 + height / 24;

    /* box right */

    dx8 = dx7 - width / 24;
    dy8 = dy7;
    dx9 = dx7;
    dy9 = dy2;

    filledTrigonRGBA (renderer, dx, dy, dx2, dy2, dx3, dy3, app[appnum].screen[screennum].gadget_color.text_shadow.r, app[appnum].screen[screennum].gadget_color.text_shadow.g, app[appnum].screen[screennum].gadget_color.text_shadow.b, 255);

    boxRGBA (renderer, dx4, dy4, dx5, dy5, app[appnum].screen[screennum].gadget_color.text_shadow.r, app[appnum].screen[screennum].gadget_color.text_shadow.g, app[appnum].screen[screennum].gadget_color.text_shadow.b, 255);

    boxRGBA (renderer, dx6, dy6, dx7, dy7, app[appnum].screen[screennum].gadget_color.text_shadow.r, app[appnum].screen[screennum].gadget_color.text_shadow.g, app[appnum].screen[screennum].gadget_color.text_shadow.b, 255);

    boxRGBA (renderer, dx8, dy8, dx9, dy9, app[appnum].screen[screennum].gadget_color.text_shadow.r, app[appnum].screen[screennum].gadget_color.text_shadow.g, app[appnum].screen[screennum].gadget_color.text_shadow.b, 255);
}


int copy_surface (SDL_Renderer *dest_renderer, SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
{
    Uint32 pixel;
    Uint8 r, g, b;
    Sint16 src_x, src_y, dst_x, dst_y;
    Sint16 srcrect_x, srcrect_y, srcrect_w, srcrect_h;
    Sint16 dstrect_x, dstrect_y;

    if (srcrect == NULL)
    {
        srcrect_x = 0;
        srcrect_y = 0;
        srcrect_w = src->w;
        srcrect_h = src->h;
    }
    else
    {
        srcrect_x = srcrect->x;
        srcrect_y = srcrect->y;
        srcrect_w = srcrect->w;
        srcrect_h = srcrect->h;
    }

    if (dstrect == NULL)
    {
        dstrect_x = 0;
        dstrect_y = 0;
    }
    else
    {
        dstrect_x = dstrect->x;
        dstrect_y = dstrect->y;
    }

    dst_y = dstrect_y;

    for (src_y = srcrect_y; src_y < srcrect_y + srcrect_h; src_y++)
    {
        dst_x = dstrect_x;

        for (src_x = srcrect_x; src_x < srcrect_x + srcrect_w; src_x++)
        {
            if (SDL_LockSurface (src) < 0)
            {
                printf ("copy_surface: can't lock source surface!\n");
                return (-1);
            }

            pixel = getpixel (src, src_x, src_y);
            SDL_UnlockSurface (src);
            SDL_GetRGB (pixel, src->format, &r, &g, &b);

            pixelRGBA (dest_renderer, dst_x, dst_y, r, g, b, 255);

            dst_x++;
        }

        dst_y++;
    }
    return (0);
}

U1 draw_gadget_cycle (SDL_Renderer *renderer, S2 appnum, S2 screennum, U2 gadget_index, U1 selected, U1 value)
{
    struct gadget_cycle *cycle;
    Uint32 rmask, gmask, bmask, amask;
    SDL_Rect menu_rect, copy_rect;
    S4 i;
    Sint16 text_x, text_y;

    /* SDL interprets each pixel as a 32-bit number, so our masks must depend */
    /* on the endianness (byte order) of the machine */

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif

    cycle = (struct gadget_cycle *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    switch (selected)
    {
        case GADGET_NOT_SELECTED:
            draw_gadget_light (renderer, appnum, screennum, cycle->x, cycle->y, cycle->x2, cycle->y2);

            if (! draw_text_ttf (app[appnum].screen[screennum].surface, appnum, screennum, cycle->text[value], cycle->text_x, cycle->text_y, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b))
            {
                return (FALSE);
            }

            draw_cycle_arrow_light (renderer, appnum, screennum, cycle->arrow_x, cycle->arrow_y, cycle->x2, cycle->y2);
            break;

        case GADGET_SELECTED:
            draw_gadget_shadow (renderer, appnum, screennum, cycle->x, cycle->y, cycle->x2, cycle->y2);

            if (! draw_text_ttf (app[appnum].screen[screennum].surface, appnum, screennum, cycle->text[value], cycle->text_x, cycle->text_y, app[appnum].screen[screennum].gadget_color.text_shadow.r, app[appnum].screen[screennum].gadget_color.text_shadow.g, app[appnum].screen[screennum].gadget_color.text_shadow.b))
            {
                return (FALSE);
            }

            draw_cycle_arrow_shadow (renderer, appnum, screennum, cycle->arrow_x, cycle->arrow_y, cycle->x2, cycle->y2);
            break;

        case GADGET_MENU_DOWN:
            if (! cycle->menu)
            {
                /* allocate backup surface, to copy the area that will be covered by the menu */

                app[appnum].screen[screennum].copy_surface = SDL_CreateRGBSurface (SDL_SWSURFACE, cycle->menu_x2 - cycle->menu_x + 1, cycle->menu_y2 - cycle->menu_y + 1, app[appnum].screen[screennum].video_bpp, rmask, gmask, bmask, amask);
                if (app[appnum].screen[screennum].copy_surface == NULL)
                {
                    printf ("draw_gadget_cycle: error can't allocate copy %i surface!\n", gadget_index);
                    return (FALSE);
                }

                /* Use alpha blending */
                if (SDL_SetSurfaceBlendMode (app[appnum].screen[screennum].copy_surface, SDL_BLENDMODE_BLEND) < 0)
                {
                    printf ("draw_gadget_cycle: error can't set copy alpha channel %i surface!\n", gadget_index);
                    return (FALSE);
                }

                app[appnum].screen[screennum].copy_renderer = SDL_CreateSoftwareRenderer (app[appnum].screen[screennum].copy_surface);
                
                /* make copy of menu area */

                menu_rect.x = cycle->menu_x;
                menu_rect.y = cycle->menu_y;
                menu_rect.w = cycle->menu_x2 - cycle->menu_x + 1;
                menu_rect.h = cycle->menu_y2 - cycle->menu_y + 1;

                copy_rect.x = 0;
                copy_rect.y = 0;

                if (copy_surface (app[appnum].screen[screennum].copy_renderer, app[appnum].screen[screennum].surface, &menu_rect, app[appnum].screen[screennum].copy_surface, &copy_rect) < 0)
                {
                    printf ("draw_gadget_cycle: error can't blit copy %i surface!\n", gadget_index);
                    return (FALSE);
                }

                /* allocate menu surface */

                app[appnum].screen[screennum].temp_surface = SDL_CreateRGBSurface (SDL_SWSURFACE, cycle->menu_x2 - cycle->menu_x + 1, cycle->menu_y2 - cycle->menu_y + 1, app[appnum].screen[screennum].video_bpp, rmask, gmask, bmask, amask);
                if (app[appnum].screen[screennum].temp_surface == NULL)
                {
                    printf ("draw_gadget_cycle: error can't allocate menu %i surface!\n", gadget_index);
                    return (FALSE);
                }

                /* Use alpha blending */
                if (SDL_SetSurfaceBlendMode (app[appnum].screen[screennum].temp_surface, SDL_BLENDMODE_BLEND) < 0)
                {
                    printf ("draw_gadget_cycle: error can't set menu alpha channel %i surface!\n", gadget_index);
                    return (FALSE);
                }
                
                /* set renderer */
				app[appnum].screen[screennum].temp_renderer = SDL_CreateSoftwareRenderer (app[appnum].screen[screennum].temp_surface);

                cycle->menu = TRUE;
            }

            /* draw menu */

            draw_gadget_light (app[appnum].screen[screennum].renderer, appnum, screennum, 0, 0, cycle->menu_x2 - cycle->menu_x, cycle->menu_y2 - cycle->menu_y);

            text_x = cycle->text_x - cycle->menu_x;
            text_y = cycle->text_y - cycle->y;

            for (i = 0; i < cycle->menu_entries; i++)
            {
                if (i == value)
                {
                    boxRGBA (app[appnum].screen[screennum].renderer, 1, text_y, cycle->menu_x2 - cycle->menu_x - 1, text_y + cycle->text_height, app[appnum].screen[screennum].gadget_color.backgr_shadow.r, app[appnum].screen[screennum].gadget_color.backgr_shadow.g, app[appnum].screen[screennum].gadget_color.backgr_shadow.b, 255);

                    if (! draw_text_ttf (app[appnum].screen[screennum].temp_surface, appnum, screennum, cycle->text[i], text_x, text_y, app[appnum].screen[screennum].gadget_color.text_shadow.r, app[appnum].screen[screennum].gadget_color.text_shadow.g, app[appnum].screen[screennum].gadget_color.text_shadow.b))
                    {
                        return (FALSE);
                    }
                }
                else
                {
                    if (! draw_text_ttf (app[appnum].screen[screennum].temp_surface, appnum, screennum, cycle->text[i], text_x, text_y, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b))
                    {
                        return (FALSE);
                    }
                }

                text_y += cycle->text_height + (cycle->text_height / 2);
            }

            menu_rect.x = cycle->menu_x;
            menu_rect.y = cycle->menu_y;

            if (copy_surface (app[appnum].screen[screennum].renderer, app[appnum].screen[screennum].temp_surface, NULL, app[appnum].screen[screennum].surface, &menu_rect) < 0)
            {
                printf ("draw_gadget_cycle: error can't blit menu %i surface!\n", gadget_index);
                return (FALSE);
            }

            update_rect (appnum, screennum, cycle->menu_x, cycle->menu_y, cycle->menu_x2 - cycle->menu_x + 1, cycle->menu_y2 - cycle->menu_y + 1);
            break;

        case GADGET_MENU_UP:
            /* close menu: copy backup surface */

            menu_rect.x = cycle->menu_x;
            menu_rect.y = cycle->menu_y;

            if (copy_surface (app[appnum].screen[screennum].renderer, app[appnum].screen[screennum].copy_surface, NULL, app[appnum].screen[screennum].surface, &menu_rect) < 0)
            {
                printf ("draw_gadget_cycle: error can't blit copy %i surface!\n", gadget_index);
                return (FALSE);
            }

            update_rect (appnum, screennum, cycle->menu_x, cycle->menu_y, cycle->menu_x2 - cycle->menu_x + 1, cycle->menu_y2 - cycle->menu_y + 1);

            /* redraw cycle gadget */

            draw_gadget_light (renderer, appnum, screennum, cycle->x, cycle->y, cycle->x2, cycle->y2);

            if (! draw_text_ttf (app[appnum].screen[screennum].surface, appnum, screennum, cycle->text[value], cycle->text_x, cycle->text_y, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b))
            {
                return (FALSE);
            }

            draw_cycle_arrow_light (renderer, appnum, screennum, cycle->arrow_x, cycle->arrow_y, cycle->x2, cycle->y2);

            /* free bitmaps */

            if (app[appnum].screen[screennum].copy_surface)
            {
                SDL_FreeSurface (app[appnum].screen[screennum].copy_surface);
                app[appnum].screen[screennum].copy_surface = NULL;
            }
            if (app[appnum].screen[screennum].temp_surface)
            {
                SDL_FreeSurface (app[appnum].screen[screennum].temp_surface);
                app[appnum].screen[screennum].temp_surface = NULL;
            }

            cycle->menu = FALSE;
            break;
    }

    if (cycle->status == GADGET_NOT_ACTIVE)
    {
        draw_ghost_gadget (renderer, appnum, screennum, cycle->x, cycle->y, cycle->x2, cycle->y2);
    }

    update_rect (appnum, screennum, cycle->x, cycle->y, cycle->x2 - cycle->x + 1, cycle->y2 - cycle->y + 1);
    return (TRUE);
}

U1 draw_gadget_progress_bar (SDL_Renderer *renderer, S2 appnum, S2 screennum, U2 gadget_index, U1 selected)
{
    struct gadget_progress_bar *progress;
	Sint16 bar_x, bar_len;

    progress = (struct gadget_progress_bar *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

	bar_len = (progress->x2 - progress->x) - 2;
	bar_x = progress->x + 1 + ((bar_len * progress->value) / 100);

    if (selected == GADGET_NOT_SELECTED)
    {
        draw_gadget_light (renderer, appnum, screennum, progress->x, progress->y, progress->x2, progress->y2);

		boxRGBA (renderer, progress->x + 1, progress->y + 1, bar_x, progress->y2 - 1, 18, 98, 245, 255);

        if (! draw_text_ttf (app[appnum].screen[screennum].surface, appnum, screennum, progress->text, progress->text_x, progress->text_y, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b))
        {
            return (FALSE);
        }
    }
    else
    {
        draw_gadget_shadow (renderer, appnum, screennum, progress->x, progress->y, progress->x2, progress->y2);

		boxRGBA (app[appnum].screen[screennum].renderer, progress->x + 1, progress->y + 1, bar_x, progress->y2 - 1, 18, 98, 245, 255);

        if (! draw_text_ttf (app[appnum].screen[screennum].surface, appnum, screennum, progress->text, progress->text_x, progress->text_y, app[appnum].screen[screennum].gadget_color.text_shadow.r, app[appnum].screen[screennum].gadget_color.text_shadow.g, app[appnum].screen[screennum].gadget_color.text_shadow.b))
        {
            return (FALSE);
        }
    }

    if (progress->status == GADGET_NOT_ACTIVE)
    {
        draw_ghost_gadget (renderer, appnum, screennum, progress->x, progress->y, progress->x2, progress->y2);
    }

    update_rect (appnum, screennum, progress->x, progress->y, progress->x2 - progress->x + 1, progress->y2 - progress->y + 1);
    return (TRUE);
}

U1 draw_gadget_string (SDL_Renderer *renderer, S2 appnum, S2 screennum, U2 gadget_index, U1 selected)
{
    S2 value_len;
    Sint16 cursor_x;

    struct gadget_string *string;

    string = (struct gadget_string *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    draw_gadget_input (renderer, appnum, screennum, string->x, string->y, string->x2, string->y2);

    boxRGBA (renderer, string->text_x, string->text_y, string->text_x2, string->text_y2, app[appnum].screen[screennum].gadget_color.backgr_light.r, app[appnum].screen[screennum].gadget_color.backgr_light.g, app[appnum].screen[screennum].gadget_color.backgr_light.b, 255);

    if (! draw_text_ttf (app[appnum].screen[screennum].surface, appnum, screennum, string->text, string->text_x, string->text_y, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b))
    {
        return (FALSE);
    }

    /* draw value string (input) */

    value_len = strlen (string->value);
    if (value_len > 0)
    {
        if (value_len <= string->visible_len)
        {
            /* value string fits in gadget */

            if (! draw_text_ttf (app[appnum].screen[screennum].surface, appnum, screennum, string->value, string->input_x, string->input_y, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b))
            {
                return (FALSE);
            }
        }
    }

    if (selected == GADGET_SELECTED)
    {
        if (string->cursor_pos == -1)
        {
            /* init cursor position */

            if (value_len <= string->visible_len)
            {
                string->cursor_pos = value_len;
            }
            else
            {
                string->cursor_pos = string->visible_len;
            }

            string->insert_pos = string->cursor_pos;
        }

        /* draw cursor */

        cursor_x = string->input_x + (string->cursor_width * string->cursor_pos);

        boxRGBA (renderer, cursor_x, string->input_y, cursor_x + string->cursor_width, string->input_y + string->cursor_height, app[appnum].screen[screennum].gadget_color.text_light.r, app[appnum].screen[screennum].gadget_color.text_light.g, app[appnum].screen[screennum].gadget_color.text_light.b, 50);
    }

    if (string->status == GADGET_NOT_ACTIVE)
    {
        draw_ghost_gadget (renderer, appnum, screennum, string->x, string->y, string->x2, string->y2);
    }

    update_rect (appnum, screennum, string->x, string->y, string->x2 - string->x + 1, string->y2 - string->y + 1);
    update_rect (appnum, screennum, string->text_x, string->text_y, string->text_x2 - string->text_x + 1, string->text_y2 - string->text_y + 1);
    return (TRUE);
}



U1 event_gadget_string (S2 appnum, S2 screennum, U2 gadget_index)
{
    SDL_Event event;
    SDL_Keycode key;

    U1 *string_buf, wait;
    S2 value_len;

    struct gadget_string *string;

    string = (struct gadget_string *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    /* allocate string buffer */

    string_buf = (U1 *) malloc ((string->string_len + 1) * sizeof (U1));
    if (string_buf == NULL)
    {
        printf ("event_gadget_string: error can't allocate string buffer!\n");
        return (FALSE);
    }

    if (! draw_gadget_string (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_SELECTED))
    {
        free (string_buf);
        return (FALSE);
    }

    #if __ANDROID__
		/* show on screen keyboard */
		
		SDL_ANDROID_ToggleScreenKeyboardTextInput (string->value);
	#endif
    
    /* wait for event */

    wait = TRUE;

    while (wait)
    {
        if (! SDL_WaitEvent (&event))
        {
            printf ("event_gadget_string: error can't wait for event!\n");
            free (string_buf);
            return FALSE;
        }

        value_len = strlen (string->value);

        switch (event.type)
        {
            case SDL_KEYDOWN:
                key = event.key.keysym.sym;
                switch (key)
                {
                    case SDLK_BACKSPACE:
                        printf ("event_gadget_string: BACKSPACE\n");

                        if (string->insert_pos > 0)
                        {
                            strremoveleft (string_buf, string->value, string->insert_pos);
                            strcpy (string->value, string_buf);

                            string->cursor_pos--;
                            string->insert_pos--;

                            if (! draw_gadget_string (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_SELECTED))
                            {
                                free (string_buf);
                                return (FALSE);
                            }
                        }
                        break;

                    case SDLK_DELETE:
                        printf ("event_gadget_string: DELETE\n");

                        strremoveright (string_buf, string->value, string->insert_pos);
                        strcpy (string->value, string_buf);

                        if (! draw_gadget_string (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_SELECTED))
                        {
                            free (string_buf);
                            return (FALSE);
                        }
                        break;

                        break;

                    case SDLK_LEFT:
                        printf ("event_gadget_string: CURSOR LEFT\n");

                        if (string->cursor_pos > 0)
                        {
                            string->cursor_pos--;
                            string->insert_pos--;

                            if (! draw_gadget_string (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_SELECTED))
                            {
                                free (string_buf);
                                return (FALSE);
                            }
                        }

                        break;

                    case SDLK_RIGHT:
                        printf ("event_gadget_string: CURSOR RIGHT\n");

                        if (string->cursor_pos < value_len)
                        {
                            string->cursor_pos++;
                            string->insert_pos++;

                            if (! draw_gadget_string (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_SELECTED))
                            {
                                free (string_buf);
                                return (FALSE);
                            }
                        }
                        break;

                    case SDLK_RETURN:
                        printf ("event_gadget_string: RETURN\n");
                        wait = FALSE;
                        break;

                    default:
						printf ("event_gadget_string: char: %i\n", SDL_GetKeyName (event.key.keysym.sym));

                        if (value_len < string->string_len && SDL_GetKeyName (event.key.keysym.sym) != 0)
                        {
                                strinsertchar (string_buf, string->value, SDL_GetKeyName (event.key.keysym.sym), string->insert_pos);
                                strcpy (string->value, string_buf);

                                if (string->cursor_pos < string->visible_len)
                                {
                                    string->cursor_pos++;
                                }

                                string->insert_pos++;

                                if (! draw_gadget_string (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_SELECTED))
                                {
                                    free (string_buf);
                                    return (FALSE);
                                }
                        }
                        break;

                }
                break;
				
			case SDL_MOUSEBUTTONDOWN:
				printf ("lost focus: MOUSE clicked!\n");
				wait = FALSE;
				break;
        }
    }

    free (string_buf);
    return (TRUE);
}

void get_finger_position (float tfinger_x, float tfinger_y, int *x, int *y)
{
	x = app[0].screen[0].width * tfinger_x;
	y = app[0].screen[0].height * tfinger_y;
}

void get_user_position (int *x, int *y, *button)
{
	Uint8 buttonmask;
	SDL_Event event;
	
	SDL_PumpEvents ();
	SDL_PollEvent (&event);
	
#if __ANDROID__
	switch (event.type)
	{
		case SDL_FINGERDOWN:
			get_finger_position (event.tfinger.x, event.tfinger.y, x, y);
			button = TRUE;
			break;
			
		case SDL_FINGERUP:
			button = FALSE;
			break;
	}
#else
	buttonmask = SDL_GetMouseState (x, y);

    if (buttonmask & SDL_BUTTON (1))
	{
		button = TRUE;
	}
	else
	{
		button = FALSE;
	}
#endif	
}

void gadget_event (S2 appnum, S2 screennum)
{
    SDL_Event event;
    U1 wait, cycle_menu;
    Uint8 buttonmask, mouse_button, double_click, select;
    U2 gadget, i;
    S4 value, old_value;
    int x, y;
	Uint32 window_id = SDL_GetWindowID (app[appnum].screen[screennum].window);
	int window_selected = TRUE;
	
    struct gadget_button *button;
    struct gadget_checkbox *checkbox;
    struct gadget_cycle *cycle;
    struct gadget_string *string;
    struct gadget_box *box;

	printf ("gadget event...\n");
	
    if (! app[appnum].screen[screennum].gadget)
    {
        printf ("gadget_event: error gadget list not allocated!\n");
        send_8 (app[appnum].tcpsock, ERROR);
        return;
    }

    while (1 == 1)
    {
        wait = TRUE;

        while (wait)
        {
			/* draw mouse cursor */
		
			// SDL_GetMouseState (&mouse_x, &mouse_y);
			// draw_cursor (cursor, mouse_x, mouse_y);
			// update_screen (0);
		
			
			/* wait for event */
            if (! SDL_WaitEvent (&event))
            {
                printf ("gadget_event: error can't wait for event!\n");
                send_8 (app[appnum].tcpsock, ERROR);
                return;
            }

            switch (event.type)
            {
				/*
				case SDL_WINDOWEVENT:
					if (event.window.windowID == window_id)
					{
						switch (event.window.event)
						{
							case SDL_WINDOWEVENT_ENTER:
								if (event.window.windowID == window_id)
								{
									window_selected = TRUE;
								}
								break;
					
							case SDL_WINDOWEVENT_LEAVE:
								if (event.window.windowID == window_id)
								{
									window_selected = FALSE;
								}
								break;
						}
					}
					*/
				
#if __ANDROID__
				case SDL_FINGERDOWN:
					get_finger_position (event.tfinger.x, event.tfinger.y, &x, &y);
					wait = FALSE;
					break;
#else
				
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == 1 && window_selected == TRUE)
                    {
                        x = event.button.x;
                        y = event.button.y;
                        wait = FALSE;
                    }
                    break;
#endif
            }
        }

        for (i = 0; i < app[appnum].screen[screennum].gadgets; i++)
        {
            if (app[appnum].screen[screennum].gadget[i].gptr)
            {
                switch (app[appnum].screen[screennum].gadget[i].type)
                {
                    case GADGET_BUTTON:
                        button = (struct gadget_button *) app[appnum].screen[screennum].gadget[i].gptr;

                        if ((x > button->x && x < button->x2) && (y > button->y && y < button->y2) && button->status == GADGET_ACTIVE)
                        {
                            mouse_button = 1;
                            while (mouse_button == 1)
                            {
                                SDL_Delay (100);

								get_user_position (&x, &y, &mouse_button);
								
                                if ((x > button->x && x < button->x2) && (y > button->y && y < button->y2))
                                {
                                    printf ("gadget_event: drawing button %i selected.\n", i);

                                    if (! draw_gadget_button (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_SELECTED))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_8 (app[appnum].tcpsock, ERROR);
                                        return;
                                    }
                                }
                                else
                                {
                                    printf ("gadget_event: drawing button %i normal.\n", i);

                                    if (! draw_gadget_button (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_NOT_SELECTED))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_8 (app[appnum].tcpsock, ERROR);
                                        return;
                                    }
                                }
                            }

                            if (! draw_gadget_button (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_NOT_SELECTED))
                            {
                                printf ("gadget_event: error can't draw gadget!\n");
                                send_8 (app[appnum].tcpsock, ERROR);
                                return;
                            }

                            if ((x > button->x && x < button->x2) && (y > button->y && y < button->y2))
                            {
                                /* gadget was selected */

                                printf ("gadget_event: button gadget %i selected.\n", i);

                                /* send data */
                                send_8 (app[appnum].tcpsock, OK);
                                send_8 (app[appnum].tcpsock, GADGET);
                                send_16 (app[appnum].tcpsock, i);
                                send_8 (app[appnum].tcpsock, END);
                                return;
                            }
                        }
                        break;

                    case GADGET_CHECKBOX:
                        checkbox = (struct gadget_checkbox *) app[appnum].screen[screennum].gadget[i].gptr;

                        if ((x > checkbox->x && x < checkbox->x2) && (y > checkbox->y && y < checkbox->y2) && checkbox->status == GADGET_ACTIVE)
                        {
                            mouse_button = 1;
                            while (mouse_button == 1)
                            {
                                SDL_Delay (100);

                                SDL_PumpEvents ();
                                buttonmask = SDL_GetMouseState (&x, &y);

                                if (! (buttonmask & SDL_BUTTON (1)))
                                {
                                    mouse_button = 0;
                                }

                                if ((x > checkbox->x && x < checkbox->x2) && (y > checkbox->y && y < checkbox->y2))
                                {
                                    printf ("gadget_event: drawing checkbox %i selected.\n", i);

                                    if (! draw_gadget_checkbox (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_SELECTED, checkbox->value))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_8 (app[appnum].tcpsock, ERROR);
                                        return;
                                    }
                                }
                                else
                                {
                                    printf ("gadget_event: drawing checkbox %i normal.\n", i);

                                    if (! draw_gadget_checkbox (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_NOT_SELECTED, checkbox->value))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_8 (app[appnum].tcpsock, ERROR);
                                        return;
                                    }
                                }
                            }

                            if (! draw_gadget_checkbox (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_NOT_SELECTED, checkbox->value))
                            {
                                printf ("gadget_event: error can't draw gadget!\n");
                                send_8 (app[appnum].tcpsock, ERROR);
                                return;
                            }

                            if ((x > checkbox->x && x < checkbox->x2) && (y > checkbox->y && y < checkbox->y2))
                            {
                                /* gadget was selected */

                                if (checkbox->value == GADGET_CHECKBOX_TRUE)
                                {
                                    checkbox->value = GADGET_CHECKBOX_FALSE;
                                }
                                else
                                {
                                    checkbox->value = GADGET_CHECKBOX_TRUE;
                                }

                                if (! draw_gadget_checkbox (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_NOT_SELECTED, checkbox->value))
                                {
                                    printf ("gadget_event: error can't draw gadget!\n");
                                    send_8 (app[appnum].tcpsock, ERROR);
                                    return;
                                }

                                printf ("gadget_event: checkbox gadget %i selected.\n", i);

                                /* send data */
                                send_8 (app[appnum].tcpsock, OK);
                                send_8 (app[appnum].tcpsock, GADGET);
                                send_16 (app[appnum].tcpsock, i);
                                send_8 (app[appnum].tcpsock, GADGET_INT_VALUE);
                                send_16 (app[appnum].tcpsock, checkbox->value);
                                send_8 (app[appnum].tcpsock, END);
                                return;
                            }
                        }
                        break;

                    case GADGET_CYCLE:
                        cycle = (struct gadget_cycle *) app[appnum].screen[screennum].gadget[i].gptr;

                        if ((x > cycle->x && x < cycle->x2) && (y > cycle->y && y < cycle->y2) && cycle->status == GADGET_ACTIVE)
                        {
                            mouse_button = 1;
                            while (mouse_button == 1)
                            {
                                SDL_Delay (100);

                                SDL_PumpEvents ();
                                buttonmask = SDL_GetMouseState (&x, &y);

                                if (! (buttonmask & SDL_BUTTON (1)))
                                {
                                    mouse_button = 0;
                                }

                                if ((x > cycle->x && x < cycle->x2) && (y > cycle->y && y < cycle->y2))
                                {
                                    printf ("gadget_event: drawing cycle %i selected.\n", i);

                                    if (! draw_gadget_cycle (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_SELECTED, cycle->value))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_8 (app[appnum].tcpsock, ERROR);
                                        return;
                                    }
                                }
                                else
                                {
                                    printf ("gadget_event: drawing cycle %i normal.\n", i);

                                    if (! draw_gadget_cycle (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_NOT_SELECTED, cycle->value))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_8 (app[appnum].tcpsock, ERROR);
                                        return;
                                    }
                                }
                            }

                            if (! draw_gadget_cycle (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_NOT_SELECTED, cycle->value))
                            {
                                printf ("gadget_event: error can't draw gadget!\n");
                                send_8 (app[appnum].tcpsock, ERROR);
                                return;
                            }

                            if ((x > cycle->x && x < cycle->x2) && (y > cycle->y && y < cycle->y2))
                            {
                                /* gadget was selected */

                                if (! draw_gadget_cycle (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_MENU_DOWN, cycle->value))
                                {
                                    printf ("gadget_event: error can't draw gadget!\n");
                                    send_8 (app[appnum].tcpsock, ERROR);
                                    return;
                                }

                                cycle_menu = TRUE;
                                while (cycle_menu)
                                {
                                    mouse_button = 0; double_click = 0; select = 0;
									old_value = -1;
									
/* On Android the highlighted menu entry will be selected with a double click
 * this was needed, otherwise the second and following entries could never be selected!
 * On normal desktop system one click is used!
 * 
 * This was a FIX to make this usable on Android.
 */
									
									
#if GADGET_CYCLE_DOUBLECLICK 
									while (mouse_button == 0)
                                    {
										select = 0;
										SDL_Delay (100);

                                        SDL_PumpEvents ();
                                        buttonmask = SDL_GetMouseState (&x, &y);

                                        if (buttonmask & SDL_BUTTON (1))
                                        {
                                            select = 1;
                                        }
										
										
										
                                        if ((x > cycle->menu_x && x < cycle->menu_x2) && (y > cycle->menu_y && y < cycle->menu_y2))
                                        {
                                            for (value = 0; value < cycle->menu_entries; value++)
                                            {
                                                if ((y > cycle->menu_y + (value * (cycle->text_height + (cycle->text_height / 2)))) && ((y < cycle->menu_y + ((value + 1) * (cycle->text_height + (cycle->text_height / 2))))))
                                                {
                                                    cycle->value = value;
													
													if (value == old_value)
													{
														if (select == 1)
														{
															/* mousebutton pressed, wait for release */
															
															double_click = 0;
															
															while (double_click == 0)
															{	
																SDL_Delay (100);

																SDL_PumpEvents ();
																buttonmask = SDL_GetMouseState (&x, &y);

																if (! (buttonmask & SDL_BUTTON (1)))
																{
																	double_click = 1;
																	
																	/* button down */
																}
															}
															
															
															
															if (double_click == 1)
															{
																/* button up = selected */
																if ((y > cycle->menu_y + (value * (cycle->text_height + (cycle->text_height / 2)))) && ((y < cycle->menu_y + ((value + 1) * (cycle->text_height + (cycle->text_height / 2))))))
																{
																	/* click in the same menu entry: select item */
																	mouse_button = 1;
																}
															}
														}
													}
													old_value = value;
                                                }
                                            }

                                            if (! draw_gadget_cycle (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_MENU_DOWN, cycle->value))
                                            {
                                                printf ("gadget_event: error can't draw gadget!\n");
                                                send_8 (app[appnum].tcpsock, ERROR);
                                                return;
                                            }
                                        }
									}   
#else
                                    while (mouse_button == 0)
                                    {
                                        SDL_Delay (100);

                                        SDL_PumpEvents ();
                                        buttonmask = SDL_GetMouseState (&x, &y);

                                        if (buttonmask & SDL_BUTTON (1))
                                        {
                                            mouse_button = 1;
                                        }

                                        if ((x > cycle->menu_x && x < cycle->menu_x2) && (y > cycle->menu_y && y < cycle->menu_y2))
                                        {
                                            for (value = 0; value < cycle->menu_entries; value++)
                                            {
                                                if ((y > cycle->menu_y + (value * (cycle->text_height + (cycle->text_height / 2)))) && ((y < cycle->menu_y + ((value + 1) * (cycle->text_height + (cycle->text_height / 2))))))
                                                {
                                                    cycle->value = value;
                                                }
                                            }

                                            if (! draw_gadget_cycle (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_MENU_DOWN, cycle->value))
                                            {
                                                printf ("gadget_event: error can't draw gadget!\n");
                                                send_8 (app[appnum].tcpsock, ERROR);
                                                return;
                                            }
                                        }
                                    }
#endif

                                    if ((x > cycle->menu_x && x < cycle->menu_x2) && (y > cycle->menu_y && y < cycle->menu_y2))
                                    {
                                        /* gadget was selected */

                                        cycle_menu = FALSE;
                                    }
                                }

                                /* remove all pending mouse down events */

                                wait = TRUE;

                                while (wait)
                                {
                                    while (SDL_PollEvent (&event))
                                    {
                                        if (event.type == SDL_MOUSEBUTTONUP && event.button.state == SDL_RELEASED)
                                        {
                                            wait = FALSE;
                                        }
                                    }
                                }
                                SDL_Delay (100);

                                if (! draw_gadget_cycle (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_MENU_UP, cycle->value))
                                {
                                    printf ("gadget_event: error can't draw gadget!\n");
                                    send_8 (app[appnum].tcpsock, ERROR);
                                    return;
                                }

                                printf ("gadget_event: cycle gadget %i selected.\n", i);

                                send_8 (app[appnum].tcpsock, OK);
                                send_8 (app[appnum].tcpsock, GADGET);
                                send_16 (app[appnum].tcpsock, i);
                                send_8 (app[appnum].tcpsock, GADGET_INT_VALUE);
                                send_16 (app[appnum].tcpsock, cycle->value);
                                send_8 (app[appnum].tcpsock, END);
                                return;
                            }
                        }
                        break;

                    case GADGET_STRING:
                        string = (struct gadget_string *) app[appnum].screen[screennum].gadget[i].gptr;

                        if ((x > string->x && x < string->x2) && (y > string->y && y < string->y2) && string->status == GADGET_ACTIVE)
                        {
                            mouse_button = 1;
                            while (mouse_button == 1)
                            {
                                SDL_Delay (100);

                                SDL_PumpEvents ();
                                buttonmask = SDL_GetMouseState (&x, &y);

                                if (! (buttonmask & SDL_BUTTON (1)))
                                {
                                    mouse_button = 0;
                                }

                                if ((x > string->x && x < string->x2) && (y > string->y && y < string->y2))
                                {
                                    printf ("gadget_event: drawing string %i selected.\n", i);

                                    if (! event_gadget_string (appnum, screennum, i))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_8 (app[appnum].tcpsock, ERROR);
                                        return;
                                    }
                                }
                                else
                                {
                                    printf ("gadget_event: drawing string %i normal.\n", i);

                                    if (! draw_gadget_string (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_NOT_SELECTED))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_8 (app[appnum].tcpsock, ERROR);
                                        return;
                                    }
                                }
                            }

                            if (! draw_gadget_string (app[appnum].screen[screennum].renderer, appnum, screennum, i, GADGET_NOT_SELECTED))
                            {
                                printf ("gadget_event: error can't draw gadget!\n");
                                send_8 (app[appnum].tcpsock, ERROR);
                                return;
                            }

                            if ((x > string->x && x < string->x2) && (y > string->y && y < string->y2))
                            {
                                /* gadget was selected */

                                printf ("gadget_event: string gadget %i selected.\n", i);

                                /* send data */
                                send_8 (app[appnum].tcpsock, OK);
                                send_8 (app[appnum].tcpsock, GADGET);
                                send_16 (app[appnum].tcpsock, i);
                                send_8 (app[appnum].tcpsock, GADGET_STRING_VALUE);
                                send_line_string (app[appnum].tcpsock, string->value);
                                send_line_string (app[appnum].tcpsock, "");         /* send empty line, to mark end of string */
                                send_8 (app[appnum].tcpsock, END);
                                return;
                            }
                        }
                        break;

                    case GADGET_BOX:
                        box = (struct gadget_box *) app[appnum].screen[screennum].gadget[i].gptr;

                        if ((x > box->x && x < box->x2) && (y > box->y && y < box->y2) && box->status == GADGET_ACTIVE)
                        {
                            mouse_button = 1;
                            while (mouse_button == 1)
                            {
                                SDL_Delay (100);

                                SDL_PumpEvents ();
                                buttonmask = SDL_GetMouseState (&x, &y);

                                if (! (buttonmask & SDL_BUTTON (1)))
                                {
                                    mouse_button = 0;
                                }
                            }

                            if ((x > box->x && x < box->x2) && (y > box->y && y < box->y2))
                            {
                                /* gadget was selected */

                                printf ("gadget_event: box gadget %i selected.\n", i);

                                /* send data */
                                send_8 (app[appnum].tcpsock, OK);
                                send_8 (app[appnum].tcpsock, GADGET);
                                send_16 (app[appnum].tcpsock, i);
                                send_8 (app[appnum].tcpsock, END);
                                return;
                            }
                        }
                        break;
                }
            }
        }
    }
}



U1 set_gadget_button (S2 appnum, S2 screennum)
{
    S2 text_len;
    Sint16 x2, y2, text_x, text_y;
    int text_height, text_width, dummy;
    U2 gadget_index = app[appnum].screen[screennum].gadget_index;
    struct gadget_button *button;

    text_len = strlen (app[appnum].screen[screennum].text);

    if (! app[appnum].screen[screennum].gadget)
    {
        printf ("set_gadget_button: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (gadget_index >= app[appnum].screen[screennum].gadgets)
    {
        printf ("set_gadget_button: error gadget index out of range!\n");
        return (FALSE);
    }

    if (! app[appnum].screen[screennum].font_ttf.font)
    {
        printf ("set_gadget_button: error no ttf font loaded!\n");
        return (FALSE);
    }

    free_gadget (appnum, screennum, gadget_index);

    app[appnum].screen[screennum].gadget[gadget_index].gptr = (struct gadget_button *) malloc (sizeof (struct gadget_button));
    if (app[appnum].screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("set_gadget_button: error can't allocate structure!\n");
        return (FALSE);
    }

    button = (struct gadget_button *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    button->text = (U1 *) malloc ((text_len + 1) * sizeof (U1));
    if (button->text == NULL)
    {
        printf ("set_gadget_button: error can't allocate text!\n");
        return (FALSE);
    }

    /* get text width and height */

    text_height = TTF_FontHeight (app[appnum].screen[screennum].font_ttf.font);
    if (TTF_SizeText (app[appnum].screen[screennum].font_ttf.font, app[appnum].screen[screennum].text, &text_width, &dummy) != 0)
    {
        printf ("set_gadget_button: error can't get text width!\n");
        return (FALSE);
    }

    /* layout button */

    x2 = app[appnum].screen[screennum].x + text_width + text_height * 2;
    y2 = app[appnum].screen[screennum].y + text_height + (text_height / 2);

    text_x = app[appnum].screen[screennum].x + text_height;
    text_y = app[appnum].screen[screennum].y + (text_height / 4);

    app[appnum].screen[screennum].gadget[gadget_index].type = GADGET_BUTTON;
    strcpy (button->text, app[appnum].screen[screennum].text);
    button->status = app[appnum].screen[screennum].gadget_status;
    button->x = app[appnum].screen[screennum].x;
    button->y = app[appnum].screen[screennum].y;
    button->x2 = x2;
    button->y2 = y2;
    button->text_x = text_x;
    button->text_y = text_y;

    if (! draw_gadget_button (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_NOT_SELECTED))
    {
        printf ("set_gadget_button: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 set_gadget_progress_bar (S2 appnum, S2 screennum)
{
    S2 text_len;
    Sint16 x2, y2, text_x, text_y;
    int text_height, text_width, dummy;
    U2 gadget_index = app[appnum].screen[screennum].gadget_index;
    struct gadget_progress_bar *progress;

    text_len = strlen (app[appnum].screen[screennum].text);

    if (! app[appnum].screen[screennum].gadget)
    {
        printf ("set_gadget_progress_bar: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (gadget_index >= app[appnum].screen[screennum].gadgets)
    {
        printf ("set_gadget_progress_bar: error gadget index out of range!\n");
        return (FALSE);
    }

    if (! app[appnum].screen[screennum].font_ttf.font)
    {
        printf ("set_gadget_progress_bar: error no ttf font loaded!\n");
        return (FALSE);
    }

    free_gadget (appnum, screennum, gadget_index);

    app[appnum].screen[screennum].gadget[gadget_index].gptr = (struct gadget_progress_bar *) malloc (sizeof (struct gadget_progress_bar));
    if (app[appnum].screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("set_gadget_progress_bar: error can't allocate structure!\n");
        return (FALSE);
    }

    progress = (struct gadget_progress_bar *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    progress->text = (U1 *) malloc ((text_len + 1) * sizeof (U1));
    if (progress->text == NULL)
    {
        printf ("set_gadget_progress_bar: error can't allocate text!\n");
        return (FALSE);
    }

    /* get text width and height */

    text_height = TTF_FontHeight (app[appnum].screen[screennum].font_ttf.font);
    if (TTF_SizeText (app[appnum].screen[screennum].font_ttf.font, app[appnum].screen[screennum].text, &text_width, &dummy) != 0)
    {
        printf ("set_gadget_progress_bar: error can't get text width!\n");
        return (FALSE);
    }

    /* layout progress bar */

    x2 = app[appnum].screen[screennum].x + text_width + text_height * 2;
    y2 = app[appnum].screen[screennum].y + text_height + (text_height / 2);

    text_x = app[appnum].screen[screennum].x + text_height;
    text_y = app[appnum].screen[screennum].y + (text_height / 4);

    app[appnum].screen[screennum].gadget[gadget_index].type = GADGET_PROGRESS_BAR;
    strcpy (progress->text, app[appnum].screen[screennum].text);
    progress->status = app[appnum].screen[screennum].gadget_status;
    progress->x = app[appnum].screen[screennum].x;
    progress->y = app[appnum].screen[screennum].y;
    progress->x2 = x2;
    progress->y2 = y2;
    progress->text_x = text_x;
    progress->text_y = text_y;
	progress->value = app[appnum].screen[screennum].gadget_progress_value;

	printf ("progress_bar value: %i\n", progress->value);
	
    if (! draw_gadget_progress_bar (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_NOT_SELECTED))
    {
        printf ("set_gadget_progress_bar: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 set_gadget_checkbox (S2 appnum, S2 screennum)
{
    S2 text_len;
    Sint16 x2, y2, text_x, text_y;
    int text_height, text_width, dummy;
    U2 gadget_index = app[appnum].screen[screennum].gadget_index;
    struct gadget_checkbox *checkbox;

    text_len = strlen (app[appnum].screen[screennum].text);

    if (! app[appnum].screen[screennum].gadget)
    {
        printf ("set_gadget_checkbox: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (gadget_index >= app[appnum].screen[screennum].gadgets)
    {
        printf ("set_gadget_checkbox: error gadget index out of range!\n");
        return (FALSE);
    }

    if (! app[appnum].screen[screennum].font_ttf.font)
    {
        printf ("set_gadget_checkbox: error no ttf font loaded!\n");
        return (FALSE);
    }

    free_gadget (appnum, screennum, gadget_index);

    app[appnum].screen[screennum].gadget[gadget_index].gptr = (struct gadget_checkbox *) malloc (sizeof (struct gadget_checkbox));
    if (app[appnum].screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("set_gadget_checkbox: error can't allocate structure!\n");
        return (FALSE);
    }

    checkbox = (struct gadget_checkbox *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    checkbox->text = (U1 *) malloc ((text_len + 1) * sizeof (U1));
    if (checkbox->text == NULL)
    {
        printf ("set_gadget_checkbox: error can't allocate text!\n");
        return (FALSE);
    }

    /* get text width and height */

    text_height = TTF_FontHeight (app[appnum].screen[screennum].font_ttf.font);
    if (TTF_SizeText (app[appnum].screen[screennum].font_ttf.font, app[appnum].screen[screennum].text, &text_width, &dummy) != 0)
    {
        printf ("set_gadget_checkbox: error can't get text width!\n");
        return (FALSE);
    }

    /* layout checkbox */

    x2 = app[appnum].screen[screennum].x + text_height + (text_height / 2);
    y2 = app[appnum].screen[screennum].y + text_height + (text_height / 2);

    text_x = x2 + (text_height + (text_height / 2)) / 2;
    text_y = app[appnum].screen[screennum].y + (text_height / 4);

    app[appnum].screen[screennum].gadget[gadget_index].type = GADGET_CHECKBOX;
    strcpy (checkbox->text, app[appnum].screen[screennum].text);
    checkbox->status = app[appnum].screen[screennum].gadget_status;
    checkbox->value = app[appnum].screen[screennum].gadget_int_value;
    checkbox->x = app[appnum].screen[screennum].x;
    checkbox->y = app[appnum].screen[screennum].y;
    checkbox->x2 = x2;
    checkbox->y2 = y2;
    checkbox->text_x = text_x;
    checkbox->text_y = text_y;
    checkbox->text_x2 = text_x + text_width;
    checkbox->text_y2 = text_y + text_height;

    if (! draw_gadget_checkbox (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_NOT_SELECTED, app[appnum].screen[screennum].gadget_int_value))
    {
        printf ("set_gadget_checkbox: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 set_gadget_cycle (S2 appnum, S2 screennum)
{
    S2 text_len, max_text_len = 0;
    Sint16 x2, y2, text_x, text_y, menu_x, menu_y, menu_x2, menu_y2, menu_height;
    int text_height, text_width, max_text_width = 0, dummy;
    U2 gadget_index = app[appnum].screen[screennum].gadget_index, i;
    struct gadget_cycle *cycle;

    if (! app[appnum].screen[screennum].gadget)
    {
        printf ("set_gadget_cycle: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (gadget_index >= app[appnum].screen[screennum].gadgets)
    {
        printf ("set_gadget_cycle: error gadget index out of range!\n");
        return (FALSE);
    }

    if (! app[appnum].screen[screennum].gadget_cycle_text)
    {
        printf ("set_gadget_cycle: error text list not allocated!\n");
        return (FALSE);
    }

    if (! app[appnum].screen[screennum].font_ttf.font)
    {
        printf ("set_gadget_cycle: error no ttf font loaded!\n");
        return (FALSE);
    }

    free_gadget (appnum, screennum, gadget_index);

    app[appnum].screen[screennum].gadget[gadget_index].gptr = (struct gadget_cycle *) malloc (sizeof (struct gadget_cycle));
    if (app[appnum].screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("set_gadget_cycle: error can't allocate structure!\n");
        return (FALSE);
    }

    cycle = (struct gadget_cycle *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    /* copy menu text list */

    cycle->text = (U1 **) malloc (app[appnum].screen[screennum].gadget_cycle_menu_entries * sizeof (app[appnum].screen[screennum].gadget_cycle_text));
    if (cycle->text == NULL)
    {
        printf ("set_gadget_cycle: error can't allocate menu text list!\n");
        return (FALSE);
    }

    for (i = 0; i < app[appnum].screen[screennum].gadget_cycle_menu_entries; i++)
    {
        if (! app[appnum].screen[screennum].gadget_cycle_text[i])
        {
            printf ("set_gadget_cycle: error no menu text!\n");
            return (FALSE);
        }

        text_len = strlen (app[appnum].screen[screennum].gadget_cycle_text[i]);

        cycle->text[i] = (U1 *) malloc ((text_len + 1) * sizeof (U1));
        if (cycle->text[i] == NULL)
        {
            printf ("set_gadget_cycle: error can't allocate menu text!\n");
            return (FALSE);
        }

        strcpy (cycle->text[i], app[appnum].screen[screennum].gadget_cycle_text[i]);

        if (TTF_SizeText (app[appnum].screen[screennum].font_ttf.font, cycle->text[i], &text_width, &dummy) != 0)
        {
            printf ("set_gadget_cycle: error can't get text width!\n");
            return (FALSE);
        }

        if (text_width > max_text_width)
        {
            max_text_width = text_width;
        }
    }

    text_height = TTF_FontHeight (app[appnum].screen[screennum].font_ttf.font);

    /* layout cycle */

    x2 = app[appnum].screen[screennum].x + max_text_width + text_height * 2 + (text_height + (text_height / 2));
    y2 = app[appnum].screen[screennum].y + text_height + (text_height / 2);

    text_x = app[appnum].screen[screennum].x + text_height;
    text_y = app[appnum].screen[screennum].y + (text_height / 4);

    menu_height = app[appnum].screen[screennum].gadget_cycle_menu_entries * (text_height + (text_height / 2));

    menu_x = app[appnum].screen[screennum].x;

    if (app[appnum].screen[screennum].y + menu_height <= app[appnum].screen[screennum].height - 1)
    {
        menu_y = app[appnum].screen[screennum].y;
    }
    else
    {
        menu_y = y2 - menu_height;
    }

    menu_x2 = x2;
    menu_y2 = menu_y + menu_height;

    app[appnum].screen[screennum].gadget[gadget_index].type = GADGET_CYCLE;
    cycle->status = app[appnum].screen[screennum].gadget_status;
    cycle->value = app[appnum].screen[screennum].gadget_int_value;
    cycle->menu_entries = app[appnum].screen[screennum].gadget_cycle_menu_entries;
    cycle->x = app[appnum].screen[screennum].x;
    cycle->y = app[appnum].screen[screennum].y;
    cycle->x2 = x2;
    cycle->y2 = y2;
    cycle->text_x = text_x;
    cycle->text_y = text_y;
    cycle->text_height = text_height;
    cycle->menu_x = menu_x;
    cycle->menu_y = menu_y;
    cycle->menu_x2 = menu_x2;
    cycle->menu_y2 = menu_y2;
    cycle->arrow_x = x2 - (text_height + (text_height / 2));
    cycle->arrow_y = app[appnum].screen[screennum].y;
    cycle->menu = FALSE;

    if (! draw_gadget_cycle (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_NOT_SELECTED, app[appnum].screen[screennum].gadget_int_value))
    {
        printf ("set_gadget_cycle: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 set_gadget_string (S2 appnum, S2 screennum)
{
    S2 text_len;
    Sint16 x, y, x2, y2, text_x, text_y, input_x, input_y;
    int text_height, text_width, char_width, dummy;
    U2 gadget_index = app[appnum].screen[screennum].gadget_index;
    struct gadget_string *string;

    if (strlen (app[appnum].screen[screennum].gadget_string_value) > app[appnum].screen[screennum].gadget_string_string_len)
    {
        printf ("set_gadget_string: error string value overflow!\n");
        return (FALSE);
    }

    text_len = strlen (app[appnum].screen[screennum].text);

    if (! app[appnum].screen[screennum].gadget)
    {
        printf ("set_gadget_string: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (gadget_index >= app[appnum].screen[screennum].gadgets)
    {
        printf ("set_gadget_string: error gadget index out of range!\n");
        return (FALSE);
    }

    if (! app[appnum].screen[screennum].font_ttf.font)
    {
        printf ("set_gadget_string: error no ttf font loaded!\n");
        return (FALSE);
    }

    free_gadget (appnum, screennum, gadget_index);

    app[appnum].screen[screennum].gadget[gadget_index].gptr = (struct gadget_string *) malloc (sizeof (struct gadget_string));
    if (app[appnum].screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("set_gadget_string: error can't allocate structure!\n");
        return (FALSE);
    }

    string = (struct gadget_string *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    /* allocate space for gadget title */

    string->text = (U1 *) malloc ((text_len + 1) * sizeof (U1));
    if (string->text == NULL)
    {
        printf ("set_gadget_string: error can't allocate text!\n");
        return (FALSE);
    }

    /* allocate space for input string */

    string->value = (U1 *) malloc ((app[appnum].screen[screennum].gadget_string_string_len + 1) * sizeof (U1));
    if (string->value == NULL)
    {
        printf ("set_gadget_string: error can't allocate value!\n");
        return (FALSE);
    }

    /* allocate space for display-buffer string */

    string->display = (U1 *) malloc ((app[appnum].screen[screennum].gadget_string_string_len + 1) * sizeof (U1));
    if (string->display == NULL)
    {
        printf ("set_gadget_string: error can't allocate display!\n");
        return (FALSE);
    }


    /* get text width and height */

    text_height = TTF_FontHeight (app[appnum].screen[screennum].font_ttf.font);
    if (TTF_SizeText (app[appnum].screen[screennum].font_ttf.font, app[appnum].screen[screennum].text, &text_width, &dummy) != 0)
    {
        printf ("set_gadget_string: error can't get text width!\n");
        return (FALSE);
    }

    /* get width of one char */

    if (TTF_SizeText (app[appnum].screen[screennum].font_ttf.font, "W", &char_width, &dummy) != 0)
    {
        printf ("set_gadget_string: error can't get char width!\n");
        return (FALSE);
    }

    /* layout string */

    text_x = app[appnum].screen[screennum].x;
    text_y = app[appnum].screen[screennum].y + (text_height / 4);

    x = text_x + text_width + text_height;
    x2 = x + ((app[appnum].screen[screennum].gadget_string_visible_len + 1) * char_width) + (char_width * 2);
    y2 = app[appnum].screen[screennum].y + text_height + (text_height / 2);

    input_x = x + char_width;
    input_y = text_y;

    app[appnum].screen[screennum].gadget[gadget_index].type = GADGET_STRING;
    strcpy (string->text, app[appnum].screen[screennum].text);
    strcpy (string->value, app[appnum].screen[screennum].gadget_string_value);
    string->status = app[appnum].screen[screennum].gadget_status;
    string->x = x;
    string->y = app[appnum].screen[screennum].y;
    string->x2 = x2;
    string->y2 = y2;
    string->text_x = text_x;
    string->text_y = text_y;
    string->text_x2 = text_x + text_width;
    string->text_y2 = text_y + text_height;

    string->input_x = input_x;
    string->input_y = input_y;
    string->string_len = app[appnum].screen[screennum].gadget_string_string_len;
    string->visible_len = app[appnum].screen[screennum].gadget_string_visible_len;

    string->cursor_width = char_width;
    string->cursor_height = text_height;
    string->cursor_pos = -1;
    string->insert_pos = -1;

    printf ("set_gadget_string: status = %i\n", string->status);

    if (! draw_gadget_string (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_NOT_SELECTED))
    {
        printf ("set_gadget_string: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 set_gadget_box (S2 appnum, S2 screennum)
{
    /* set a gadget box without a drawed box, to set a active area */

    struct gadget_box *box;
    U2 gadget_index = app[appnum].screen[screennum].gadget_index;

    if (! app[appnum].screen[screennum].gadget)
    {
        printf ("set_gadget_box: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (gadget_index >= app[appnum].screen[screennum].gadgets)
    {
        printf ("set_gadget_box: error gadget index out of range!\n");
        return (FALSE);
    }

    free_gadget (appnum, screennum, gadget_index);

    app[appnum].screen[screennum].gadget[gadget_index].gptr = (struct gadget_box *) malloc (sizeof (struct gadget_box));
    if (app[appnum].screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("set_gadget_box: error can't allocate structure!\n");
        return (FALSE);
    }

    box = (struct gadget_box *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    app[appnum].screen[screennum].gadget[gadget_index].type = GADGET_BOX;
    box->status = app[appnum].screen[screennum].gadget_status;
    box->x = app[appnum].screen[screennum].x;
    box->y = app[appnum].screen[screennum].y;
    box->x2 = app[appnum].screen[screennum].x2;
    box->y2 = app[appnum].screen[screennum].y2;

    return (TRUE);
}


U1 change_gadget_checkbox (S2 appnum, S2 screennum)
{
    /* change value and status (active / inactive) */

    U2 gadget_index = app[appnum].screen[screennum].gadget_index;
    struct gadget_checkbox *checkbox;

    if (! app[appnum].screen[screennum].gadget)
    {
        printf ("change_gadget_checkbox: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (app[appnum].screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("change_gadget_checkbox: error gadget %i not allocated!\n", gadget_index);
        return (FALSE);
    }

    if (app[appnum].screen[screennum].gadget[gadget_index].type != GADGET_CHECKBOX)
    {
        printf ("change_gadget_checkbox: error %i not a checkbox gadget!\n", gadget_index);
        return (FALSE);
    }

    checkbox = (struct gadget_checkbox *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    checkbox->value = app[appnum].screen[screennum].gadget_int_value;
    checkbox->status = app[appnum].screen[screennum].gadget_status;

    if (! draw_gadget_checkbox (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_NOT_SELECTED, app[appnum].screen[screennum].gadget_int_value))
    {
        printf ("change_gadget_checkbox: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 change_gadget_string (S2 appnum, S2 screennum)
{
    /* change value and status (active / inactive) */

    U2 gadget_index = app[appnum].screen[screennum].gadget_index;
    struct gadget_string *string;

    if (! app[appnum].screen[screennum].gadget)
    {
        printf ("change_gadget_string: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (app[appnum].screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("change_gadget_string: error gadget %i not allocated!\n", gadget_index);
        return (FALSE);
    }

    if (app[appnum].screen[screennum].gadget[gadget_index].type != GADGET_STRING)
    {
        printf ("change_gadget_string: error %i not a string gadget!\n", gadget_index);
        return (FALSE);
    }

    string = (struct gadget_string *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    if (strlen (app[appnum].screen[screennum].gadget_string_value) > string->string_len)
    {
        printf ("change_gadget_string: error string value overflow!\n");
        return (FALSE);
    }

    strcpy (string->value, app[appnum].screen[screennum].gadget_string_value);
    string->status = app[appnum].screen[screennum].gadget_status;

    if (! draw_gadget_string (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_NOT_SELECTED))
    {
        printf ("change_gadget_string: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 change_gadget_box (S2 appnum, S2 screennum)
{
    /* change value and status (active / inactive) */

    U2 gadget_index = app[appnum].screen[screennum].gadget_index;
    struct gadget_box *box;

    if (! app[appnum].screen[screennum].gadget)
    {
        printf ("change_gadget_checkbox: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (app[appnum].screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("change_gadget_checkbox: error gadget %i not allocated!\n", gadget_index);
        return (FALSE);
    }

    if (app[appnum].screen[screennum].gadget[gadget_index].type != GADGET_BOX)
    {
        printf ("change_gadget_box: error %i not a box gadget!\n", gadget_index);
        return (FALSE);
    }

    box = (struct gadget_box *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    box->status = app[appnum].screen[screennum].gadget_status;

    return (TRUE);
}


U1 change_gadget_progress_bar (S2 appnum, S2 screennum)
{
    /* change value and status (active / inactive) */

    U2 gadget_index = app[appnum].screen[screennum].gadget_index;
    struct gadget_progress_bar *progress;

    if (! app[appnum].screen[screennum].gadget)
    {
        printf ("change_gadget_progress_bar: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (app[appnum].screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("change_gadget_progress_bar: error gadget %i not allocated!\n", gadget_index);
        return (FALSE);
    }

    if (app[appnum].screen[screennum].gadget[gadget_index].type != GADGET_PROGRESS_BAR)
    {
        printf ("change_gadget_progress_bar: error %i not a progress bar gadget!\n", gadget_index);
        return (FALSE);
    }

    progress = (struct gadget_progress_bar *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

    progress->status = app[appnum].screen[screennum].gadget_status;
	progress->value = app[appnum].screen[screennum].gadget_progress_value;
	
	if (! draw_gadget_progress_bar (app[appnum].screen[screennum].renderer, appnum, screennum, gadget_index, GADGET_NOT_SELECTED))
    {
        printf ("set_gadget_progress_bar: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

void get_gadget_x2y2 (S2 appnum, S2 screennum)
{
    /* return the right bottom corner of a gadget */
    /* for layouting gadgets */

    U2 gadget_index = app[appnum].screen[screennum].gadget_index;
    Sint16 x2, y2;

    struct gadget_button *button;
    struct gadget_checkbox *checkbox;
    struct gadget_cycle *cycle;
    struct gadget_string *string;
    struct gadget_box *box;
	struct gadget_progress_bar *progress;
	
    if (! app[appnum].screen[screennum].gadget)
    {
        printf ("gadget_event: error gadget list not allocated!\n");
        send_line_string (app[appnum].tcpsock, ERROR);
        return;
    }

    switch (app[appnum].screen[screennum].gadget[gadget_index].type)
    {
        case GADGET_BUTTON:
            button = (struct gadget_button *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

            x2 = button->x2;
            y2 = button->y2;
            break;

        case GADGET_CHECKBOX:
            checkbox = (struct gadget_checkbox *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

            x2 = checkbox->text_x2;
            y2 = checkbox->y2;
            break;

        case GADGET_CYCLE:
            cycle = (struct gadget_cycle *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

            x2 = cycle->x2;
            y2 = cycle->y2;
            break;

        case GADGET_STRING:
            string = (struct gadget_string *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

            x2 = string->x2;
            y2 = string->y2;
            break;

        case GADGET_BOX:
            box = (struct gadget_box *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

            x2 = box->x2;
            y2 = box->y2;
            break;
			
		case GADGET_PROGRESS_BAR:
			progress = (struct gadget_progress_bar *) app[appnum].screen[screennum].gadget[gadget_index].gptr;

            x2 = progress->x2;
            y2 = progress->y2;
            break;
    }

    /* send data */
    send_8 (app[appnum].tcpsock, OK);
    send_16 (app[appnum].tcpsock, x2);
    send_16 (app[appnum].tcpsock, y2);
}
