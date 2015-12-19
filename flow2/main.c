/****************************************************************************
*
* Filename: main.c
*
* Author:   Stefan Pietzonke
* Project:  flow, the gfx plugin for nano VM
*
* Purpose:  main functions
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

#if __ANDROID__

#include <android/log.h>

#define  LOG_TAG    "flow-error"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#endif

#include "global_d.h"
#include "socket.h"

/* SDL stuff */

#if SOCKETS_NATIVE
	U1 ip[256], addr[256];
	S2 new_tcpsock;
#else
	U1 ip[256];
	IPaddress ipadd;
	TCPsocket tcpsock, new_tcpsock;
#endif
	

/* Raspberry Pi struct */
struct rpi rpi;


	
struct app app[MAXAPPS];
S4 app_index = -1;

void set_simple_cursor (void)
{
	Uint8 mask[] = {0x00, 0x80, 0x01, 0xc0, 0x03, 0xE0, 0x07, 0xF0, 0x0F, 0xF8, 0x1F, 0xFC, 0x3F, 0xFE, 0x7F, 0xFF };
    Uint8 data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    SDL_Cursor* cursor = SDL_CreateCursor(data, mask, 16, 8, 8, 0);
    SDL_SetCursor (cursor);
}


/* Android start menu */
U1 menu[10][80];
U1 nanovm_path[256];



U1 load_font_bitmap (S2 appnum, S2 screennum)
{
    FILE *font;
    U2 filesize;
    U1 *fontname;
    S2 fontname_len;
	
	U1 android_path[256];
	
	strcpy (android_path, ANDROID_SDCARD);
	strcat (android_path, "nanovm/fonts/");

    /* check for font path env variable */

    
#if __ANDROID__
		fontname_len = strlen (android_path);
        fontname_len = fontname_len + strlen (app[appnum].screen[screennum].fontname) + 1;

        /* allocate buffer for fontname */

        fontname = (U1 *) malloc (fontname_len * sizeof (U1));
        if (fontname == NULL)
        {
            printf ("load_font_bitmap: can't allocate %i bytes for fontname!\n", fontname_len);
            return (FALSE);
        }

        strcpy (fontname, android_path);
        strcat (fontname, app[appnum].screen[screennum].fontname);
		
#else
	if (getenv (FONT_DIR_SB) != NULL)
    {	
        /* env variable set */

        fontname_len = strlen (getenv (FONT_DIR_SB));
        fontname_len = fontname_len + strlen (app[appnum].screen[screennum].fontname) + 1;

        /* allocate buffer for fontname */

        fontname = (U1 *) malloc (fontname_len * sizeof (U1));
        if (fontname == NULL)
        {
            printf ("load_font_bitmap: can't allocate %i bytes for fontname!\n", fontname_len);
            return (FALSE);
        }

        strcpy (fontname, getenv (FONT_DIR_SB));
        strcat (fontname, app[appnum].screen[screennum].fontname);
    }
    else
    {
        /* env variable not set */

        printf ("load_font_bitmap: env variable NANOGFXFONT not set!\n");

        fontname_len = strlen (app[appnum].screen[screennum].fontname) + 1;

        /* allocate buffer for fontname */

        fontname = (U1 *) malloc (fontname_len * sizeof (U1));
        if (fontname == NULL)
        {
            printf ("load_font_bitmap: can't allocate %i bytes for fontname!\n", fontname_len);
            return (FALSE);
        }

        strcpy (fontname, app[appnum].screen[screennum].fontname);
    }
#endif

    if (! (font = fopen (fontname, "rb")))
    {
        /* can't open font */
        printf ("load_font_bitmap: can't open font %s !\n", app[appnum].screen[screennum].fontname);
        printf ("load_font_bitmap: %s\n\n", fontname);
        return (FALSE);
    }

     /* get filesize */

    fseek (font, (long) NULL, SEEK_END);
    filesize = ftell (font);
    fseek (font, (long) NULL, SEEK_SET);

    /* allocate buffer for font data */

    if (app[appnum].screen[screennum].font_bitmap.buf != NULL)
    {
        /* free old font data */

        free (app[appnum].screen[screennum].font_bitmap.buf);
    }

    app[appnum].screen[screennum].font_bitmap.buf = (U1 *) malloc (filesize * sizeof (U1));
    if (app[appnum].screen[screennum].font_bitmap.buf == NULL)
    {
        printf ("load_font_bitmap: can't allocate %i bytes for font!\n", filesize);
        return (FALSE);
    }

    printf ("font size: %i\n", filesize);

    if (fread (app[appnum].screen[screennum].font_bitmap.buf, sizeof (U1), filesize, font) != filesize)
    {
        printf ("load_font_bitmap: can't load font %s !\n", fontname);
        return (FALSE);
    }

    gfxPrimitivesSetFont (app[appnum].screen[screennum].font_bitmap.buf, app[appnum].screen[screennum].font_bitmap.width, app[appnum].screen[screennum].font_bitmap.height);
    fclose (font);
    free (fontname);

    return (TRUE);
}

