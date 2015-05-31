/****************************************************************************
*
* Filename: gadgets.c
*
* Author:   Stefan Pietzonke
* Project:  flow, the gfx plugin for nano VM
*
* Purpose:  gadget functions
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

extern struct screen screen[MAXSCREEN];
extern TCPsocket new_tcpsock;

extern S2 mouse_x, mouse_y;
extern SDL_Surface *cursor;

void free_gadgets (void);

SDL_Surface *bmap_copy;

void init_gadgets (S2 screennum, S2 gadgets)
{
    /* allocate space for gadgets */

    S2 i;

    if (screen[screennum].gadget)
    {
        free_gadgets ();
    }

    screen[screennum].gadget = (struct gadget *) malloc (gadgets * sizeof (struct gadget));
    if (screen[screennum].gadget == NULL)
    {
        printf ("error: can't allocate gadget list!\n");
    }

    screen[screennum].gadgets = gadgets;

    for (i = 0; i < gadgets; i++)
    {
        screen[screennum].gadget[i].gptr = NULL;
        screen[screennum].gadget[i].type = 0;
    }
}

void free_gadget (S2 screennum, S2 gadget_index)
{
    struct gadget_button *button;
    struct gadget_checkbox *checkbox;
    struct gadget_cycle *cycle;
    struct gadget_string *string;

    if (screen[screennum].gadget[gadget_index].gptr)
    {
        switch (screen[screennum].gadget[gadget_index].type)
        {
            case GADGET_BUTTON:
                button = (struct gadget_button *) screen[screennum].gadget[gadget_index].gptr;

                if (button->text)
                {
                    free (button->text);
                    button->text = NULL;
                    printf ("free_gadget: %i button->text freed.\n", gadget_index);
                }
                break;

            case GADGET_CHECKBOX:
                checkbox = (struct gadget_checkbox *) screen[screennum].gadget[gadget_index].gptr;

                if (checkbox->text)
                {
                    free (checkbox->text);
                    checkbox->text = NULL;
                    printf ("free_gadget: %i checkbox->text freed.\n", gadget_index);
                }
                break;

            case GADGET_CYCLE:
                cycle = (struct gadget_cycle *) screen[screennum].gadget[gadget_index].gptr;

                if (cycle->text)
                {
                    dealloc_array_U1 (cycle->text, cycle->menu_entries);
                    cycle->text = NULL;
                    printf ("free_gadget: %i cycle->text freed.\n", gadget_index);
                }
                break;

            case GADGET_STRING:
                string = (struct gadget_string *) screen[screennum].gadget[gadget_index].gptr;

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

        free (screen[screennum].gadget[gadget_index].gptr);
        screen[screennum].gadget[gadget_index].gptr = NULL;
        screen[screennum].gadget[gadget_index].type = 0;
        printf ("free_gadget: %i gadget freed.\n", gadget_index);
   }
}

void free_gadgets (void)
{
    S2 i, j;

    for (i = 0; i < MAXSCREEN; i++)
    {
        if (screen[i].gadget)
        {
            for (j = 0; j < screen[i].gadgets; j++)
            {
                free_gadget (i, j);
            }
        }
    }
}


/* border colors */

void set_gadget_color_border_light (S2 screennum)
{
    screen[screennum].gadget_color.border_light.r = screen[screennum].r;
    screen[screennum].gadget_color.border_light.g = screen[screennum].g;
    screen[screennum].gadget_color.border_light.b = screen[screennum].b;
}

void set_gadget_color_border_shadow (S2 screennum)
{
    screen[screennum].gadget_color.border_shadow.r = screen[screennum].r;
    screen[screennum].gadget_color.border_shadow.g = screen[screennum].g;
    screen[screennum].gadget_color.border_shadow.b = screen[screennum].b;
}

/* background colors */

void set_gadget_color_backgr_light (S2 screennum)
{
    screen[screennum].gadget_color.backgr_light.r = screen[screennum].r;
    screen[screennum].gadget_color.backgr_light.g = screen[screennum].g;
    screen[screennum].gadget_color.backgr_light.b = screen[screennum].b;
}

void set_gadget_color_backgr_shadow (S2 screennum)
{
    screen[screennum].gadget_color.backgr_shadow.r = screen[screennum].r;
    screen[screennum].gadget_color.backgr_shadow.g = screen[screennum].g;
    screen[screennum].gadget_color.backgr_shadow.b = screen[screennum].b;
}

/* text colors */

void set_gadget_color_text_light (S2 screennum)
{
    screen[screennum].gadget_color.text_light.r = screen[screennum].r;
    screen[screennum].gadget_color.text_light.g = screen[screennum].g;
    screen[screennum].gadget_color.text_light.b = screen[screennum].b;
}

void set_gadget_color_text_shadow (S2 screennum)
{
    screen[screennum].gadget_color.text_shadow.r = screen[screennum].r;
    screen[screennum].gadget_color.text_shadow.g = screen[screennum].g;
    screen[screennum].gadget_color.text_shadow.b = screen[screennum].b;
}


void draw_gadget_light (S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
{
    lineRGBA (screen[screennum].bmap, x, y, x2, y, screen[screennum].gadget_color.border_light.r, screen[screennum].gadget_color.border_light.g, screen[screennum].gadget_color.border_light.b, 255);

    lineRGBA (screen[screennum].bmap, x2, y, x2, y2, screen[screennum].gadget_color.border_light.r, screen[screennum].gadget_color.border_light.g, screen[screennum].gadget_color.border_light.b, 255);

    lineRGBA (screen[screennum].bmap, x, y + 1, x, y2, screen[screennum].gadget_color.border_shadow.r, screen[screennum].gadget_color.border_shadow.g, screen[screennum].gadget_color.border_shadow.b, 255);

    lineRGBA (screen[screennum].bmap, x, y2, x2 - 1, y2, screen[screennum].gadget_color.border_shadow.r, screen[screennum].gadget_color.border_shadow.g, screen[screennum].gadget_color.border_shadow.b, 255);

    boxRGBA (screen[screennum].bmap, x + 1, y + 1, x2 - 1, y2 - 1, screen[screennum].gadget_color.backgr_light.r, screen[screennum].gadget_color.backgr_light.g, screen[screennum].gadget_color.backgr_light.b, 255);
}

void draw_gadget_shadow (S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
{
    lineRGBA (screen[screennum].bmap, x, y, x2, y, screen[screennum].gadget_color.border_shadow.r, screen[screennum].gadget_color.border_shadow.g, screen[screennum].gadget_color.border_shadow.b, 255);

    lineRGBA (screen[screennum].bmap, x2, y, x2, y2, screen[screennum].gadget_color.border_shadow.r, screen[screennum].gadget_color.border_shadow.g, screen[screennum].gadget_color.border_shadow.b, 255);

    lineRGBA (screen[screennum].bmap, x, y + 1, x, y2, screen[screennum].gadget_color.border_light.r, screen[screennum].gadget_color.border_light.g, screen[screennum].gadget_color.border_light.b, 255);

    lineRGBA (screen[screennum].bmap, x, y2, x2 - 1, y2, screen[screennum].gadget_color.border_light.r, screen[screennum].gadget_color.border_light.g, screen[screennum].gadget_color.border_light.b, 255);

    boxRGBA (screen[screennum].bmap, x + 1, y + 1, x2 - 1, y2 - 1, screen[screennum].gadget_color.backgr_shadow.r, screen[screennum].gadget_color.backgr_shadow.g, screen[screennum].gadget_color.backgr_shadow.b, 255);
}

void draw_gadget_input (S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
{
    /* outer line */

    lineRGBA (screen[screennum].bmap, x, y, x2, y, screen[screennum].gadget_color.border_light.r, screen[screennum].gadget_color.border_light.g, screen[screennum].gadget_color.border_light.b, 255);

    lineRGBA (screen[screennum].bmap, x2, y, x2, y2, screen[screennum].gadget_color.border_light.r, screen[screennum].gadget_color.border_light.g, screen[screennum].gadget_color.border_light.b, 255);

    lineRGBA (screen[screennum].bmap, x, y + 1, x, y2, screen[screennum].gadget_color.border_shadow.r, screen[screennum].gadget_color.border_shadow.g, screen[screennum].gadget_color.border_shadow.b, 255);

    lineRGBA (screen[screennum].bmap, x, y2, x2 - 1, y2, screen[screennum].gadget_color.border_shadow.r, screen[screennum].gadget_color.border_shadow.g, screen[screennum].gadget_color.border_shadow.b, 255);

    /* inner line */

    x++;
    y++;
    x2--;
    y2--;

    lineRGBA (screen[screennum].bmap, x, y, x2, y, screen[screennum].gadget_color.border_shadow.r, screen[screennum].gadget_color.border_shadow.g, screen[screennum].gadget_color.border_shadow.b, 255);

    lineRGBA (screen[screennum].bmap, x2, y, x2, y2, screen[screennum].gadget_color.border_shadow.r, screen[screennum].gadget_color.border_shadow.g, screen[screennum].gadget_color.border_shadow.b, 255);

    lineRGBA (screen[screennum].bmap, x, y + 1, x, y2, screen[screennum].gadget_color.border_light.r, screen[screennum].gadget_color.border_light.g, screen[screennum].gadget_color.border_light.b, 255);

    lineRGBA (screen[screennum].bmap, x, y2, x2 - 1, y2, screen[screennum].gadget_color.border_light.r, screen[screennum].gadget_color.border_light.g, screen[screennum].gadget_color.border_light.b, 255);

    /* clear inside */

    x++;
    y++;
    x2--;
    y2--;

    boxRGBA (screen[screennum].bmap, x, y, x2, y2, screen[screennum].gadget_color.backgr_light.r, screen[screennum].gadget_color.backgr_light.g, screen[screennum].gadget_color.backgr_light.b, 255);
}

void draw_ghost_gadget (S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
{
    Sint16 dx, dy;

    for (dy = y + 2; dy <= y2 - 2; dy += 2)
    {
        for (dx = x + 2; dx <= x2 - 2; dx += 2)
        {
            pixelRGBA (screen[screennum].bmap, dx, dy, screen[screennum].gadget_color.backgr_shadow.r, screen[screennum].gadget_color.backgr_shadow.g, screen[screennum].gadget_color.backgr_shadow.b, 255);
        }
    }
}

U1 draw_gadget_button (S2 screennum, U2 gadget_index, U1 selected)
{
    struct gadget_button *button;

    button = (struct gadget_button *) screen[screennum].gadget[gadget_index].gptr;

    if (selected == GADGET_NOT_SELECTED)
    {
        draw_gadget_light (screennum, button->x, button->y, button->x2, button->y2);

        if (! draw_text_ttf (screennum, button->text, button->text_x, button->text_y, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b))
        {
            return (FALSE);
        }
    }
    else
    {
        draw_gadget_shadow (screennum, button->x, button->y, button->x2, button->y2);

        if (! draw_text_ttf (screennum, button->text, button->text_x, button->text_y, screen[screennum].gadget_color.text_shadow.r, screen[screennum].gadget_color.text_shadow.g, screen[screennum].gadget_color.text_shadow.b))
        {
            return (FALSE);
        }
    }

    if (button->status == GADGET_NOT_ACTIVE)
    {
        draw_ghost_gadget (screennum, button->x, button->y, button->x2, button->y2);
    }

    SDL_UpdateRect (screen[screennum].bmap, button->x, button->y, button->x2 - button->x + 1, button->y2 - button->y + 1);
    return (TRUE);
}

void draw_checkmark_light (S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
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

    filledTrigonRGBA (screen[screennum].bmap, dx, dy, dx2, dy2, dx3, dy3, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b, 255);

    filledTrigonRGBA (screen[screennum].bmap, dx2, dy2, dx3, dy3, dx4, dy4, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b, 255);
}

void draw_checkmark_shadow (S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
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

    filledTrigonRGBA (screen[screennum].bmap, dx, dy, dx2, dy2, dx3, dy3, screen[screennum].gadget_color.text_shadow.r, screen[screennum].gadget_color.text_shadow.g, screen[screennum].gadget_color.text_shadow.b, 255);

    filledTrigonRGBA (screen[screennum].bmap, dx2, dy2, dx3, dy3, dx4, dy4, screen[screennum].gadget_color.text_shadow.r, screen[screennum].gadget_color.text_shadow.g, screen[screennum].gadget_color.text_shadow.b, 255);
}

U1 draw_gadget_checkbox (S2 screennum, U2 gadget_index, U1 selected, U1 value)
{
    struct gadget_checkbox *checkbox;

    checkbox = (struct gadget_checkbox *) screen[screennum].gadget[gadget_index].gptr;

    if (selected == GADGET_NOT_SELECTED)
    {
        draw_gadget_light (screennum, checkbox->x, checkbox->y, checkbox->x2, checkbox->y2);

        if (value == GADGET_CHECKBOX_TRUE)
        {
            draw_checkmark_light (screennum, checkbox->x, checkbox->y, checkbox->x2, checkbox->y2);
        }
    }
    else
    {
        draw_gadget_shadow (screennum, checkbox->x, checkbox->y, checkbox->x2, checkbox->y2);

        if (value == GADGET_CHECKBOX_TRUE)
        {
            draw_checkmark_shadow (screennum, checkbox->x, checkbox->y, checkbox->x2, checkbox->y2);
        }
    }

    if (checkbox->status == GADGET_NOT_ACTIVE)
    {
        draw_ghost_gadget (screennum, checkbox->x, checkbox->y, checkbox->x2, checkbox->y2);
    }

    boxRGBA (screen[screennum].bmap, checkbox->text_x, checkbox->text_y, checkbox->text_x2, checkbox->text_y2, screen[screennum].gadget_color.backgr_light.r, screen[screennum].gadget_color.backgr_light.g, screen[screennum].gadget_color.backgr_light.b, 255);

    if (! draw_text_ttf (screennum, checkbox->text, checkbox->text_x, checkbox->text_y, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b))
    {
        return (FALSE);
    }

    SDL_UpdateRect (screen[screennum].bmap, checkbox->x, checkbox->y, checkbox->x2 - checkbox->x + 1, checkbox->y2 - checkbox->y + 1);
    SDL_UpdateRect (screen[screennum].bmap, checkbox->text_x, checkbox->text_y, checkbox->text_x2 - checkbox->text_x + 1, checkbox->text_y2 - checkbox->text_y + 1);
    return (TRUE);
}

void draw_cycle_arrow_light (S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
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

    filledTrigonRGBA (screen[screennum].bmap, dx, dy, dx2, dy2, dx3, dy3, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b, 255);

    boxRGBA (screen[screennum].bmap, dx4, dy4, dx5, dy5, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b, 255);

    boxRGBA (screen[screennum].bmap, dx6, dy6, dx7, dy7, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b, 255);

    boxRGBA (screen[screennum].bmap, dx8, dy8, dx9, dy9, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b, 255);
}

void draw_cycle_arrow_shadow (S2 screennum, Sint16 x, Sint16 y, Sint16 x2, Sint16 y2)
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

    filledTrigonRGBA (screen[screennum].bmap, dx, dy, dx2, dy2, dx3, dy3, screen[screennum].gadget_color.text_shadow.r, screen[screennum].gadget_color.text_shadow.g, screen[screennum].gadget_color.text_shadow.b, 255);

    boxRGBA (screen[screennum].bmap, dx4, dy4, dx5, dy5, screen[screennum].gadget_color.text_shadow.r, screen[screennum].gadget_color.text_shadow.g, screen[screennum].gadget_color.text_shadow.b, 255);

    boxRGBA (screen[screennum].bmap, dx6, dy6, dx7, dy7, screen[screennum].gadget_color.text_shadow.r, screen[screennum].gadget_color.text_shadow.g, screen[screennum].gadget_color.text_shadow.b, 255);

    boxRGBA (screen[screennum].bmap, dx8, dy8, dx9, dy9, screen[screennum].gadget_color.text_shadow.r, screen[screennum].gadget_color.text_shadow.g, screen[screennum].gadget_color.text_shadow.b, 255);
}


int copy_surface (SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
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

            pixelRGBA (dst, dst_x, dst_y, r, g, b, 255);

            dst_x++;
        }

        dst_y++;
    }
    return (0);
}

U1 draw_gadget_cycle (S2 screennum, U2 gadget_index, U1 selected, S4 value)
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

    cycle = (struct gadget_cycle *) screen[screennum].gadget[gadget_index].gptr;

    switch (selected)
    {
        case GADGET_NOT_SELECTED:
            draw_gadget_light (screennum, cycle->x, cycle->y, cycle->x2, cycle->y2);

            if (! draw_text_ttf (screennum, cycle->text[value], cycle->text_x, cycle->text_y, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b))
            {
                return (FALSE);
            }

            draw_cycle_arrow_light (screennum, cycle->arrow_x, cycle->arrow_y, cycle->x2, cycle->y2);
            break;

        case GADGET_SELECTED:
            draw_gadget_shadow (screennum, cycle->x, cycle->y, cycle->x2, cycle->y2);

            if (! draw_text_ttf (screennum, cycle->text[value], cycle->text_x, cycle->text_y, screen[screennum].gadget_color.text_shadow.r, screen[screennum].gadget_color.text_shadow.g, screen[screennum].gadget_color.text_shadow.b))
            {
                return (FALSE);
            }

            draw_cycle_arrow_shadow (screennum, cycle->arrow_x, cycle->arrow_y, cycle->x2, cycle->y2);
            break;

        case GADGET_MENU_DOWN:
            if (! cycle->menu)
            {
                /* allocate backup surface, to copy the area that will be covered by the menu */

                bmap_copy = SDL_CreateRGBSurface (SDL_SWSURFACE | SDL_SRCALPHA, cycle->menu_x2 - cycle->menu_x + 1, cycle->menu_y2 - cycle->menu_y + 1, screen[screennum].video_bpp, rmask, gmask, bmask, amask);
                if (bmap_copy == NULL)
                {
                    printf ("draw_gadget_cycle: error can't allocate copy %i surface!\n", gadget_index);
                    return (FALSE);
                }

                /* Use alpha blending */
                if (SDL_SetAlpha (bmap_copy, SDL_SRCALPHA, 0) < 0)
                {
                    printf ("draw_gadget_cycle: error can't set copy alpha channel %i surface!\n", gadget_index);
                    return (FALSE);
                }

                /* make copy of menu area */

                menu_rect.x = cycle->menu_x;
                menu_rect.y = cycle->menu_y;
                menu_rect.w = cycle->menu_x2 - cycle->menu_x + 1;
                menu_rect.h = cycle->menu_y2 - cycle->menu_y + 1;

                copy_rect.x = 0;
                copy_rect.y = 0;

                if (copy_surface (screen[screennum].bmap, &menu_rect, bmap_copy, &copy_rect) < 0)
                {
                    printf ("draw_gadget_cycle: error can't blit copy %i surface!\n", gadget_index);
                    return (FALSE);
                }

                /* allocate menu surface */

                screen[BLITSCREEN].bmap = SDL_CreateRGBSurface (SDL_SWSURFACE | SDL_SRCALPHA, cycle->menu_x2 - cycle->menu_x + 1, cycle->menu_y2 - cycle->menu_y + 1, screen[screennum].video_bpp, rmask, gmask, bmask, amask);
                if (screen[BLITSCREEN].bmap == NULL)
                {
                    printf ("draw_gadget_cycle: error can't allocate menu %i surface!\n", gadget_index);
                    return (FALSE);
                }

                /* Use alpha blending */
                if (SDL_SetAlpha (screen[BLITSCREEN].bmap, SDL_SRCALPHA, 0) < 0)
                {
                    printf ("draw_gadget_cycle: error can't set menu alpha channel %i surface!\n", gadget_index);
                    return (FALSE);
                }

                screen[BLITSCREEN].gadget_color = screen[screennum].gadget_color;
                screen[BLITSCREEN].font_ttf = screen[screennum].font_ttf;
                cycle->menu = TRUE;
            }

            /* draw menu */

            draw_gadget_light (BLITSCREEN, 0, 0, cycle->menu_x2 - cycle->menu_x, cycle->menu_y2 - cycle->menu_y);

            text_x = cycle->text_x - cycle->menu_x;
            text_y = cycle->text_y - cycle->y;

            for (i = 0; i < cycle->menu_entries; i++)
            {
                if (i == value)
                {
                    boxRGBA (screen[BLITSCREEN].bmap, 1, text_y, cycle->menu_x2 - cycle->menu_x - 1, text_y + cycle->text_height, screen[screennum].gadget_color.backgr_shadow.r, screen[screennum].gadget_color.backgr_shadow.g, screen[screennum].gadget_color.backgr_shadow.b, 255);

                    if (! draw_text_ttf (BLITSCREEN, cycle->text[i], text_x, text_y, screen[screennum].gadget_color.text_shadow.r, screen[screennum].gadget_color.text_shadow.g, screen[screennum].gadget_color.text_shadow.b))
                    {
                        return (FALSE);
                    }
                }
                else
                {
                    if (! draw_text_ttf (BLITSCREEN, cycle->text[i], text_x, text_y, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b))
                    {
                        return (FALSE);
                    }
                }

                text_y += cycle->text_height + (cycle->text_height / 2);
            }

            menu_rect.x = cycle->menu_x;
            menu_rect.y = cycle->menu_y;

            if (copy_surface (screen[BLITSCREEN].bmap, NULL, screen[screennum].bmap, &menu_rect) < 0)
            {
                printf ("draw_gadget_cycle: error can't blit menu %i surface!\n", gadget_index);
                return (FALSE);
            }

            SDL_UpdateRect (screen[screennum].bmap, cycle->menu_x, cycle->menu_y, cycle->menu_x2 - cycle->menu_x + 1, cycle->menu_y2 - cycle->menu_y + 1);
            break;

        case GADGET_MENU_UP:
            /* close menu: copy backup surface */

            menu_rect.x = cycle->menu_x;
            menu_rect.y = cycle->menu_y;

            if (copy_surface (bmap_copy, NULL, screen[screennum].bmap, &menu_rect) < 0)
            {
                printf ("draw_gadget_cycle: error can't blit copy %i surface!\n", gadget_index);
                return (FALSE);
            }

            SDL_UpdateRect (screen[screennum].bmap, cycle->menu_x, cycle->menu_y, cycle->menu_x2 - cycle->menu_x + 1, cycle->menu_y2 - cycle->menu_y + 1);

            /* redraw cycle gadget */

            draw_gadget_light (screennum, cycle->x, cycle->y, cycle->x2, cycle->y2);

            if (! draw_text_ttf (screennum, cycle->text[value], cycle->text_x, cycle->text_y, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b))
            {
                return (FALSE);
            }

            draw_cycle_arrow_light (screennum, cycle->arrow_x, cycle->arrow_y, cycle->x2, cycle->y2);

            /* free bitmaps */

            if (bmap_copy)
            {
                SDL_FreeSurface (bmap_copy);
                bmap_copy = NULL;
            }
            if (screen[BLITSCREEN].bmap)
            {
                SDL_FreeSurface (screen[BLITSCREEN].bmap);
                screen[BLITSCREEN].bmap = NULL;
            }

            cycle->menu = FALSE;
            break;
    }

    if (cycle->status == GADGET_NOT_ACTIVE)
    {
        draw_ghost_gadget (screennum, cycle->x, cycle->y, cycle->x2, cycle->y2);
    }

    SDL_UpdateRect (screen[screennum].bmap, cycle->x, cycle->y, cycle->x2 - cycle->x + 1, cycle->y2 - cycle->y + 1);
    return (TRUE);
}

U1 draw_gadget_progress_bar (S2 screennum, U2 gadget_index, U1 selected)
{
    struct gadget_progress_bar *progress;
	Sint16 bar_x, bar_len;

    progress = (struct gadget_progress_bar *) screen[screennum].gadget[gadget_index].gptr;

	bar_len = (progress->x2 - progress->x) - 2;
	bar_x = progress->x + 1 + ((bar_len * progress->value) / 100);

    if (selected == GADGET_NOT_SELECTED)
    {
        draw_gadget_light (screennum, progress->x, progress->y, progress->x2, progress->y2);

		boxRGBA (screen[screennum].bmap, progress->x + 1, progress->y + 1, bar_x, progress->y2 - 1, 18, 98, 245, 255);

        if (! draw_text_ttf (screennum, progress->text, progress->text_x, progress->text_y, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b))
        {
            return (FALSE);
        }
    }
    else
    {
        draw_gadget_shadow (screennum, progress->x, progress->y, progress->x2, progress->y2);

		boxRGBA (screen[screennum].bmap, progress->x + 1, progress->y + 1, bar_x, progress->y2 - 1, 18, 98, 245, 255);

        if (! draw_text_ttf (screennum, progress->text, progress->text_x, progress->text_y, screen[screennum].gadget_color.text_shadow.r, screen[screennum].gadget_color.text_shadow.g, screen[screennum].gadget_color.text_shadow.b))
        {
            return (FALSE);
        }
    }

    if (progress->status == GADGET_NOT_ACTIVE)
    {
        draw_ghost_gadget (screennum, progress->x, progress->y, progress->x2, progress->y2);
    }

    SDL_UpdateRect (screen[screennum].bmap, progress->x, progress->y, progress->x2 - progress->x + 1, progress->y2 - progress->y + 1);
    return (TRUE);
}

U1 draw_gadget_string (S2 screennum, U2 gadget_index, U1 selected)
{
    S2 value_len;
    Sint16 cursor_x;

    struct gadget_string *string;

    string = (struct gadget_string *) screen[screennum].gadget[gadget_index].gptr;

    draw_gadget_input (screennum, string->x, string->y, string->x2, string->y2);

    boxRGBA (screen[screennum].bmap, string->text_x, string->text_y, string->text_x2, string->text_y2, screen[screennum].gadget_color.backgr_light.r, screen[screennum].gadget_color.backgr_light.g, screen[screennum].gadget_color.backgr_light.b, 255);

    if (! draw_text_ttf (screennum, string->text, string->text_x, string->text_y, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b))
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

            if (! draw_text_ttf (screennum, string->value, string->input_x, string->input_y, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b))
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

        boxRGBA (screen[screennum].bmap, cursor_x, string->input_y, cursor_x + string->cursor_width, string->input_y + string->cursor_height, screen[screennum].gadget_color.text_light.r, screen[screennum].gadget_color.text_light.g, screen[screennum].gadget_color.text_light.b, 50);
    }

    if (string->status == GADGET_NOT_ACTIVE)
    {
        draw_ghost_gadget (screennum, string->x, string->y, string->x2, string->y2);
    }

    SDL_UpdateRect (screen[screennum].bmap, string->x, string->y, string->x2 - string->x + 1, string->y2 - string->y + 1);
    SDL_UpdateRect (screen[screennum].bmap, string->text_x, string->text_y, string->text_x2 - string->text_x + 1, string->text_y2 - string->text_y + 1);
    return (TRUE);
}



