/****************************************************************************
*
* Filename: draw.c
*
* Author:   Stefan Pietzonke
* Project:  flow, the gfx plugin for nano VM
*
* Purpose:  various bitmap handling functions
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

#include <global_d.h>

extern struct app app[MAXAPPS];

void set_clip_noborder (SDL_Surface *screen, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2)
{
    SDL_Rect clip;
    clip.x = x1;
    clip.y = y1;
    clip.w = x2 - x1;
    clip.h = y2 - y1;
    SDL_SetClipRect(screen, &clip);
}

/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
Uint32 getpixel (SDL_Surface *surface, Sint16 x, Sint16 y)
{
    Uint8 bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                return p[0] << 16 | p[1] << 8 | p[2];
            }
            else
            {
                return p[0] | p[1] << 8 | p[2] << 16;
            }

        case 4:
            return *(Uint32 *)p;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void clear_screen (S2 appnum, S2 screennum)
{
    Uint32 color;

    /* Setup clear color color */
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    {
        color = SDL_MapRGBA (app[appnum].screen[screennum].surface->format, app[appnum].screen[screennum].r, app[appnum].screen[screennum].g, app[appnum].screen[screennum].b, app[appnum].screen[screennum].alpha);
    }
    else
    {
        color = SDL_MapRGBA (app[appnum].screen[screennum].surface->format, app[appnum].screen[screennum].r, app[appnum].screen[screennum].g, app[appnum].screen[screennum].b, app[appnum].screen[screennum].alpha);
    }

    /* Clear the screen */
    set_clip_noborder (app[appnum].screen[screennum].surface, 0, 0, app[appnum].screen[screennum].width, app[appnum].screen[screennum].height);
    SDL_FillRect (app[appnum].screen[screennum].surface, NULL, color);
}