U1 load_font_ttf (S2 appnum, S2 screennum)
{
    U1 *fontname;
    S2 fontname_len;
	
	U1 android_path[256];
	
	strcpy (android_path, ANDROID_SDCARD);
	strcat (android_path, "nanovm/fonts/");

    if (app[appnum].screen[screennum].font_ttf.font != NULL)
    {
        /* close old font */

        TTF_CloseFont (app[appnum].screen[screennum].font_ttf.font);
    }

#if __ANDROID__
		fontname_len = strlen (android_path);
        fontname_len = fontname_len + strlen (app[appnum].screen[screennum].fontname) + 1;

        /* allocate buffer for fontname */

        fontname = (U1 *) malloc (fontname_len * sizeof (U1));
        if (fontname == NULL)
        {
            printf ("load_font_bitmap: can't allocate %i bytes for fontname!\n", fontname_len);
            return (FALSE);
        }

        strcpy (fontname, android_path);
        strcat (fontname, app[appnum].screen[screennum].fontname);
		
#else
	if (getenv (FONT_DIR_SB) != NULL)
	{	
        /* env variable set */

        fontname_len = strlen (getenv (FONT_DIR_SB));
        fontname_len = fontname_len + strlen (app[appnum].screen[screennum].fontname) + 1;

        /* allocate buffer for fontname */

        fontname = (U1 *) malloc (fontname_len * sizeof (U1));
        if (fontname == NULL)
        {
            printf ("load_font_bitmap: can't allocate %i bytes for fontname!\n", fontname_len);
            return (FALSE);
        }

        strcpy (fontname, getenv (FONT_DIR_SB));
        strcat (fontname, app[appnum].screen[screennum].fontname);
    }
    else
    {
        /* env variable not set */

        printf ("load_font_bitmap: env variable NANOGFXFONT not set!\n");

        fontname_len = strlen (app[appnum].screen[screennum].fontname) + 1;

        /* allocate buffer for fontname */

        fontname = (U1 *) malloc (fontname_len * sizeof (U1));
        if (fontname == NULL)
        {
            printf ("load_font_bitmap: can't allocate %i bytes for fontname!\n", fontname_len);
            return (FALSE);
        }

        strcpy (fontname, app[appnum].screen[screennum].fontname);
    }
#endif
    
    app[appnum].screen[screennum].font_ttf.font = TTF_OpenFont (fontname, app[appnum].screen[screennum].font_ttf.size);
    if (app[appnum].screen[screennum].font_ttf.font == NULL)
    {
        printf ("load_font_ttf: can't load font %s size %i !\n", fontname, app[appnum].screen[screennum].font_ttf.size);
        free (fontname);
        return (FALSE);
    }
    else
    {
        free (fontname);
        return (TRUE);
    }
}


U1 set_ttf_style (S2 appnum, S2 screennum)
{
    U1 style = TTF_STYLE_NORMAL;

    if (app[appnum].screen[screennum].font_ttf.font == NULL)
    {
        printf ("set_ttf_style: can't set style. No font opened!\n");
        return (FALSE);
    }

    if (! app[appnum].screen[screennum].font_ttf.style_normal)
    {
        if (app[appnum].screen[screennum].font_ttf.style_bold)
        {
            style = style | TTF_STYLE_BOLD;
        }

        if (app[appnum].screen[screennum].font_ttf.style_italic)
        {
            style = style | TTF_STYLE_ITALIC;
        }

        if (app[appnum].screen[screennum].font_ttf.style_underline)
        {
            style = style | TTF_STYLE_UNDERLINE;
        }
    }

    TTF_SetFontStyle (app[appnum].screen[screennum].font_ttf.font, style);

    return (TRUE);
}

U1 draw_text_ttf (SDL_Surface *surface, S2 appnum, S2 screennum, U1 *textstr, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Surface *text;
    SDL_Rect dstrect;
    SDL_Color color;

    color.r = r;
    color.g = g;
    color.b = b;

    if (app[appnum].screen[screennum].font_ttf.font == NULL)
    {
        printf ("draw_text_ttf: can't draw text. No font opened!\n");
        return (FALSE);
    }

    if (! set_ttf_style (appnum, screennum))
    {
        return (FALSE);
    }

    text = TTF_RenderText_Blended (app[appnum].screen[screennum].font_ttf.font, textstr, color);
    if (text == NULL)
    {
        printf ("draw_text_ttf: can't render text! %s\n", SDL_GetError ());
        return (FALSE);
    }

    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = text->w;
    dstrect.h = text->h;

    SDL_BlitSurface (text, NULL, surface, &dstrect);
    SDL_FreeSurface (text);

    return (TRUE);
}

U1 load_picture (S2 appnum, S2 screennum)
{
    SDL_Surface *picture;
    SDL_Rect dstrect;

    dstrect.x = app[appnum].screen[screennum].x;
    dstrect.y = app[appnum].screen[screennum].y;
    dstrect.w = 0;
    dstrect.h = 0;

    picture = IMG_Load (app[appnum].screen[screennum].picture_name);
    if (picture == NULL)
    {
        printf ("load_picture: can't load picture %s !\n", app[appnum].screen[screennum].picture_name);
        return (FALSE);
    }

    SDL_BlitSurface (picture, NULL, app[appnum].screen[screennum].surface, &dstrect);
    SDL_FreeSurface (picture);

    return (TRUE);
}

U1 save_picture (S2 appnum, S2 screennum)
{
	SDL_Surface *output_surf;
	Uint32 rmask, gmask, bmask, amask;

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
	#else
	rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
	#endif
	
	update_screen (appnum, screennum);
	
	/* Creating the output surface to save */
//	output_surf = SDL_CreateRGBSurface (screen[screennum].bmap->flags, screen[screennum].bmap->w, screen[screennum].bmap->h, screen[screennum].bmap->format->BitsPerPixel, rmask, gmask, bmask, amask);
	output_surf = SDL_CreateRGBSurface (app[appnum].screen[screennum].surface->flags, app[appnum].screen[screennum].surface->w, app[appnum].screen[screennum].surface->h, app[appnum].screen[screennum].surface->format->BitsPerPixel, 0, 0, 0, 0);
	copy_surface (app[appnum].screen[screennum].surface, NULL, output_surf, NULL);

	if (SDL_SaveBMP (output_surf, app[appnum].screen[screennum].picture_name) < 0)
	{
		SDL_FreeSurface (output_surf);
		return (FALSE);
	}
	else
	{
		SDL_FreeSurface (output_surf);
		return (TRUE);
	}
}

void get_pixel_color (S2 appnum, S2 screennum)
{
    Uint32 pixel;
    Uint8 r, g, b;

    if (SDL_LockSurface (app[appnum].screen[screennum].surface) < 0)
    {
        printf ("get_pixelcolor: can't lock surface!\n");
    }

    pixel = getpixel (app[appnum].screen[screennum].surface, app[appnum].screen[screennum].x, app[appnum].screen[screennum].y);
	if (pixel == 0)
	{
		printf ("get_pixel_color: ERROR GETTING PIXEL COLOR!\n");
	}
	
    SDL_UnlockSurface (app[appnum].screen[screennum].surface);
    SDL_GetRGB (pixel, app[appnum].screen[screennum].surface->format, &r, &g, &b);

    /* send data */
    send_8 (app[appnum].tcpsock, OK);
    send_16 (app[appnum].tcpsock, r);
    send_16 (app[appnum].tcpsock, g);
    send_16 (app[appnum].tcpsock, b);
	
	printf ("get_pixel_color: %i, %i, %i\n", r, g, b);
}