U1 event_gadget_string (S2 screennum, U2 gadget_index)
{
    SDL_Event event;
    SDLKey key;

    U1 *string_buf, wait;
    S2 value_len;

	int x, y;
	
    struct gadget_string *string;
	
    string = (struct gadget_string *) screen[screennum].gadget[gadget_index].gptr;

    /* allocate string buffer */

    string_buf = (U1 *) malloc ((string->string_len + 1) * sizeof (U1));
    if (string_buf == NULL)
    {
        printf ("event_gadget_string: error can't allocate string buffer!\n");
        return (FALSE);
    }

    if (! draw_gadget_string (screennum, gadget_index, GADGET_SELECTED))
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

                            if (! draw_gadget_string (screennum, gadget_index, GADGET_SELECTED))
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

                        if (! draw_gadget_string (screennum, gadget_index, GADGET_SELECTED))
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

                            if (! draw_gadget_string (screennum, gadget_index, GADGET_SELECTED))
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

                            if (! draw_gadget_string (screennum, gadget_index, GADGET_SELECTED))
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
                        printf ("event_gadget_string: char: %i\n", event.key.keysym.unicode);

                        if (value_len < string->string_len && event.key.keysym.unicode != 0)
                        {
                                strinsertchar (string_buf, string->value, event.key.keysym.unicode, string->insert_pos);
                                strcpy (string->value, string_buf);

                                if (string->cursor_pos < string->visible_len)
                                {
                                    string->cursor_pos++;
                                }

                                string->insert_pos++;

                                if (! draw_gadget_string (screennum, gadget_index, GADGET_SELECTED))
                                {
                                    free (string_buf);
                                    return (FALSE);
                                }
                        }
                        break;

                }
                break;
				
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == 1)
                {
                    x = event.button.x;
                    y = event.button.y;
				
					if ( !((x > string->x && x < string->x2) && (y > string->y && y < string->y2)))
                    {
						/* mouseclick outside of gadget */
					
						printf ("lost focus: MOUSE clicked!\n");
						wait = FALSE;
					}
				}
				break;
        }
    }

    free (string_buf);
    return (TRUE);
}




