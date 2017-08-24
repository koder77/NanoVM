/****************************************************************************
*
* Filename: sdl-main.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  sdl shared library for direct drawing access
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2017
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


#include "../include/host.h"
#include "../include/global_d.h"

#include "../dll_shared_libs/nanovmlib.h"

#include <SDL/SDL.h>
#include <SDL/SDL_byteorder.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>


static SDL_Surface *surf = NULL;
static TTF_Font *font = NULL;



int sdl_open_screen (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 width, height, bit;
    S8 ret = 0;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

    if (stpull_l (threadnum, pthreads, stacksize, &bit) != ERR_STACK_OK) printf ("sdl_open_screen: bit stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &height) != ERR_STACK_OK) printf ("sdl_open_screen: height stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &width) != ERR_STACK_OK) printf ("sdl_open_screen: width stack error!\n");

    surf = SDL_SetVideoMode (width, height, bit, SDL_SWSURFACE | SDL_SRCALPHA | SDL_RESIZABLE | SDL_ANYFORMAT);
    if (surf == NULL)
    {
        fprintf (stderr, "Couldn't set %i x %i x %i video mode: %s\n", width, height, bit, SDL_GetError ());
        ret = 1;    // error code
    }
    
    TTF_Init ();
    
    if (stpush_l (threadnum, pthreads, stacksize, ret) != ERR_STACK_OK) printf ("sdl_open_screen: return stack error!\n");
    return (0);
}

    
int sdl_font_ttf (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    U1 fontname[256];
    S8 size;
    
    SDL_Surface *textsurf;
    SDL_Rect dstrect;
    SDL_Color color;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    
    if (stpull_l (threadnum, pthreads, stacksize, &size) != ERR_STACK_OK) printf ("sdl_font_ttf: size stack error!\n");
    if (stpull_s (threadnum, pthreads, stacksize, fontname) != ERR_STACK_OK) printf ("sdl_font_ttf: fontname stack error!\n");
    
    font = TTF_OpenFont (fontname, size);
    if (font == NULL)
    {
        printf ("sdl_text_ttf: can't open font! %s\n", SDL_GetError ());
    }
    
    return (0);
}
    
int sdl_text_ttf (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    U1 text[256];
    S8 x, y, r, g, b;
    
    SDL_Surface *textsurf;
    SDL_Rect dstrect;
    SDL_Color color;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    if (stpull_s (threadnum, pthreads, stacksize, text) != ERR_STACK_OK) printf ("sdl_text_ttf: text stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_text_ttf: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_text_ttf: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_text_ttf: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_text_ttf: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_text_ttf: x stack error!\n");
    
    color.r = r;
    color.g = g;
    color.b = b;
    
    textsurf = TTF_RenderText_Blended (font, text, color);
    if (textsurf == NULL)
    {
        printf ("sdl_text_ttf: can't render text! %s\n", SDL_GetError ());
    }
    
    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = textsurf->w;
    dstrect.h = textsurf->h;

    SDL_BlitSurface (textsurf, NULL, surf, &dstrect);
    SDL_FreeSurface (textsurf);
    
    return (0);
}

int sdl_quit (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    SDL_Quit ();
    return (0);
}

int sdl_update (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    SDL_Flip (surf);
    return (0);
}

int sdl_mouse_event (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    Uint8 buttonmask, button = 0;
    Uint16 x, y;
    int xstate, ystate;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    SDL_PumpEvents ();
    buttonmask = SDL_GetMouseState (&xstate, &ystate);
	x = xstate; y = ystate;
    
    if (buttonmask & SDL_BUTTON (1))
    {
        button = 1;
    }

    if (buttonmask & SDL_BUTTON (2))
    {
        button = 2;
    }

    if (buttonmask & SDL_BUTTON (3))
    {
        button = 3;
    }
    
    if (stpush_l (threadnum, pthreads, stacksize, x) != ERR_STACK_OK) printf ("sdl_mouse: x stack error!\n");
    if (stpush_l (threadnum, pthreads, stacksize, y) != ERR_STACK_OK) printf ("sdl_mouse: y stack error!\n");
    if (stpush_l (threadnum, pthreads, stacksize, button) != ERR_STACK_OK) printf ("sdl_mouse: button stack error!\n");
    return (0);
}
    
    
int sdl_pixel_alpha (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 x, y, r, g, b, alpha;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;

    if (stpull_l (threadnum, pthreads, stacksize, &alpha) != ERR_STACK_OK) printf ("sdl_pixel_alpha: alpha stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_pixel_alpha: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_pixel_alpha: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_pixel_alpha: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_pixel_alpha: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_pixel_alpha: x stack error!\n");
    
    pixelRGBA (surf, x, y, r, g, b, alpha);
    return (0);
}

int sdl_line_alpha (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 x, y, x2, y2, r, g, b, alpha;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    if (stpull_l (threadnum, pthreads, stacksize, &alpha) != ERR_STACK_OK) printf ("sdl_line_alpha: alpha stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_line_alpha: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_line_alpha: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_line_alpha: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y2) != ERR_STACK_OK) printf ("sdl_line_alpha: y2 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x2) != ERR_STACK_OK) printf ("sdl_line_alpha: x2 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_line_alpha: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_line_alpha: x stack error!\n");
    
    lineRGBA (surf, x, y, x2, y2, r, g, b, alpha);
    return (0);
}

int sdl_rectangle_alpha (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 x, y, x2, y2, r, g, b, alpha;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    if (stpull_l (threadnum, pthreads, stacksize, &alpha) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: alpha stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y2) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: y2 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x2) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: x2 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: x stack error!\n");
    
    rectangleRGBA (surf, x, y, x2, y2, r, g, b, alpha);
    return (0);
}

int sdl_rectangle_fill_alpha (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 x, y, x2, y2, r, g, b, alpha;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    if (stpull_l (threadnum, pthreads, stacksize, &alpha) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: alpha stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y2) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: y2 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x2) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: x2 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: x stack error!\n");
    
    boxRGBA (surf, x, y, x2, y2, r, g, b, alpha);
    return (0);
}

int sdl_circle_alpha (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 x, y, radius, r, g, b, alpha;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    if (stpull_l (threadnum, pthreads, stacksize, &alpha) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: alpha stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &radius) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: radius stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: x stack error!\n");
    
    circleRGBA (surf, x, y, radius, r, g, b, alpha);
    return (0);
}

int sdl_circle_fill_alpha (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 x, y, radius, r, g, b, alpha;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    if (stpull_l (threadnum, pthreads, stacksize, &alpha) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: alpha stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &radius) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: radius stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: x stack error!\n");
    
    filledCircleRGBA (surf, x, y, radius, r, g, b, alpha);
    return (0);
}

int sdl_ellipse_alpha (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 x, y, xradius, yradius, r, g, b, alpha;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    if (stpull_l (threadnum, pthreads, stacksize, &alpha) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: alpha stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &yradius) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: yradius stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &xradius) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: xradius stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: x stack error!\n");
    
    ellipseRGBA (surf, x, y, xradius, yradius, r, g, b, alpha);
    return (0);
}

int sdl_ellipse_fill_alpha (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 x, y, xradius, yradius, r, g, b, alpha;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    if (stpull_l (threadnum, pthreads, stacksize, &alpha) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: alpha stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &yradius) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: yradius stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &xradius) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: xradius stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: x stack error!\n");
    
    filledEllipseRGBA (surf, x, y, xradius, yradius, r, g, b, alpha);
    return (0);
}

int sdl_pie_alpha (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 x, y, radius, startangle, endangle, r, g, b, alpha;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    if (stpull_l (threadnum, pthreads, stacksize, &alpha) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: alpha stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &endangle) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: endangle stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &startangle) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: startangle stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &radius) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: radius stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: x stack error!\n");
    
    pieRGBA (surf, x, y, radius, startangle, endangle, r, g, b, alpha);
    return (0);
}

int sdl_pie_fill_alpha (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 x, y, radius, startangle, endangle, r, g, b, alpha;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    if (stpull_l (threadnum, pthreads, stacksize, &alpha) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: alpha stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &endangle) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: endangle stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &startangle) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: startangle stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &radius) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: radius stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_rectangle_fill_alpha: x stack error!\n");
    
    filledPieRGBA (surf, x, y, radius, startangle, endangle, r, g, b, alpha);
    return (0);
}

int sdl_trigon_alpha (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 x, y, x2, y2, x3, y3, r, g, b, alpha;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    if (stpull_l (threadnum, pthreads, stacksize, &alpha) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: alpha stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y3) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: y3 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x3) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: x3 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y2) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: y2 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x2) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: x2 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: x stack error!\n");
    
    trigonRGBA (surf, x, y, x2, y2, x3, y3, r, g, b, alpha);
    return (0);
}

int sdl_trigon_fill_alpha (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    S8 x, y, x2, y2, x3, y3, r, g, b, alpha;
    
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
    
    if (stpull_l (threadnum, pthreads, stacksize, &alpha) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: alpha stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &b) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: b stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &g) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: g stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &r) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: r stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y3) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: y3 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x3) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: x3 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y2) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: y2 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x2) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: x2 stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &y) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: y stack error!\n");
    if (stpull_l (threadnum, pthreads, stacksize, &x) != ERR_STACK_OK) printf ("sdl_rectangle_alpha: x stack error!\n");
    
    filledTrigonRGBA (surf, x, y, x2, y2, x3, y3, r, g, b, alpha);
    return (0);
}