void get_mouse (S2 appnum, S2 screennum)
{
    Uint8 buttonmask, button = 0, window_selected = 0;
    Uint16 x, y;
	int xstate, ystate;
	SDL_Event *event;
	
	Uint32 window_id = SDL_GetWindowID (app[appnum].screen[screennum].window);
	
    /* check mouse status */

    printf ("get_mouse: SDL_GetMouseState ... ");

    SDL_PumpEvents ();
	if (event->type == SDL_WINDOWEVENT)
	{
		if (event->window.event == SDL_WINDOWEVENT_ENTER)
		{
			if (window_id == event->window.windowID)
			{
				window_selected = 1;
			}
		}
	}
			
			
    buttonmask = SDL_GetMouseState (&xstate, &ystate);
	x = xstate; y = ystate;
	
    printf (" ok\n");

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

    /* send data */
    send_8 (app[appnum].tcpsock, OK);
    send_16 (app[appnum].tcpsock, x);
    send_16 (app[appnum].tcpsock, y);
    send_16 (app[appnum].tcpsock, button);
}

U1 open_new_screen (S2 appnum, S2 screennum)
{
	SDL_Window *window;
	SDL_Surface *surface;
	SDL_Renderer *renderer;
	
	/* Create window and renderer for given surface */
	window = SDL_CreateWindow(app[appnum].screen[screennum].title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, app[appnum].screen[screennum].width, app[appnum].screen[screennum].height, SDL_WINDOW_SHOWN);
	if(!window)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window creation fail : %s\n",SDL_GetError());
		
		#if __ANDROID__
			LOGE("SDL create window error!\n");
		#endif
		
		return (FALSE);
	}	
	surface = SDL_GetWindowSurface(window);
	renderer = SDL_CreateSoftwareRenderer(surface);
	if(!renderer)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Render creation for surface fail : %s\n",SDL_GetError());
		
		#if __ANDROID__
			LOGE("SDL renderer error!\n");
		#endif
		
		return (FALSE);
	}
	SDL_RenderSetViewport(renderer, NULL);
	
	/* save window/screen pointers */
	app[appnum].screen[screennum].window = window;
	app[appnum].screen[screennum].surface = surface;
	app[appnum].screen[screennum].renderer = renderer;
	
	return (TRUE);
}

void update_screen (S2 appnum, S2 screennum)
{
	SDL_UpdateWindowSurface (app[appnum].screen[screennum].window);
}

void *handle_client (S4 number)
{
	S2 command;
	U1 arg[256];
	S2 i, endconn = FALSE;
	
#if SOCKETS_NATIVE
	S2 sock;
#else
	TCPsocket sock;
#endif
	
	S2 buf2;
	S4 screennum = 0;
	
	U2 ret;
	
	sock = app[number].tcpsock;
	
	SDL_Event event;
	
	printf ("handle_client: %i\n", number);
	
	while (! endconn)
	{
wait_command:
		/* SDL event handling stuff... */
		
		while (SDL_PollEvent (&event))
		{
			#if __ANDROID__
				LOGD("polling event...\n");
			#endif
			
			if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_HIDDEN)
				{
					// copy_screen ();
				}
			
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					// If the window was iconified or restored
				
					// reopen_screen (0, event.resize.w, event.resize.h);
					// restore_screen ();
				
					#if __ANDROID__
						LOGD("screen resized\n");
					#endif
				}
			}
		}
		
		if (! read_16 (sock, &command))
        {
            printf ("error: can't read command!\n");
			
#if __ANDROID__
			LOGE("read-command: error!\n");
#endif
			
            goto wait_command;
        }
		
		
		
#if __ANDROID__
		LOGD("read-command: %i\n", command);