void gadget_event (S2 screennum)
{
    SDL_Event event;
    U1 wait, cycle_menu;
    Uint8 buttonmask, mouse_button, double_click, select;
    U2 gadget, i;
    S4 value, old_value;
    int x, y;
	
	U1 resized = 0;

    struct gadget_button *button;
    struct gadget_checkbox *checkbox;
    struct gadget_cycle *cycle;
    struct gadget_string *string;
    struct gadget_box *box;

    if (! screen[screennum].gadget)
    {
        printf ("gadget_event: error gadget list not allocated!\n");
        send_16 (new_tcpsock, ERROR);
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
                send_16 (new_tcpsock, ERROR);
                return;
            }

            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == 1)
                    {
                        x = event.button.x;
                        y = event.button.y;
                        wait = FALSE;
                    }
                    break;
					
#if __ANDROID__	
				case SDL_ACTIVEEVENT:
					resized = 0;
					
					while (resized == 0)
					{
						while (SDL_PollEvent (&event))
						{
							if (event.type == SDL_VIDEORESIZE)
							{
								//If the window was iconified or restored
				
								reopen_screen (screennum, event.resize.w, event.resize.h);
								restore_screen ();
								
								resized = 1;
							}
						}
					}
					break;
#endif
            }
        }

        for (i = 0; i < screen[screennum].gadgets; i++)
        {
            if (screen[screennum].gadget[i].gptr)
            {
                switch (screen[screennum].gadget[i].type)
                {
                    case GADGET_BUTTON:
                        button = (struct gadget_button *) screen[screennum].gadget[i].gptr;

                        if ((x > button->x && x < button->x2) && (y > button->y && y < button->y2) && button->status == GADGET_ACTIVE)
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

                                if ((x > button->x && x < button->x2) && (y > button->y && y < button->y2))
                                {
                                    printf ("gadget_event: drawing button %i selected.\n", i);

                                    if (! draw_gadget_button (screennum, i, GADGET_SELECTED))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_16 (new_tcpsock, ERROR);
                                        return;
                                    }
                                }
                                else
                                {
                                    printf ("gadget_event: drawing button %i normal.\n", i);

                                    if (! draw_gadget_button (screennum, i, GADGET_NOT_SELECTED))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_16 (new_tcpsock, ERROR);
                                        return;
                                    }
                                }
                            }

                            if (! draw_gadget_button (screennum, i, GADGET_NOT_SELECTED))
                            {
                                printf ("gadget_event: error can't draw gadget!\n");
                                send_16 (new_tcpsock, ERROR);
                                return;
                            }

                            if ((x > button->x && x < button->x2) && (y > button->y && y < button->y2))
                            {
                                /* gadget was selected */

                                printf ("gadget_event: button gadget %i selected.\n", i);

                                /* send data */
                                send_16 (new_tcpsock, OK);
                                send_16 (new_tcpsock, GADGET);
                                send_16 (new_tcpsock, i);
                                send_16 (new_tcpsock, END);
                                return;
                            }
                        }
                        break;

                    case GADGET_CHECKBOX:
                        checkbox = (struct gadget_checkbox *) screen[screennum].gadget[i].gptr;

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

                                    if (! draw_gadget_checkbox (screennum, i, GADGET_SELECTED, checkbox->value))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_16 (new_tcpsock, ERROR);
                                        return;
                                    }
                                }
                                else
                                {
                                    printf ("gadget_event: drawing checkbox %i normal.\n", i);

                                    if (! draw_gadget_checkbox (screennum, i, GADGET_NOT_SELECTED, checkbox->value))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_16 (new_tcpsock, ERROR);
                                        return;
                                    }
                                }
                            }

                            if (! draw_gadget_checkbox (screennum, i, GADGET_NOT_SELECTED, checkbox->value))
                            {
                                printf ("gadget_event: error can't draw gadget!\n");
                                send_16 (new_tcpsock, ERROR);
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

                                if (! draw_gadget_checkbox (screennum, i, GADGET_NOT_SELECTED, checkbox->value))
                                {
                                    printf ("gadget_event: error can't draw gadget!\n");
                                    send_16 (new_tcpsock, ERROR);
                                    return;
                                }

                                printf ("gadget_event: checkbox gadget %i selected.\n", i);

                                /* send data */
                                send_16 (new_tcpsock, OK);
                                send_16 (new_tcpsock, GADGET);
                                send_16 (new_tcpsock, i);
                                send_16 (new_tcpsock, GADGET_INT_VALUE);
                                send_16 (new_tcpsock, checkbox->value);
                                send_16 (new_tcpsock, END);
                                return;
                            }
                        }
                        break;

                    case GADGET_CYCLE:
                        cycle = (struct gadget_cycle *) screen[screennum].gadget[i].gptr;

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

                                    if (! draw_gadget_cycle (screennum, i, GADGET_SELECTED, cycle->value))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_16 (new_tcpsock, ERROR);
                                        return;
                                    }
                                }
                                else
                                {
                                    printf ("gadget_event: drawing cycle %i normal.\n", i);

                                    if (! draw_gadget_cycle (screennum, i, GADGET_NOT_SELECTED, cycle->value))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_16 (new_tcpsock, ERROR);
                                        return;
                                    }
                                }
                            }

                            if (! draw_gadget_cycle (screennum, i, GADGET_NOT_SELECTED, cycle->value))
                            {
                                printf ("gadget_event: error can't draw gadget!\n");
                                send_16 (new_tcpsock, ERROR);
                                return;
                            }

                            if ((x > cycle->x && x < cycle->x2) && (y > cycle->y && y < cycle->y2))
                            {
                                /* gadget was selected */

                                if (! draw_gadget_cycle (screennum, i, GADGET_MENU_DOWN, cycle->value))
                                {
                                    printf ("gadget_event: error can't draw gadget!\n");
                                    send_16 (new_tcpsock, ERROR);
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

                                            if (! draw_gadget_cycle (screennum, i, GADGET_MENU_DOWN, cycle->value))
                                            {
                                                printf ("gadget_event: error can't draw gadget!\n");
                                                send_16 (new_tcpsock, ERROR);
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

                                            if (! draw_gadget_cycle (screennum, i, GADGET_MENU_DOWN, cycle->value))
                                            {
                                                printf ("gadget_event: error can't draw gadget!\n");
                                                send_16 (new_tcpsock, ERROR);
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

                                if (! draw_gadget_cycle (screennum, i, GADGET_MENU_UP, cycle->value))
                                {
                                    printf ("gadget_event: error can't draw gadget!\n");
                                    send_16 (new_tcpsock, ERROR);
                                    return;
                                }

                                printf ("gadget_event: cycle gadget %i selected.\n", i);

                                send_16 (new_tcpsock, OK);
                                send_16 (new_tcpsock, GADGET);
                                send_16 (new_tcpsock, i);
                                send_16 (new_tcpsock, GADGET_INT_VALUE);
                                send_16 (new_tcpsock, cycle->value);
                                send_16 (new_tcpsock, END);
                                return;
                            }
                        }
                        break;

                    case GADGET_STRING:
                        string = (struct gadget_string *) screen[screennum].gadget[i].gptr;

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

                                    if (! event_gadget_string (screennum, i))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_16 (new_tcpsock, ERROR);
                                        return;
                                    }
                                    else
									{
										/* gadget was selected */

										printf ("gadget_event: drawing string %i normal.\n", i);

										if (! draw_gadget_string (screennum, i, GADGET_NOT_SELECTED))
										{
											printf ("gadget_event: error can't draw gadget!\n");
											send_16 (new_tcpsock, ERROR);
											return;
										}

										/* send data */
										send_16 (new_tcpsock, OK);
										send_16 (new_tcpsock, GADGET);
										send_16 (new_tcpsock, i);
										send_16 (new_tcpsock, GADGET_STRING_VALUE);
										send_line_string (new_tcpsock, string->value);
										send_line_string (new_tcpsock, "");         /* send empty line, to mark end of string */
										send_16 (new_tcpsock, END);
										return;
									}
                                }
                                else
                                {
                                    printf ("gadget_event: drawing string %i normal.\n", i);

                                    if (! draw_gadget_string (screennum, i, GADGET_NOT_SELECTED))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        send_16 (new_tcpsock, ERROR);
                                        return;
                                    }
                                }
                            }

                            if (! draw_gadget_string (screennum, i, GADGET_NOT_SELECTED))
                            {
                                printf ("gadget_event: error can't draw gadget!\n");
                                send_16 (new_tcpsock, ERROR);
                                return;
                            }

                            if ((x > string->x && x < string->x2) && (y > string->y && y < string->y2))
                            {
                                /* gadget was selected */

                                printf ("gadget_event: string gadget %i selected.\n", i);

                                /* send data */
                                send_16 (new_tcpsock, OK);
                                send_16 (new_tcpsock, GADGET);
                                send_16 (new_tcpsock, i);
                                send_16 (new_tcpsock, GADGET_STRING_VALUE);
                                send_line_string (new_tcpsock, string->value);
                                send_line_string (new_tcpsock, "");         /* send empty line, to mark end of string */
                                send_16 (new_tcpsock, END);
                                return;
                            }
                        }
                        break;

                    case GADGET_BOX:
                        box = (struct gadget_box *) screen[screennum].gadget[i].gptr;

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
                                send_16 (new_tcpsock, OK);
                                send_16 (new_tcpsock, GADGET);
                                send_16 (new_tcpsock, i);
                                send_16 (new_tcpsock, END);
                                return;
                            }
                        }
                        break;
                }
            }
        }
    }
}