#endif
		if (command >= END && command <= SAVE_PICTURE)
		{
			printf ("command: %i\n", command);
			
			switch (command)
			{
				case END:
					send_8 (sock, OK);
					
					close_app_screens (number);
					free_app_handle (number);
					endconn = TRUE;
					break;
				
				case SHUTDOWN:
					send_8 (sock, OK);
					endconn = TRUE;
					break;

				case OPEN_SCREEN:
					#if ! (__ANDROID__ || START_MENU)
					
					if (! open_new_screen (number, screennum))
					{
						send_8 (sock, ERROR);
						free_app_handle (number);
						endconn = TRUE;
					}
					else
					{
						send_8 (sock, OK);
					}
					
					#else
					
					send_8 (sock, OK);		/* screen on Android already opened! */
					
					#endif
					
					break;

				case CLOSE_SCREEN:
					SDL_DestroyWindow (app[number].screen[screennum].window);
					send_8 (sock, OK);
					break;

				case CLEAR_SCREEN:
					clear_screen (number, screennum);
					send_8 (sock, OK);
					break;

				case UPDATE_SCREEN:
					update_screen (number, screennum);
					send_8 (sock, OK);
					break;

				case COLOR:
					break;
					
				case PIXEL:
					printf ("pixel\n");
					pixelRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;

				case LINE:
					lineRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].x2, app[number].screen[screennum].y2, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case RECTANGLE:
					rectangleRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].x2, app[number].screen[screennum].y2, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case RECTANGLE_FILL:
					boxRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].x2, app[number].screen[screennum].y2, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case CIRCLE:
					circleRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].radius, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case CIRCLE_FILL:
					filledCircleRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].radius, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case ELLIPSE:
					ellipseRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].xradius, app[number].screen[screennum].yradius, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case ELLIPSE_FILL:
					filledEllipseRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].xradius, app[number].screen[screennum].yradius, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case PIE:
					pieRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].radius, app[number].screen[screennum].startangle, app[number].screen[screennum].endangle, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case PIE_FILL:
					filledPieRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].radius, app[number].screen[screennum].startangle, app[number].screen[screennum].endangle, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
				
				case TRIGON:
					trigonRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].x2, app[number].screen[screennum].y2, app[number].screen[screennum].x3, app[number].screen[screennum].y3, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case TRIGON_FILL:
					filledTrigonRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].x2, app[number].screen[screennum].y2, app[number].screen[screennum].x3, app[number].screen[screennum].y3, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case POLYGON:
					if (app[number].screen[screennum].vx == NULL || app[number].screen[screennum].vy == NULL)
					{
						/* error: coordinate arrays not allocated */
						send_8 (sock, ERROR);
					}
					
					polygonRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].vx, app[number].screen[screennum].vy, app[number].screen[screennum].vectors, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case POLYGON_FILL:
					if (app[number].screen[screennum].vx == NULL || app[number].screen[screennum].vy == NULL)
					{
						/* error: coordinate arrays not allocated */
						send_8 (sock, ERROR);
					}
					
					filledPolygonRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].vx, app[number].screen[screennum].vy, app[number].screen[screennum].vectors, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case BEZIER:
					if (app[number].screen[screennum].vx == NULL || app[number].screen[screennum].vy == NULL)
					{
						/* error: coordinate arrays not allocated */
						send_8 (sock, ERROR);
					}
					
					bezierRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].vx, app[number].screen[screennum].vy, app[number].screen[screennum].vectors, app[number].screen[screennum].steps, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case TEXT_BMAP:
					stringRGBA (app[number].screen[screennum].renderer, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].text, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b, app[number].screen[screennum].alpha);
					send_8 (sock, OK);
					break;
					
				case LOADFONT_BMAP:
					if (load_font_bitmap (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                        send_8 (app[number].tcpsock, ERROR);
                    }
                    break;
					
				case TEXT_TTF:
					if (draw_text_ttf (app[number].screen[screennum].surface, number, screennum, app[number].screen[screennum].text, app[number].screen[screennum].x, app[number].screen[screennum].y, app[number].screen[screennum].r, app[number].screen[screennum].g, app[number].screen[screennum].b))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                       send_8 (app[number].tcpsock, ERROR);
                    }
                    break;
					
				case LOADFONT_TTF:
					if (load_font_ttf (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                        send_8 (app[number].tcpsock, ERROR);
                    }
                    break;
					
				case LOAD_PICTURE:
					if (load_picture (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                        send_8 (app[number].tcpsock, ERROR);
                    }
                    break;
					
				case GETPIXEL:
					get_pixel_color (number, screennum);
                    break;
					
				case GETMOUSE:
					get_mouse (number, screennum);
					break;
				
				
					
					
				case GADGET_COLOR_BORDER_LIGHT:
                    set_gadget_color_border_light (number, screennum);
                    break;

                case GADGET_COLOR_BORDER_SHADOW:
                    set_gadget_color_border_shadow (number, screennum);
                    break;

                case GADGET_COLOR_BACKGR_LIGHT:
                    set_gadget_color_backgr_light (number, screennum);
                    break;

                case GADGET_COLOR_BACKGR_SHADOW:
                    set_gadget_color_backgr_shadow (number, screennum);
                    break;

                case GADGET_COLOR_TEXT_LIGHT:
                    set_gadget_color_text_light (number, screennum);
                    break;

                case GADGET_COLOR_TEXT_SHADOW:
                    set_gadget_color_text_shadow (number, screennum);
                    break;

                case GADGET_BUTTON:
                    if (set_gadget_button (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                       send_8 (app[number].tcpsock, ERROR);
                    }
                    break;

                case GADGET_CHECKBOX:
                    if (set_gadget_checkbox (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                       send_8 (app[number].tcpsock, ERROR);
                    }
                    break;

                case GADGET_CYCLE:
                    if (set_gadget_cycle (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                       send_8 (app[number].tcpsock, ERROR);
                    }
                    break;

                case GADGET_STRING:
                    if (set_gadget_string (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                       send_8 (app[number].tcpsock, ERROR);
                    }
                    break;

                case GADGET_BOX:
                    if (set_gadget_box (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                       send_8 (app[number].tcpsock, ERROR);
                    }
                    break;
					
				case GADGET_PROGRESS_BAR:
					if (set_gadget_progress_bar (number, screennum))
					{
						 send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                       send_8 (app[number].tcpsock, ERROR);
                    }
                    break;
					
                case GADGET_EVENT:
                    gadget_event (number, screennum);
                    break;

                case GADGET_GET_X2Y2:
                    get_gadget_x2y2 (number, screennum);
                    break;

                case GADGET_CHECKBOX_CHANGE:
                    if (change_gadget_checkbox (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                       send_8 (app[number].tcpsock, ERROR);
                    }
                    break;

                case GADGET_STRING_CHANGE:
                    if (change_gadget_string (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                       send_8 (app[number].tcpsock, ERROR);
                    }
                    break;

                case GADGET_BOX_CHANGE:
                    if (change_gadget_box (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                       send_8 (app[number].tcpsock, ERROR);
                    }
                    break;
					
				case GADGET_PROGRESS_BAR_CHANGE:
					if (change_gadget_progress_bar (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                       send_8 (app[number].tcpsock, ERROR);
                    }
                    break;
					
				
				case RPI_GPIO_START:
					send_8 (app[number].tcpsock, rpi_gpio_start ());
                    break;
					
				case RPI_GPIO_MODE:
					rpi_gpio_mode (rpi.pin, rpi.value);
					break;
					
				case RPI_GPIO_READ:
					ret = rpi_gpio_read ();
					send_16 (app[number].tcpsock, ret);
					break;
					
				case RPI_GPIO_WRITE:
					rpi_gpio_write (rpi.value);
					break;
					
				case SAVE_PICTURE:
					if (save_picture (number, screennum))
                    {
                        send_8 (app[number].tcpsock, OK);
                    }
                    else
                    {
                        send_8 (app[number].tcpsock, ERROR);
                    }
                    break;

			}
		}
				
		printf ("reading data: %i\n", command);
		
		if (command >= SCREENNUM && command < TEXT)
		{
			if (! read_16 (sock, &buf2))
			{
				printf ("error: can't read data!\n");
				break;
			}
			
			
			switch (command)
			{
		
				case SCREENNUM:
					screennum = buf2;

					if (screennum < 0 || screennum > MAXSCREEN - 1)
					{
						screennum = 0;
					}
						
					break;

				case WIDTH:
					app[number].screen[screennum].width = buf2;
					break;

				case HEIGHT:
					app[number].screen[screennum].height = buf2;
					break;

				case BIT:
					app[number].screen[screennum].video_bpp = buf2;
					break;

				case X:
					app[number].screen[screennum].x = buf2;
					break;

				case Y:
					app[number].screen[screennum].y = buf2;
					break;

				case X2:
					app[number].screen[screennum].x2 = buf2;
					break;

				case Y2:
					app[number].screen[screennum].y2 = buf2;
					break;

				case X3:
					app[number].screen[screennum].x3 = buf2;
					break;

				case Y3:
					app[number].screen[screennum].y3 = buf2;
					break;
					
				case VECTORS:
					break;
					
				case STEPS:
					break;
					
				case RADIUS:
					app[number].screen[screennum].radius = buf2;
					break;
					
				case XRADIUS:
					app[number].screen[screennum].xradius = buf2;
					break;
					
				case YRADIUS:
					app[number].screen[screennum].yradius = buf2;
					break;
					
				case STARTANGLE:
					app[number].screen[screennum].startangle = buf2;
					break;
					
				case ENDANGLE:
					app[number].screen[screennum].endangle = buf2;
					break;
				
				case R:
					app[number].screen[screennum].r = buf2;
					break;

				case G:
					app[number].screen[screennum].g = buf2;
					break;

				case B:
					app[number].screen[screennum].b = buf2;
					break;

				case R_BG:
					app[number].screen[screennum].r_bg = buf2;
					break;

				case G_BG:
					app[number].screen[screennum].g_bg = buf2;
					break;

				case B_BG:
					app[number].screen[screennum].b_bg = buf2;
					break;

				case ALPHA:
					app[number].screen[screennum].alpha = buf2;
					break;
				
				case FONTWIDTH:
					app[number].screen[screennum].font_bitmap.width = buf2;
					break;
					
				case FONTHEIGHT:
					app[number].screen[screennum].font_bitmap.height = buf2;
					break;
					
				case FONTSIZE:
					app[number].screen[screennum].font_ttf.size = buf2;
					break;
					
				case FONTSTYLE:
					switch (buf2)
                    {
                        case FONTSTYLE_NORMAL:
                            /* set normal style. reset other styles */

                            app[number].screen[screennum].font_ttf.style_normal = TRUE;
                            app[number].screen[screennum].font_ttf.style_bold = FALSE;
                            app[number].screen[screennum].font_ttf.style_italic = FALSE;
                            app[number].screen[screennum].font_ttf.style_underline = FALSE;
                            break;

                        case FONTSTYLE_BOLD:
                            app[number].screen[screennum].font_ttf.style_bold = TRUE;
                            break;

                        case FONTSTYLE_ITALIC:
                            app[number].screen[screennum].font_ttf.style_italic = TRUE;
                            break;

                        case FONTSTYLE_UNDERLINE:
                            app[number].screen[screennum].font_ttf.style_underline = TRUE;
                            break;
                    }
                    break;
					
				case GADGET:
                    app[number].screen[screennum].gadget_index = buf2;
                    break;
					
				case GADGETS:
                    /* allocate space for gadgets */

                    init_gadgets (number, screennum, buf2);
                    break;
				
				case GADGET_STATUS:
                    app[number].screen[screennum].gadget_status = buf2;
                    break;

                case GADGET_INT_VALUE:
                    app[number].screen[screennum].gadget_int_value = buf2;
                    break;

                case GADGET_CYCLE_ENTRIES:
                    /* allocate space for menu text array */

                    if (app[number].screen[screennum].gadget_cycle_text)
                    {
                        dealloc_array_U1 (app[number].screen[screennum].gadget_cycle_text, app[number].screen[screennum].gadget_cycle_menu_entries);
                    }

                    app[number].screen[screennum].gadget_cycle_text = (U1 **) malloc (buf2 * sizeof (app[number].screen[screennum].gadget_cycle_text));
                    if (app[number].screen[screennum].gadget_cycle_text == NULL)
                    {
                        printf ("error: can't allocate cycle gadget text!\n");
                    }
                    else
                    {
                        for (i = 0; i < buf2; i++)
                        {
                            app[number].screen[screennum].gadget_cycle_text[i] = NULL;
                        }
                    }

                    app[number].screen[screennum].gadget_cycle_menu_entries = buf2;
                    app[number].screen[screennum].gadget_cycle_text_index = 0;
                    break;

                case GADGET_STRING_STR_LEN:
                    app[number].screen[screennum].gadget_string_string_len = buf2;
                    break;

                case GADGET_STRING_VIS_LEN:
                    app[number].screen[screennum].gadget_string_visible_len = buf2;
                    break;

                case VX:
                    if (app[number].screen[screennum].vx_index < app[number].screen[screennum].vectors)
                    {
                        if (app[number].screen[screennum].vx)
                        {
                            app[number].screen[screennum].vx[app[number].screen[screennum].vx_index] = buf2;
                            app[number].screen[screennum].vx_index++;
                        }
                        else
                        {
                            printf ("error: polygon vx not allocated!\n");
                        }
                    }
                    else
                    {
                        printf ("error: polygon vx overflow!\n");
                    }
                    break;

                case VY:
                    if (app[number].screen[screennum].vy_index < app[number].screen[screennum].vectors)
                    {
                        if (app[number].screen[screennum].vy)
                        {
                            app[number].screen[screennum].vy[app[number].screen[screennum].vy_index] = buf2;
                            app[number].screen[screennum].vy_index++;
                        }
                        else
                        {
                            printf ("error: polygon vy not allocated!\n");
                        }
                    }
                    else
                    {
                        printf ("error: polygon vy overflow!\n");
                    }
                    break;
            }
		}
		
		if (command >= TEXT && command <= GADGET_CYCLE_TEXT)
        {
            if (! read_line_string (app[number].tcpsock, arg, 255))
            {
                printf ("error: can't read text!\n");
                endconn = TRUE;
                break;
            }

            switch (command)
            {
                case TEXT:
                    strcpy (app[number].screen[screennum].text, arg);
                    break;

                case SCREENTITLE:
                    strcpy (app[number].screen[screennum].title, arg);
                    break;

                case SCREENICON:
                    strcpy (app[number].screen[screennum].icon, arg);
                    break;

                case PICTURENAME:
                    strcpy (app[number].screen[screennum].picture_name, arg);
                    break;

                case FONTNAME:
                    strcpy (app[number].screen[screennum].fontname, arg);
                    break;

                case GADGET_STRING_VALUE:
                    strcpy (app[number].screen[screennum].gadget_string_value, arg);
                    break;

                case GADGET_CYCLE_TEXT:
                    /* add menu texts */

                    if (app[number].screen[screennum].gadget_cycle_text_index < app[number].screen[screennum].gadget_cycle_menu_entries)
                    {
                        if (app[number].screen[screennum].gadget_cycle_text)
                        {
                            app[number].screen[screennum].gadget_cycle_text[app[number].screen[screennum].gadget_cycle_text_index] = (U1 *) malloc ((strlen (arg) + 1) * sizeof (U1));
                            if (app[number].screen[screennum].gadget_cycle_text[app[number].screen[screennum].gadget_cycle_text_index])
                            {
                                strcpy (app[number].screen[screennum].gadget_cycle_text[app[number].screen[screennum].gadget_cycle_text_index], arg);
                                app[number].screen[screennum].gadget_cycle_text_index++;
                            }
                            else
                            {
                                printf ("error: cycle gadget text entry not allocated!\n");
                            }
                        }
                        else
                        {
                            printf ("error: cycle gadget text not allocated!\n");
                        }
                    }
                    else
                    {
                        printf ("error: cycle gadget text overflow!\n");
                    }
                    break;
            }
        }
        
        if (command >= RS232_COMPORT_NUMBER && command <= GADGET_PROGRESS_BAR_VALUE)
		{
			switch (command)
			{		
				case RS232_COMPORT_NUMBER:
					if (! read_16 (app[number].tcpsock, &buf2))
					{
						printf ("error: can't read data!\n");
                        break;
                    }
                    
                    app[number].rs232.comport = buf2;
					break;
					
				case RS232_BAUDRATE:
					if (! read_16 (app[number].tcpsock, &buf2))
					{
						printf ("error: can't read data!\n");
                        break;
                    }
                    
                    app[number].rs232.baudrate = buf2;
					break;
                    
				case RS232_BUF_SIZE:
					if (! read_16 (app[number].tcpsock, &buf2))
					{
						printf ("error: can't read data!\n");
                        break;
                    }
                    
                    app[number].rs232.bufsize = buf2;
					break;
					
				case RS232_BYTE:
					if (! read_8 (app[number].tcpsock, &buf2))
					{
						printf ("error: can't read data!\n");
                        break;
                    }
                    app[number].rs232.buf[0] = buf2;
					break;
					
				case RS232_BUF:
					for (i = 0; i < app[number].rs232.bufsize; i++)
					{
						if (! read_8 (app[number].tcpsock, &buf2))
						{
							printf ("error: can't read data!\n");
							break;
						}
						app[number].rs232.buf[i] = buf2;
					}
					break;
					
				case GADGET_PROGRESS_BAR_VALUE:
					if (! read_16 (app[number].tcpsock, &buf2))
					{
						printf ("error: can't read data!\n");
                        break;
                    }
                    
					app[number].screen[screennum].gadget_progress_value = buf2;
					break;
            }
        }
	}
	
	close (app[number].tcpsock);
	
#if ! SINGLE_APP_MODE
	pthread_exit (0);
#endif
}




/* start menu ======================================================= */

S2 start_menu_event (void)
{
	S2 screennum = 0;
    SDL_Event event;
    U1 wait, cycle_menu;
    Uint8 buttonmask, mouse_button, double_click, select;
    U2 gadget, i;
    S4 value, old_value;
    int x, y;

    struct gadget_button *button;

    if (! app[0].screen[screennum].gadget)
    {
        printf ("gadget_event: error gadget list not allocated!\n");
        return;
    }

    while (1 == 1)
    {
        wait = TRUE;

        while (wait)
        {
			/* wait for event */
            if (! SDL_WaitEvent (&event))
            {
                printf ("gadget_event: error can't wait for event!\n");
                return (-1);
            }
        }

        for (i = 0; i < app[0].screen[screennum].gadgets; i++)
        {
            if (app[0].screen[screennum].gadget[i].gptr)
            {
                switch (app[0].screen[screennum].gadget[i].type)
                {
                    case GADGET_BUTTON:
                        button = (struct gadget_button *) app[0].screen[screennum].gadget[i].gptr;

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

                                    if (! draw_gadget_button (app[0].screen[screennum].renderer, 0, screennum, i, GADGET_SELECTED))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        return (-1);
                                    }
                                }
                                else
                                {
                                    printf ("gadget_event: drawing button %i normal.\n", i);

                                    if (! draw_gadget_button (app[0].screen[screennum].renderer, 0,screennum, i, GADGET_NOT_SELECTED))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        return (-1);
                                    }
                                }
                            }

                            if (! draw_gadget_button (app[0].screen[screennum].renderer, 0,screennum, i, GADGET_NOT_SELECTED))
                            {
                                printf ("gadget_event: error can't draw gadget!\n");
                                return (-1);
                            }

                            if ((x > button->x && x < button->x2) && (y > button->y && y < button->y2))
                            {
                                /* gadget was selected */

                                printf ("gadget_event: button gadget %i selected.\n", i);

                                /* send data */
                                return (i);
                            }
                        }
                        break;
				}
			}
		}
	}
}


S2 start_menu (void)
{
	FILE *fptr;
	
	U1 startmenufile[256];
	
	S2 x = 50, y = 10;
	S2 max_entries = 9;
	S2 i = 0, g;
	S2 j, len;
	
	U1 rbuf[80];
	U1 *read, ok;
	S2 gadget;
	
	
	// show splashscreen
	
	// set background color
	
	app[0].screen[0].r = 0;
	app[0].screen[0].g = 0;
	app[0].screen[0].b = 0;
	
	clear_screen (0, 0);
	
	
	/* load 260 x 244 splash screen logo (black/white) */
	load_scan ("/sdcard/nanovm/nano-android-splash.scn", app[0].screen[0].surface);
	SDL_UpdateWindowSurface (app[0].screen[0].window);
	
	/* wait 5 secs */
	
	SDL_Delay (5000);
	
	
	/* read startmenu.txt */
	
#if START_MENU
	strcpy (startmenufile, getenv (NANOVM_ROOT_SB));
	strcat (startmenufile, "/home/startmenu.txt");

	if ((fptr = fopen (startmenufile, "r")) == NULL)
	{
		/* ERROR can't open file! */
		return (1);
	}
	
#else

	if ((fptr = fopen ("/sdcard/nanovm/home/startmenu.txt", "r")) == NULL)
	{
		/* ERROR can't open file! */
		return (1);
	}
	
#endif

	init_gadgets (0, 0, 10);
	
	
	// set colors

	app[0].screen[0].gadget_color.backgr_light.r = 232;
	app[0].screen[0].gadget_color.backgr_light.g = 239;
	app[0].screen[0].gadget_color.backgr_light.b = 247;
	
	app[0].screen[0].gadget_color.backgr_shadow.r = 194;
	app[0].screen[0].gadget_color.backgr_shadow.g = 201;
    app[0].screen[0].gadget_color.backgr_shadow.b = 209;
    
	app[0].screen[0].gadget_color.border_light.r = 143;
	app[0].screen[0].gadget_color.border_light.g = 147;
	app[0].screen[0].gadget_color.border_light.b = 151;
	
	app[0].screen[0].gadget_color.border_shadow.r = 85;
	app[0].screen[0].gadget_color.border_shadow.r = 87;
    app[0].screen[0].gadget_color.border_shadow.r = 90;
    
    app[0].screen[0].gadget_color.text_light.r = 0;
	app[0].screen[0].gadget_color.text_light.g = 0;
	app[0].screen[0].gadget_color.text_light.b = 0;
	
    app[0].screen[0].gadget_color.text_shadow.r = 255;
	app[0].screen[0].gadget_color.text_shadow.g = 255;
	app[0].screen[0].gadget_color.text_shadow.b = 255;
	
	
	// load font
	
	strcpy (app[0].screen[0].fontname, "truetype/freefont/FreeMono.ttf");
	app[0].screen[0].font_ttf.size = 25;
	
	if (load_font_ttf (0, 0) == FALSE)
	{
		printf ("ERROR: can't load font!\n");
		return (1);
	}
	
	// set background color
	
	app[0].screen[0].r = 232;
	app[0].screen[0].g = 239;
	app[0].screen[0].b = 247;
	
	clear_screen (0, 0);
	
	// get path for nano vm, get first line of config
	
	read = fgets_uni (rbuf, 256, fptr);
	if (read == NULL)
	{
		return (-1);
	}
	
	len = strlen (rbuf);
				
	for (j = 0; j < len - 1; j++)
	{
		nanovm_path[j] = rbuf[j];
	}
	nanovm_path[j] = '\0';
	
	
	strcpy (menu[i], "EXIT");
	
	ok = 0;
	while (ok == 0)
	{
		read = fgets_uni (rbuf, 80, fptr);
		if (read != NULL && (i <= max_entries))
		{
			if (rbuf[0] != '#')
			{
				/* no comment, get entry */
				
				i++;
				
				len = strlen (rbuf);
				
				for (j = 0; j < len - 1; j++)
				{
					menu[i][j] = rbuf[j];
				}
				menu[i][j] = '\0';
				
				printf ("menu: %s\n", rbuf);
			}
		}
		else
		{
			ok = 1;
		}
	}
	
	for (g = 0; g <= i; g++)
	{
		app[0].screen[0].x = x;
		app[0].screen[0].y = y;
		app[0].screen[0].gadget_status = 1;
		app[0].screen[0].gadget_index = g;
		
		strcpy (app[0].screen[0].text, menu[g]);
		
		set_gadget_button (0, 0);
		
		y = y + 45;
	}
	
	SDL_UpdateWindowSurface (app[0].screen[0].window);
	
	gadget = start_menu_event ();
	
	return (gadget);
}




/* main ============================================================= */


 #ifdef _WIN32
 int WinMain(int ac, char * av[]) {
 #else ifdef _LINUX || ifdef __ANDROID__
 int main (int ac, char *av[]) {
 #endif
	 
    U1 run = TRUE;
    S2 command;
	U2 port;
	U1 wait_for_screen = 0;
	
	S2 app_handle;
	
	int x = 10, y = 10;

	
	/* native sockets stuff */
	int yes = 1;
    S2 server, error;
    struct sockaddr_in address;
    S4 i, socketh = -1;

    struct sockaddr_in client;
    socklen_t len = sizeof (client);
	
	/* Android start menu */
	S2 program;
	U1 command_shell[256];
	pid_t pID;
	
    printf ("flow 2.0.0\n");

	
#if __ANDROID__ || START_MENU
	/* set port automatically to 2000 */
	
	port = 2000;
#else
	
    if (ac < 2)
    {
       printf ("flow <port>\n");
       exit (1);
    }

    if (strcmp (av[1], "-h") == 0)
    {
       printf ("flow <port>\n");
       exit (1);
    }

    port = atoi (av[1]);    /* get port */
#endif

	init_screens ();
	
    printf ("Initializing SDL.\n");

    /* Initialize the SDL library */

    /* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	/* Initialize SDL */
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init fail : %s\n", SDL_GetError());
		
		#if __ANDROID__
			LOGE("SDL init error!\n");
		#endif
			
		exit (1);
	}

    
#if __ANDROID__ || START_MENU
    /* Alpha blending doesn't work well at 8-bit color */
		info = SDL_GetVideoInfo();
		if (info->vfmt->BitsPerPixel > 8)
		{
			video_bpp = info->vfmt->BitsPerPixel;
		
		}
		else
		{
			video_bpp = 16;
		}
		
    app[0].screen[0].width = info->current_w;
	app[0].screen[0].height = info->current_h;
	strcpy (app[0].screen[0].title, "");

    if (! open_new_screen (0, 0))
    {
		printf ("openscreen error!\n");
		
#if ! START_MENU
		LOGE("openscreen error!\n");
#endif
		
        exit (1);
    }
    
#endif

    /*
    if (Mix_OpenAudio (audio_rate, audio_format, audio_channels, audio_buffers))
    {
        fprintf (stderr, "SDL Unable to open audio!\n");
		
#if __ANDROID__
		LOGE("SDL Unable to open audio!");
#endif
        exit(1);
    }
	*/

	// mouse_cursor = init_system_cursor (&arrow);
	// SDL_SetCursor (mouse_cursor);
	
	// set_simple_cursor ();
	
    printf ("SDL initialized.\n");

    /* Initialize the TTF library */
    
	/* Initialize the TTF library */
    if ( TTF_Init() < 0 ) {
        fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
        SDL_Quit();
        return(2);
    }
	
    /* Clean up on exit */
    atexit (SDL_Quit);

	
#if __ANDROID__ || START_MENU

	/* run the program menu: launch Nano VM */
	
	program = start_menu ();
	
	/* check for EXIT gadget selected */
	if (program == 0)
	{
		exit (0);
	}
	
	draw_text_ttf (app[0].screen[0].surface, 0, 0, "starting Nano VM...", 50, 400, 0, 0, 0);
	
	// strcpy (command_shell, "/data/data/jackpal.androidterm/kbox2/bin/nanovm ");
	
	strcpy (command_shell, nanovm_path);
	strcat (command_shell, " ");
	strcat (command_shell, menu[program]);
	strcat (command_shell, " 127.0.0.1 2000 &");
	
#endif
		
	
#if SOCKETS_NATIVE
	printf ("starting native socket...\n");

	strcpy (ip, "127.0.0.1");
	//exe_gethostbyaddr (ip, &addr);
	
	address.sin_family = AF_INET;
    
	// address.sin_addr.s_addr = inet_addr (ip);
	
	address.sin_addr.s_addr = htonl(INADDR_ANY);
    
	address.sin_port = htons ((U2) port);
    memset (&(address.sin_zero), BINUL, 8); 
	
	server = socket (PF_INET, SOCK_STREAM, 0);
    if (server == -1)
    {
        printf ("can't open socket!\n");
#if __ANDROID__
	LOGE("can't open socket!");
#endif	
		exit (1);
    }

    /* avoiding socket already in use error */

    error = (setsockopt (server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (NINT)));
    if (error == -1)
    {
        printf ("can't setsockopt!\n");
#if __ANDROID__
	LOGE("can't open setsockopt!");
#endif	
		exit (1);
    }

    error = bind (server, (struct sockaddr *) &address, sizeof (address));
    if (error == -1)
    {
        printf ("can't bind!\n");
#if __ANDROID__
	LOGE("can't open bind!");
#endif	
		exit (1);
    }

    error = listen (server, SOMAXCONN);
    if (error == -1)
    {
        printf ("can't listen!\n");
#if __ANDROID__
	LOGE("can't listen!");
#endif	
		exit (1);
    }
	
#else	

    /* SDL socket init */

    if (SDLNet_Init () == -1)
    {
        printf ("SDLNet_Init: %s\n", SDLNet_GetError ());
#if __ANDROID__
	LOGE("SDL Unable to open Net!");
#endif	
        exit (1);
   
	}

    // --------------------------------------------------------------

    if (SDLNet_ResolveHost (&ipadd, NULL, port) == -1)
    {
        printf ("SDLNet_ResolveHost: %s\n", SDLNet_GetError ());
#if __ANDROID__
	LOGE("SDL Unable to open SDL_Net_ResolveHost!");
#endif	
        exit (1);
    }

    tcpsock = SDLNet_TCP_Open (&ipadd);
    if (!tcpsock)
    {
        printf ("SDLNet_TCP_Open: %s\n", SDLNet_GetError ());
#if __ANDROID__
	LOGE("SDL Unable to SDLNet_TCP_Open!");
#endif	
        exit (1);
    }

#endif   
    
    

#if __ANDROID__ || START_MENU
	
#if ! START_MENU
    LOGD("waiting...\n");
#endif
	
	system (command_shell);
	
	draw_text_ttf (app[0].screen[0].surface, 0, 0, "waiting for connection...", 50, 440, 0, 0, 0);
	
#endif
	
	
    while (run)
    {
        // printf ("waiting...........\n");

#if SOCKETS_NATIVE
		new_tcpsock = accept (server, (struct sockaddr *) &client, &len);
		if (new_tcpsock == -1)
		{
			SDL_Delay (10);
			continue;
		}
#else		
        new_tcpsock = SDLNet_TCP_Accept (tcpsock);
        if (!new_tcpsock)
        {
            /* printf( "SDLNet_TCP_Accept: %s\n", SDLNet_GetError ()); */
            SDL_Delay (10);     /* wait 1/100 Second */
            continue;
        }
#endif
        else
        {

            /* communicate over new_tcpsock */

			app_handle = get_free_app_handle ();
			if (app_handle > -1)
			{
				send_8 (new_tcpsock, OK);
				app[app_handle].tcpsock = new_tcpsock;
				
#if SINGLE_APP_MODE
				handle_client (app_handle);
#else
				printf("STARTING APP HANDLE THREAD...\n");
				pthread_create (&app[app_handle].thread, NULL, (void *) handle_client, (void*) app_handle);
#endif
			}
			else
			{
				send_8 (new_tcpsock, ERROR);
			}	
        }
    }
	
	
	
    
    free_memory ();

    printf ("Quiting SDL.\n");

    /* Shutdown all subsystems */

#if WITH_SOUND
    Mix_CloseAudio();
#endif
	
#if ! SOCKETS_NATIVE
    SDLNet_Quit ();
#endif
	
	TTF_Quit ();
	
    SDL_Quit ();
	
    printf ("Quiting....\n");
    exit (0);
}