U1 set_gadget_button (S2 screennum)
{
    S2 text_len;
    Sint16 x2, y2, text_x, text_y;
    int text_height, text_width, dummy;
    U2 gadget_index = screen[screennum].gadget_index;
    struct gadget_button *button;

    text_len = strlen (screen[screennum].text);

    if (! screen[screennum].gadget)
    {
        printf ("set_gadget_button: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (gadget_index >= screen[screennum].gadgets)
    {
        printf ("set_gadget_button: error gadget index out of range!\n");
        return (FALSE);
    }

    if (! screen[screennum].font_ttf.font)
    {
        printf ("set_gadget_button: error no ttf font loaded!\n");
        return (FALSE);
    }

    free_gadget (screennum, gadget_index);

    screen[screennum].gadget[gadget_index].gptr = (struct gadget_button *) malloc (sizeof (struct gadget_button));
    if (screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("set_gadget_button: error can't allocate structure!\n");
        return (FALSE);
    }

    button = (struct gadget_button *) screen[screennum].gadget[gadget_index].gptr;

    button->text = (U1 *) malloc ((text_len + 1) * sizeof (U1));
    if (button->text == NULL)
    {
        printf ("set_gadget_button: error can't allocate text!\n");
        return (FALSE);
    }

    /* get text width and height */

    text_height = TTF_FontHeight (screen[screennum].font_ttf.font);
    if (TTF_SizeText (screen[screennum].font_ttf.font, screen[screennum].text, &text_width, &dummy) != 0)
    {
        printf ("set_gadget_button: error can't get text width!\n");
        return (FALSE);
    }

    /* layout button */

    x2 = screen[screennum].x + text_width + text_height * 2;
    y2 = screen[screennum].y + text_height + (text_height / 2);

    text_x = screen[screennum].x + text_height;
    text_y = screen[screennum].y + (text_height / 4);

    screen[screennum].gadget[gadget_index].type = GADGET_BUTTON;
    strcpy (button->text, screen[screennum].text);
    button->status = screen[screennum].gadget_status;
    button->x = screen[screennum].x;
    button->y = screen[screennum].y;
    button->x2 = x2;
    button->y2 = y2;
    button->text_x = text_x;
    button->text_y = text_y;

    if (! draw_gadget_button (screennum, gadget_index, GADGET_NOT_SELECTED))
    {
        printf ("set_gadget_button: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 set_gadget_progress_bar (S2 screennum)
{
    S2 text_len;
    Sint16 x2, y2, text_x, text_y;
    int text_height, text_width, dummy;
    U2 gadget_index = screen[screennum].gadget_index;
    struct gadget_progress_bar *progress;

    text_len = strlen (screen[screennum].text);

    if (! screen[screennum].gadget)
    {
        printf ("set_gadget_progress_bar: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (gadget_index >= screen[screennum].gadgets)
    {
        printf ("set_gadget_progress_bar: error gadget index out of range!\n");
        return (FALSE);
    }

    if (! screen[screennum].font_ttf.font)
    {
        printf ("set_gadget_progress_bar: error no ttf font loaded!\n");
        return (FALSE);
    }

    free_gadget (screennum, gadget_index);

    screen[screennum].gadget[gadget_index].gptr = (struct gadget_progress_bar *) malloc (sizeof (struct gadget_progress_bar));
    if (screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("set_gadget_progress_bar: error can't allocate structure!\n");
        return (FALSE);
    }

    progress = (struct gadget_progress_bar *) screen[screennum].gadget[gadget_index].gptr;

    progress->text = (U1 *) malloc ((text_len + 1) * sizeof (U1));
    if (progress->text == NULL)
    {
        printf ("set_gadget_progress_bar: error can't allocate text!\n");
        return (FALSE);
    }

    /* get text width and height */

    text_height = TTF_FontHeight (screen[screennum].font_ttf.font);
    if (TTF_SizeText (screen[screennum].font_ttf.font, screen[screennum].text, &text_width, &dummy) != 0)
    {
        printf ("set_gadget_progress_bar: error can't get text width!\n");
        return (FALSE);
    }

    /* layout progress bar */

    x2 = screen[screennum].x + text_width + text_height * 2;
    y2 = screen[screennum].y + text_height + (text_height / 2);

    text_x = screen[screennum].x + text_height;
    text_y = screen[screennum].y + (text_height / 4);

    screen[screennum].gadget[gadget_index].type = GADGET_PROGRESS_BAR;
    strcpy (progress->text, screen[screennum].text);
    progress->status = screen[screennum].gadget_status;
    progress->x = screen[screennum].x;
    progress->y = screen[screennum].y;
    progress->x2 = x2;
    progress->y2 = y2;
    progress->text_x = text_x;
    progress->text_y = text_y;
	progress->value = screen[screennum].gadget_progress_value;

	printf ("progress_bar value: %i\n", progress->value);
	
    if (! draw_gadget_progress_bar (screennum, gadget_index, GADGET_NOT_SELECTED))
    {
        printf ("set_gadget_progress_bar: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 set_gadget_checkbox (S2 screennum)
{
    S2 text_len;
    Sint16 x2, y2, text_x, text_y;
    int text_height, text_width, dummy;
    U2 gadget_index = screen[screennum].gadget_index;
    struct gadget_checkbox *checkbox;

    text_len = strlen (screen[screennum].text);

    if (! screen[screennum].gadget)
    {
        printf ("set_gadget_checkbox: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (gadget_index >= screen[screennum].gadgets)
    {
        printf ("set_gadget_checkbox: error gadget index out of range!\n");
        return (FALSE);
    }

    if (! screen[screennum].font_ttf.font)
    {
        printf ("set_gadget_checkbox: error no ttf font loaded!\n");
        return (FALSE);
    }

    free_gadget (screennum, gadget_index);

    screen[screennum].gadget[gadget_index].gptr = (struct gadget_checkbox *) malloc (sizeof (struct gadget_checkbox));
    if (screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("set_gadget_checkbox: error can't allocate structure!\n");
        return (FALSE);
    }

    checkbox = (struct gadget_checkbox *) screen[screennum].gadget[gadget_index].gptr;

    checkbox->text = (U1 *) malloc ((text_len + 1) * sizeof (U1));
    if (checkbox->text == NULL)
    {
        printf ("set_gadget_checkbox: error can't allocate text!\n");
        return (FALSE);
    }

    /* get text width and height */

    text_height = TTF_FontHeight (screen[screennum].font_ttf.font);
    if (TTF_SizeText (screen[screennum].font_ttf.font, screen[screennum].text, &text_width, &dummy) != 0)
    {
        printf ("set_gadget_checkbox: error can't get text width!\n");
        return (FALSE);
    }

    /* layout checkbox */

    x2 = screen[screennum].x + text_height + (text_height / 2);
    y2 = screen[screennum].y + text_height + (text_height / 2);

    text_x = x2 + (text_height + (text_height / 2)) / 2;
    text_y = screen[screennum].y + (text_height / 4);

    screen[screennum].gadget[gadget_index].type = GADGET_CHECKBOX;
    strcpy (checkbox->text, screen[screennum].text);
    checkbox->status = screen[screennum].gadget_status;
    checkbox->value = screen[screennum].gadget_int_value;
    checkbox->x = screen[screennum].x;
    checkbox->y = screen[screennum].y;
    checkbox->x2 = x2;
    checkbox->y2 = y2;
    checkbox->text_x = text_x;
    checkbox->text_y = text_y;
    checkbox->text_x2 = text_x + text_width;
    checkbox->text_y2 = text_y + text_height;

    if (! draw_gadget_checkbox (screennum, gadget_index, GADGET_NOT_SELECTED, screen[screennum].gadget_int_value))
    {
        printf ("set_gadget_checkbox: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 set_gadget_cycle (S2 screennum)
{
    S2 text_len, max_text_len = 0;
    Sint16 x2, y2, text_x, text_y, menu_x, menu_y, menu_x2, menu_y2, menu_height;
    int text_height, text_width, max_text_width = 0, dummy;
    U2 gadget_index = screen[screennum].gadget_index, i;
    struct gadget_cycle *cycle;

    if (! screen[screennum].gadget)
    {
        printf ("set_gadget_cycle: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (gadget_index >= screen[screennum].gadgets)
    {
        printf ("set_gadget_cycle: error gadget index out of range!\n");
        return (FALSE);
    }

    if (! screen[screennum].gadget_cycle_text)
    {
        printf ("set_gadget_cycle: error text list not allocated!\n");
        return (FALSE);
    }

    if (! screen[screennum].font_ttf.font)
    {
        printf ("set_gadget_cycle: error no ttf font loaded!\n");
        return (FALSE);
    }

    free_gadget (screennum, gadget_index);

    screen[screennum].gadget[gadget_index].gptr = (struct gadget_cycle *) malloc (sizeof (struct gadget_cycle));
    if (screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("set_gadget_cycle: error can't allocate structure!\n");
        return (FALSE);
    }

    cycle = (struct gadget_cycle *) screen[screennum].gadget[gadget_index].gptr;

    /* copy menu text list */

    cycle->text = (U1 **) malloc (screen[screennum].gadget_cycle_menu_entries * sizeof (U1 *));
    if (cycle->text == NULL)
    {
        printf ("set_gadget_cycle: error can't allocate menu text list!\n");
        return (FALSE);
    }

    for (i = 0; i < screen[screennum].gadget_cycle_menu_entries; i++)
    {
        if (! screen[screennum].gadget_cycle_text[i])
        {
            printf ("set_gadget_cycle: error no menu text!\n");
            return (FALSE);
        }

        text_len = strlen (screen[screennum].gadget_cycle_text[i]);

        cycle->text[i] = (U1 *) malloc ((text_len + 1) * sizeof (U1));
        if (cycle->text[i] == NULL)
        {
            printf ("set_gadget_cycle: error can't allocate menu text!\n");
            return (FALSE);
        }

        strcpy (cycle->text[i], screen[screennum].gadget_cycle_text[i]);

        if (TTF_SizeText (screen[screennum].font_ttf.font, cycle->text[i], &text_width, &dummy) != 0)
        {
            printf ("set_gadget_cycle: error can't get text width!\n");
            return (FALSE);
        }

        if (text_width > max_text_width)
        {
            max_text_width = text_width;
        }
    }

    text_height = TTF_FontHeight (screen[screennum].font_ttf.font);

    /* layout cycle */

    x2 = screen[screennum].x + max_text_width + text_height * 2 + (text_height + (text_height / 2));
    y2 = screen[screennum].y + text_height + (text_height / 2);

    text_x = screen[screennum].x + text_height;
    text_y = screen[screennum].y + (text_height / 4);

    menu_height = screen[screennum].gadget_cycle_menu_entries * (text_height + (text_height / 2));

    menu_x = screen[screennum].x;

    if (screen[screennum].y + menu_height <= screen[screennum].height - 1)
    {
        menu_y = screen[screennum].y;
    }
    else
    {
        menu_y = y2 - menu_height;
    }

    menu_x2 = x2;
    menu_y2 = menu_y + menu_height;

    screen[screennum].gadget[gadget_index].type = GADGET_CYCLE;
    cycle->status = screen[screennum].gadget_status;
    cycle->value = screen[screennum].gadget_int_value;
    cycle->menu_entries = screen[screennum].gadget_cycle_menu_entries;
    cycle->x = screen[screennum].x;
    cycle->y = screen[screennum].y;
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
    cycle->arrow_y = screen[screennum].y;
    cycle->menu = FALSE;

    if (! draw_gadget_cycle (screennum, gadget_index, GADGET_NOT_SELECTED, screen[screennum].gadget_int_value))
    {
        printf ("set_gadget_cycle: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 set_gadget_string (S2 screennum)
{
    S2 text_len;
    Sint16 x, y, x2, y2, text_x, text_y, input_x, input_y;
    int text_height, text_width, char_width, dummy;
    U2 gadget_index = screen[screennum].gadget_index;
    struct gadget_string *string;

    if (strlen (screen[screennum].gadget_string_value) > screen[screennum].gadget_string_string_len)
    {
        printf ("set_gadget_string: error string value overflow!\n");
        return (FALSE);
    }

    text_len = strlen (screen[screennum].text);

    if (! screen[screennum].gadget)
    {
        printf ("set_gadget_string: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (gadget_index >= screen[screennum].gadgets)
    {
        printf ("set_gadget_string: error gadget index out of range!\n");
        return (FALSE);
    }

    if (! screen[screennum].font_ttf.font)
    {
        printf ("set_gadget_string: error no ttf font loaded!\n");
        return (FALSE);
    }

    free_gadget (screennum, gadget_index);

    screen[screennum].gadget[gadget_index].gptr = (struct gadget_string *) malloc (sizeof (struct gadget_string));
    if (screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("set_gadget_string: error can't allocate structure!\n");
        return (FALSE);
    }

    string = (struct gadget_string *) screen[screennum].gadget[gadget_index].gptr;

    /* allocate space for gadget title */

    string->text = (U1 *) malloc ((text_len + 1) * sizeof (U1));
    if (string->text == NULL)
    {
        printf ("set_gadget_string: error can't allocate text!\n");
        return (FALSE);
    }

    /* allocate space for input string */

    string->value = (U1 *) malloc ((screen[screennum].gadget_string_string_len + 1) * sizeof (U1));
    if (string->value == NULL)
    {
        printf ("set_gadget_string: error can't allocate value!\n");
        return (FALSE);
    }

    /* allocate space for display-buffer string */

    string->display = (U1 *) malloc ((screen[screennum].gadget_string_string_len + 1) * sizeof (U1));
    if (string->display == NULL)
    {
        printf ("set_gadget_string: error can't allocate display!\n");
        return (FALSE);
    }


    /* get text width and height */

    text_height = TTF_FontHeight (screen[screennum].font_ttf.font);
    if (TTF_SizeText (screen[screennum].font_ttf.font, screen[screennum].text, &text_width, &dummy) != 0)
    {
        printf ("set_gadget_string: error can't get text width!\n");
        return (FALSE);
    }

    /* get width of one char */

    if (TTF_SizeText (screen[screennum].font_ttf.font, "W", &char_width, &dummy) != 0)
    {
        printf ("set_gadget_string: error can't get char width!\n");
        return (FALSE);
    }

    /* layout string */

    text_x = screen[screennum].x;
    text_y = screen[screennum].y + (text_height / 4);

    x = text_x + text_width + text_height;
    x2 = x + ((screen[screennum].gadget_string_visible_len + 1) * char_width) + (char_width * 2);
    y2 = screen[screennum].y + text_height + (text_height / 2);

    input_x = x + char_width;
    input_y = text_y;

    screen[screennum].gadget[gadget_index].type = GADGET_STRING;
    strcpy (string->text, screen[screennum].text);
    strcpy (string->value, screen[screennum].gadget_string_value);
    string->status = screen[screennum].gadget_status;
    string->x = x;
    string->y = screen[screennum].y;
    string->x2 = x2;
    string->y2 = y2;
    string->text_x = text_x;
    string->text_y = text_y;
    string->text_x2 = text_x + text_width;
    string->text_y2 = text_y + text_height;

    string->input_x = input_x;
    string->input_y = input_y;
    string->string_len = screen[screennum].gadget_string_string_len;
    string->visible_len = screen[screennum].gadget_string_visible_len;

    string->cursor_width = char_width;
    string->cursor_height = text_height;
    string->cursor_pos = -1;
    string->insert_pos = -1;

    printf ("set_gadget_string: status = %i\n", string->status);

    if (! draw_gadget_string (screennum, gadget_index, GADGET_NOT_SELECTED))
    {
        printf ("set_gadget_string: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 set_gadget_box (S2 screennum)
{
    /* set a gadget box without a drawed box, to set a active area */

    struct gadget_box *box;
    U2 gadget_index = screen[screennum].gadget_index;

    if (! screen[screennum].gadget)
    {
        printf ("set_gadget_box: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (gadget_index >= screen[screennum].gadgets)
    {
        printf ("set_gadget_box: error gadget index out of range!\n");
        return (FALSE);
    }

    free_gadget (screennum, gadget_index);

    screen[screennum].gadget[gadget_index].gptr = (struct gadget_box *) malloc (sizeof (struct gadget_box));
    if (screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("set_gadget_box: error can't allocate structure!\n");
        return (FALSE);
    }

    box = (struct gadget_box *) screen[screennum].gadget[gadget_index].gptr;

    screen[screennum].gadget[gadget_index].type = GADGET_BOX;
    box->status = screen[screennum].gadget_status;
    box->x = screen[screennum].x;
    box->y = screen[screennum].y;
    box->x2 = screen[screennum].x2;
    box->y2 = screen[screennum].y2;

    return (TRUE);
}


U1 change_gadget_checkbox (S2 screennum)
{
    /* change value and status (active / inactive) */

    U2 gadget_index = screen[screennum].gadget_index;
    struct gadget_checkbox *checkbox;

    if (! screen[screennum].gadget)
    {
        printf ("change_gadget_checkbox: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("change_gadget_checkbox: error gadget %i not allocated!\n", gadget_index);
        return (FALSE);
    }

    if (screen[screennum].gadget[gadget_index].type != GADGET_CHECKBOX)
    {
        printf ("change_gadget_checkbox: error %i not a checkbox gadget!\n", gadget_index);
        return (FALSE);
    }

    checkbox = (struct gadget_checkbox *) screen[screennum].gadget[gadget_index].gptr;

    checkbox->value = screen[screennum].gadget_int_value;
    checkbox->status = screen[screennum].gadget_status;

    if (! draw_gadget_checkbox (screennum, gadget_index, GADGET_NOT_SELECTED, screen[screennum].gadget_int_value))
    {
        printf ("change_gadget_checkbox: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 change_gadget_cycle (S2 screennum)
{
    /* change value and status (active / inactive) */
	
	U2 gadget_index = screen[screennum].gadget_index;
    struct gadget_cycle *cycle;

    if (! screen[screennum].gadget)
    {
        printf ("change_gadget_cycle: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("change_gadget_cycle: error gadget %i not allocated!\n", gadget_index);
        return (FALSE);
    }

    if (screen[screennum].gadget[gadget_index].type != GADGET_CYCLE)
    {
        printf ("change_gadget_cycle: error %i not a cycle gadget!\n", gadget_index);
        return (FALSE);
    }
    
    cycle = (struct gadget_cycle *) screen[screennum].gadget[gadget_index].gptr;

    cycle->value = screen[screennum].gadget_int_value;
    cycle->status = screen[screennum].gadget_status;

    if (! draw_gadget_cycle (screennum, gadget_index, GADGET_NOT_SELECTED, screen[screennum].gadget_int_value))
    {
        printf ("change_gadget_cycle: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 change_gadget_string (S2 screennum)
{
    /* change value and status (active / inactive) */

    U2 gadget_index = screen[screennum].gadget_index;
    struct gadget_string *string;

    if (! screen[screennum].gadget)
    {
        printf ("change_gadget_string: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("change_gadget_string: error gadget %i not allocated!\n", gadget_index);
        return (FALSE);
    }

    if (screen[screennum].gadget[gadget_index].type != GADGET_STRING)
    {
        printf ("change_gadget_string: error %i not a string gadget!\n", gadget_index);
        return (FALSE);
    }

    string = (struct gadget_string *) screen[screennum].gadget[gadget_index].gptr;

    if (strlen (screen[screennum].gadget_string_value) > string->string_len)
    {
        printf ("change_gadget_string: error string value overflow!\n");
        return (FALSE);
    }

    strcpy (string->value, screen[screennum].gadget_string_value);
    string->status = screen[screennum].gadget_status;

    if (! draw_gadget_string (screennum, gadget_index, GADGET_NOT_SELECTED))
    {
        printf ("change_gadget_string: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 change_gadget_box (S2 screennum)
{
    /* change value and status (active / inactive) */

    U2 gadget_index = screen[screennum].gadget_index;
    struct gadget_box *box;

    if (! screen[screennum].gadget)
    {
        printf ("change_gadget_checkbox: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("change_gadget_checkbox: error gadget %i not allocated!\n", gadget_index);
        return (FALSE);
    }

    if (screen[screennum].gadget[gadget_index].type != GADGET_BOX)
    {
        printf ("change_gadget_box: error %i not a box gadget!\n", gadget_index);
        return (FALSE);
    }

    box = (struct gadget_box *) screen[screennum].gadget[gadget_index].gptr;

    box->status = screen[screennum].gadget_status;

    return (TRUE);
}


U1 change_gadget_progress_bar (S2 screennum)
{
    /* change value and status (active / inactive) */

    U2 gadget_index = screen[screennum].gadget_index;
    struct gadget_progress_bar *progress;

    if (! screen[screennum].gadget)
    {
        printf ("change_gadget_progress_bar: error gadget list not allocated!\n");
        return (FALSE);
    }

    if (screen[screennum].gadget[gadget_index].gptr == NULL)
    {
        printf ("change_gadget_progress_bar: error gadget %i not allocated!\n", gadget_index);
        return (FALSE);
    }

    if (screen[screennum].gadget[gadget_index].type != GADGET_PROGRESS_BAR)
    {
        printf ("change_gadget_progress_bar: error %i not a progress bar gadget!\n", gadget_index);
        return (FALSE);
    }

    progress = (struct gadget_progress_bar *) screen[screennum].gadget[gadget_index].gptr;

    progress->status = screen[screennum].gadget_status;
	progress->value = screen[screennum].gadget_progress_value;
	
	if (! draw_gadget_progress_bar (screennum, gadget_index, GADGET_NOT_SELECTED))
    {
        printf ("set_gadget_progress_bar: error can't draw gadget!\n");
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

void get_gadget_x2y2 (S2 screennum)
{
    /* return the right bottom corner of a gadget */
    /* for layouting gadgets */

    U2 gadget_index = screen[screennum].gadget_index;
    Sint16 x2, y2;

    struct gadget_button *button;
    struct gadget_checkbox *checkbox;
    struct gadget_cycle *cycle;
    struct gadget_string *string;
    struct gadget_box *box;
	struct gadget_progress_bar *progress;
	
    if (! screen[screennum].gadget)
    {
        printf ("gadget_event: error gadget list not allocated!\n");
        send_line_string (new_tcpsock, ERROR);
        return;
    }

    switch (screen[screennum].gadget[gadget_index].type)
    {
        case GADGET_BUTTON:
            button = (struct gadget_button *) screen[screennum].gadget[gadget_index].gptr;

            x2 = button->x2;
            y2 = button->y2;
            break;

        case GADGET_CHECKBOX:
            checkbox = (struct gadget_checkbox *) screen[screennum].gadget[gadget_index].gptr;

            x2 = checkbox->text_x2;
            y2 = checkbox->y2;
            break;

        case GADGET_CYCLE:
            cycle = (struct gadget_cycle *) screen[screennum].gadget[gadget_index].gptr;

            x2 = cycle->x2;
            y2 = cycle->y2;
            break;

        case GADGET_STRING:
            string = (struct gadget_string *) screen[screennum].gadget[gadget_index].gptr;

            x2 = string->x2;
            y2 = string->y2;
            break;

        case GADGET_BOX:
            box = (struct gadget_box *) screen[screennum].gadget[gadget_index].gptr;

            x2 = box->x2;
            y2 = box->y2;
            break;
			
		case GADGET_PROGRESS_BAR:
			progress = (struct gadget_progress_bar *) screen[screennum].gadget[gadget_index].gptr;

            x2 = progress->x2;
            y2 = progress->y2;
            break;
    }

    /* send data */
    send_16 (new_tcpsock, OK);
    send_16 (new_tcpsock, x2);
    send_16 (new_tcpsock, y2);
}
