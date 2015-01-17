/****************************************************************************
*
* Filename: main.c
*
* Author:   Stefan Pietzonke
* Project:  flow, the gfx plugin for nano VM
*
* Purpose:  main functions
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

#if __ANDROID__

#include <android/log.h>

#define  LOG_TAG    "flow-error"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#endif

#include "global_d.h"




/* rs232 struct */
struct rs232 rs232;


/* Raspberry Pi struct */
struct rpi rpi;


/* SDL stuff */

#if SOCKETS_NATIVE
	U1 ip[256], addr[256];
	S2 new_tcpsock;
#else
	IPaddress ip;
	TCPsocket tcpsock, new_tcpsock;
#endif


struct screen screen[MAXSCREEN];
S2 screennum = 0;

extern SDL_Surface *bmap_copy;


SDL_Surface *cursor;
Sint16 mouse_x, mouse_y;

SDL_Cursor *mouse_cursor;

/* Stolen from the mailing list */
/* Creates a new mouse cursor from an XPM */


/* XPM */
static const char *arrow[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  "X                               ",
  "XX                              ",
  "X.X                             ",
  "X..X                            ",
  "X...X                           ",
  "X....X                          ",
  "X.....X                         ",
  "X......X                        ",
  "X.......X                       ",
  "X........X                      ",
  "X.....XXXXX                     ",
  "X..X..X                         ",
  "X.X X..X                        ",
  "XX  X..X                        ",
  "X    X..X                       ",
  "     X..X                       ",
  "      X..X                      ",
  "      X..X                      ",
  "       XX                       ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "0,0"
};

static SDL_Cursor *init_system_cursor(const char *image[])
{
  int i, row, col;
  Uint8 data[4*32];
  Uint8 mask[4*32];
  int hot_x, hot_y;

  i = -1;
  for ( row=0; row<32; ++row ) {
    for ( col=0; col<32; ++col ) {
      if ( col % 8 ) {
        data[i] <<= 1;
        mask[i] <<= 1;
      } else {
        ++i;
        data[i] = mask[i] = 0;
      }
      switch (image[4+row][col]) {
        case 'X':
          data[i] |= 0x01;
          mask[i] |= 0x01;
          break;
        case '.':
          mask[i] |= 0x01;
          break;
        case ' ':
          break;
      }
    }
  }
  sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
  return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}


/* test ------------------------------------- */

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


void init_screens (void)
{
    S2 i;

    for (i = 0; i < MAXSCREEN; i++)
    {
        screen[i].status = CLOSED;
        strcpy (screen[i].title, "");
        strcpy (screen[i].icon, "");
        strcpy (screen[i].text, "");
        strcpy (screen[i].fontname, "");

        screen[i].vx = NULL;
        screen[i].vy = NULL;
        screen[i].font_bitmap.buf = NULL;

        screen[i].font_ttf.font = NULL;
        screen[i].font_ttf.style_normal = TRUE;
        screen[i].font_ttf.style_bold = FALSE;
        screen[i].font_ttf.style_italic = FALSE;
        screen[i].font_ttf.style_underline = FALSE;

        screen[i].gadget = NULL;
        screen[i].gadgets = 0;
        screen[i].gadget_index = 0;

        screen[i].gadget_cycle_text = NULL;
        screen[i].gadget_cycle_menu_entries = 0;
        screen[i].gadget_cycle_text_index = 0;
    }
}

void free_memory (void)
{
    S2 i;

    for (i = 0; i < MAXSCREEN - 1; i++)
    {
        if (screen[i].vx)
        {
            free (screen[i].vx);
        }

        if (screen[i].vy)
        {
            free (screen[i].vy);
        }

        if (screen[i].font_bitmap.buf)
        {
            free (screen[i].font_bitmap.buf);
        }

        if (screen[i].font_ttf.font)
        {
            TTF_CloseFont (screen[i].font_ttf.font);
        }

        if (screen[i].gadget_cycle_text)
        {
            dealloc_array_U1 (screen[i].gadget_cycle_text, screen[i].gadget_cycle_menu_entries);
        }
    }

    if (screen[BLITSCREEN].bmap)
    {
        SDL_FreeSurface (screen[BLITSCREEN].bmap);
        printf ("free_memory: BLITSCREEN freed.\n");
    }

    if (bmap_copy)
    {
        SDL_FreeSurface (bmap_copy);
        bmap_copy = NULL;
    }

    free_gadgets ();
}

void set_clip_noborder (SDL_Surface *screen, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2)
{
    SDL_Rect clip;
    clip.x = x1;
    clip.y = y1;
    clip.w = x2 - x1;
    clip.h = y2 - y1;
    SDL_SetClipRect(screen, &clip);
}

void clear_screen (S2 screennum)
{
    Uint32 color;

    /* Setup clear color color */
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    {
        color = SDL_MapRGBA (screen[screennum].bmap->format, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
    }
    else
    {
        color = SDL_MapRGBA (screen[screennum].bmap->format, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
    }

    /* Clear the screen */
    set_clip_noborder (screen[screennum].bmap, 0, 0, screen[screennum].width, screen[screennum].height);
    SDL_FillRect (screen[screennum].bmap, NULL, color);
}


SDL_Surface *screen_copy = NULL;

void create_screen_copy_surface (void)
{
	if (screen_copy == NULL)
	{
		/* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
       as expected by OpenGL for textures */
		
		Uint32 rmask, gmask, bmask, amask;

		/* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			rmask = 0xff000000;
			gmask = 0x00ff0000;
			bmask = 0x0000ff00;
			amask = 0x00000000;
		#else
			rmask = 0x000000ff;
			gmask = 0x0000ff00;
			bmask = 0x00ff0000;
			amask = 0x00000000;
		#endif

		screen_copy = SDL_CreateRGBSurface (SDL_SWSURFACE, screen[screennum].width, screen[screennum].height, screen[screennum].video_bpp, 0, 0, 0, 0);
		if (screen_copy == NULL) 
		{
			fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
			exit(1);
		}
		
		if (SDL_SetAlpha (screen_copy, SDL_SRCALPHA, 0) < 0)
		{
			fprintf (stderr, "Can't set alpha channel!\n");
		}
	}
}

void copy_whole_screen (void)
{
	create_screen_copy_surface ();
	
    copy_surface (screen[screennum].bmap, NULL, screen_copy, NULL);
}

U1 copy_screen ()
{
	copy_whole_screen ();
	
	return (TRUE);
}

void update_screen (S2 screennum)
{
	#if __ANDROID__
		LOGD("update screen: %i\n", screennum);
	#endif
	
    if (screen[screennum].status == OPEN)
    {
        // SDL_UpdateRect (screen[screennum].bmap, 0, 0, 0, 0);
        SDL_Flip (screen[screennum].bmap);
        
        printf ("[SCREEN UPDATED]\n");
		
		#if __ANDROID__
			LOGD("screen updated\n");
		#endif
			
		copy_screen ();	
    }
}

U1 open_new_screen (S2 screennum, Uint8 bit)
{
    SDL_Surface *new_screen;

    if (screen[screennum].status == OPEN || screennum < 0 || screennum >= MAXSCREEN - 2)
    {
        /* error: screen already open or num out of range */

        return (FALSE);
    }

#if __ANDROID__ 
    new_screen = SDL_SetVideoMode (screen[screennum].width, screen[screennum].height, bit, SDL_SWSURFACE | SDL_SRCALPHA | SDL_RESIZABLE | SDL_FULLSCREEN | SDL_ANYFORMAT);
#else
	new_screen = SDL_SetVideoMode (screen[screennum].width, screen[screennum].height, bit, SDL_SWSURFACE | SDL_SRCALPHA | SDL_RESIZABLE | SDL_ANYFORMAT);
#endif	
	
    if (new_screen == NULL)
    {
        fprintf (stderr, "Couldn't set %i x %i x %i video mode: %s\n", screen[screennum].width, screen[screennum].height, bit, SDL_GetError ());
        return (FALSE);
    }

    /* Use alpha blending */
    if (SDL_SetAlpha (new_screen, SDL_SRCALPHA, 0) < 0)
    {
        fprintf (stderr, "Can't set alpha channel!\n");
        return (FALSE);
    }

    SDL_WM_SetCaption (screen[screennum].title, screen[screennum].icon);

    screen[screennum].bmap = new_screen;
    screen[screennum].video_bpp = bit;
    screen[screennum].status = OPEN;
	
#if __ANDROID__
	LOGD("screen: %i open\n", screennum);
#endif

    clear_screen (screennum);
    return (TRUE);
}

U1 reopen_screen (S2 screennum, S2 width, S2 height)
{
	if (screennum < 0 || screennum >= MAXSCREEN - 2)
    {
        /* error: screen num out of range */

        return (FALSE);
    }

    screen[screennum].bmap = SDL_SetVideoMode (width, height, screen[screennum].video_bpp, SDL_SWSURFACE | SDL_SRCALPHA | SDL_RESIZABLE | SDL_FULLSCREEN | SDL_ANYFORMAT);
    
	// screen[screennum].bmap = SDL_SetVideoMode (screen[screennum].width, screen[screennum].height, screen[screennum].video_bpp, SDL_SWSURFACE | SDL_SRCALPHA | SDL_RESIZABLE | SDL_FULLSCREEN | SDL_ANYFORMAT);
	if (screen[screennum].bmap == NULL)
    {
        fprintf (stderr, "Couldn't set %i x %i x %i video mode: %s\n", screen[screennum].width, screen[screennum].height, screen[screennum].video_bpp, SDL_GetError ());
        return (FALSE);
    }

    /* Use alpha blending */
    if (SDL_SetAlpha (screen[screennum].bmap, SDL_SRCALPHA, 0) < 0)
    {
        fprintf (stderr, "Can't set alpha channel!\n");
        return (FALSE);
    }

    SDL_WM_SetCaption (screen[screennum].title, screen[screennum].icon);
    
    return (TRUE);
}

U1 close_screen (S2 screennum)
{
    if (screen[screennum].status == CLOSED || screennum < 0 || screennum >= MAXSCREEN)
    {
        /* error: screen already closed or num out of range */

        return (FALSE);
    }

    /* SDL_FreeSurface (screen[screennum].bmap); ???*/

    screen[screennum].status = CLOSED;
    return (TRUE);
}


/* graphic primitives from SDL_gfx */

void draw_pixel (S2 screennum)
{
    pixelRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}

void draw_line (S2 screennum)
{
    lineRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].x2, screen[screennum].y2 ,screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}

void draw_rectangle (S2 screennum)
{
    rectangleRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].x2, screen[screennum].y2 ,screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}

void draw_rectangle_fill (S2 screennum)
{
    boxRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].x2, screen[screennum].y2, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}

void draw_circle (S2 screennum)
{
    circleRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].radius, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}

void draw_circle_fill (S2 screennum)
{
    filledCircleRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].radius, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}

void draw_ellipse (S2 screennum)
{
    ellipseRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].xradius, screen[screennum].yradius, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}

void draw_ellipse_fill (S2 screennum)
{
    filledEllipseRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].xradius, screen[screennum].yradius, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}

void draw_pie (S2 screennum)
{
    pieRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].radius, screen[screennum].startangle, screen[screennum].endangle, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}

void draw_pie_fill (S2 screennum)
{
    filledPieRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].radius, screen[screennum].startangle, screen[screennum].endangle, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}

void draw_trigon (S2 screennum)
{
    trigonRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].x2, screen[screennum].y2, screen[screennum].x3, screen[screennum].y3, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}

void draw_trigon_filled (S2 screennum)
{
    filledTrigonRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].x2, screen[screennum].y2, screen[screennum].x3, screen[screennum].y3, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}

U1 draw_polygon (S2 screennum)
{
    if (screen[screennum].vx == NULL || screen[screennum].vy == NULL)
    {
        /* error: coordinate arrays not allocated */
        return (FALSE);
    }

    polygonRGBA (screen[screennum].bmap, screen[screennum].vx, screen[screennum].vy, screen[screennum].vectors, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);

    return (TRUE);
}

U1 draw_polygon_filled (S2 screennum)
{
    if (screen[screennum].vx == NULL || screen[screennum].vy == NULL)
    {
        /* error: coordinate arrays not allocated */
        return (FALSE);
    }

    filledPolygonRGBA (screen[screennum].bmap, screen[screennum].vx, screen[screennum].vy, screen[screennum].vectors, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);

    return (TRUE);
}

U1 draw_bezier (S2 screennum)
{
    if (screen[screennum].vx == NULL || screen[screennum].vy == NULL)
    {
        /* error: coordinate arrays not allocated */
        return (FALSE);
    }

    /* high values for steps gives a smoother curve */

    bezierRGBA (screen[screennum].bmap, screen[screennum].vx, screen[screennum].vy, screen[screennum].vectors, screen[screennum].steps, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);

    return (TRUE);
}


U1 load_font_bitmap (S2 screennum)
{
    FILE *font;
    U2 filesize;
    U1 *fontname;
    S2 fontname_len;
	
	U1 android_path[] = "/sdcard/nanovm/fonts/";

    /* check for font path env variable */

    
#if __ANDROID__
		fontname_len = strlen (android_path);
        fontname_len = fontname_len + strlen (screen[screennum].fontname) + 1;

        /* allocate buffer for fontname */

        fontname = (U1 *) malloc (fontname_len * sizeof (U1));
        if (fontname == NULL)
        {
            printf ("load_font_bitmap: can't allocate %i bytes for fontname!\n", fontname_len);
            return (FALSE);
        }

        strcpy (fontname, android_path);
        strcat (fontname, screen[screennum].fontname);
		
#else
	if (getenv (FONT_DIR_SB) != NULL)
    {	
        /* env variable set */

        fontname_len = strlen (getenv (FONT_DIR_SB));
        fontname_len = fontname_len + strlen (screen[screennum].fontname) + 1;

        /* allocate buffer for fontname */

        fontname = (U1 *) malloc (fontname_len * sizeof (U1));
        if (fontname == NULL)
        {
            printf ("load_font_bitmap: can't allocate %i bytes for fontname!\n", fontname_len);
            return (FALSE);
        }

        strcpy (fontname, getenv (FONT_DIR_SB));
        strcat (fontname, screen[screennum].fontname);
    }
    else
    {
        /* env variable not set */

        printf ("load_font_bitmap: env variable NANOGFXFONT not set!\n");

        fontname_len = strlen (screen[screennum].fontname) + 1;

        /* allocate buffer for fontname */

        fontname = (U1 *) malloc (fontname_len * sizeof (U1));
        if (fontname == NULL)
        {
            printf ("load_font_bitmap: can't allocate %i bytes for fontname!\n", fontname_len);
            return (FALSE);
        }

        strcpy (fontname, screen[screennum].fontname);
    }
#endif

    if (! (font = fopen (fontname, "rb")))
    {
        /* can't open font */
        printf ("load_font_bitmap: can't open font %s !\n", screen[screennum].fontname);
        printf ("load_font_bitmap: %s\n\n", fontname);
        return (FALSE);
    }

     /* get filesize */

    fseek (font, (long) NULL, SEEK_END);
    filesize = ftell (font);
    fseek (font, (long) NULL, SEEK_SET);

    /* allocate buffer for font data */

    if (screen[screennum].font_bitmap.buf != NULL)
    {
        /* free old font data */

        free (screen[screennum].font_bitmap.buf);
    }

    screen[screennum].font_bitmap.buf = (U1 *) malloc (filesize * sizeof (U1));
    if (screen[screennum].font_bitmap.buf == NULL)
    {
        printf ("load_font_bitmap: can't allocate %i bytes for font!\n", filesize);
        return (FALSE);
    }

    printf ("font size: %i\n", filesize);

    if (fread (screen[screennum].font_bitmap.buf, sizeof (U1), filesize, font) != filesize)
    {
        printf ("load_font_bitmap: can't load font %s !\n", fontname);
        return (FALSE);
    }

    gfxPrimitivesSetFont (screen[screennum].font_bitmap.buf, screen[screennum].font_bitmap.width, screen[screennum].font_bitmap.height);
    fclose (font);
    free (fontname);

    return (TRUE);
}

void draw_text_bitmap (S2 screennum)
{
    stringRGBA (screen[screennum].bmap, screen[screennum].x, screen[screennum].y, screen[screennum].text, screen[screennum].r, screen[screennum].g, screen[screennum].b, screen[screennum].alpha);
}


U1 set_ttf_style (S2 screennum)
{
    U1 style = TTF_STYLE_NORMAL;

    if (screen[screennum].font_ttf.font == NULL)
    {
        printf ("set_ttf_style: can't set style. No font opened!\n");
        return (FALSE);
    }

    if (! screen[screennum].font_ttf.style_normal)
    {
        if (screen[screennum].font_ttf.style_bold)
        {
            style = style | TTF_STYLE_BOLD;
        }

        if (screen[screennum].font_ttf.style_italic)
        {
            style = style | TTF_STYLE_ITALIC;
        }

        if (screen[screennum].font_ttf.style_underline)
        {
            style = style | TTF_STYLE_UNDERLINE;
        }
    }

    TTF_SetFontStyle (screen[screennum].font_ttf.font, style);

    return (TRUE);
}

U1 load_font_ttf (S2 screennum)
{
    U1 *fontname;
    S2 fontname_len;
	
	U1 android_path[] = "/sdcard/nanovm/fonts/";

    if (screen[screennum].font_ttf.font != NULL)
    {
        /* close old font */

        TTF_CloseFont (screen[screennum].font_ttf.font);
    }

    
#if __ANDROID__
	fontname_len = strlen (android_path);
    fontname_len = fontname_len + strlen (screen[screennum].fontname) + 1;

    /* allocate buffer for fontname */

	fontname = (U1 *) malloc (fontname_len * sizeof (U1));
    if (fontname == NULL)
    {
        printf ("load_font_bitmap: can't allocate %i bytes for fontname!\n", fontname_len);
        return (FALSE);
    }

    strcpy (fontname, android_path);
    strcat (fontname, screen[screennum].fontname);
		
#else
	
    /* check for font path env variable */

    if (getenv (NANOVM_ROOT_SB) != NULL)
    {
        /* env variable set */

        fontname_len = strlen (getenv (NANOVM_ROOT_SB)) + strlen ("/fonts/");
        fontname_len = fontname_len + strlen (screen[screennum].fontname) + 1;

        /* allocate buffer for fontname */

        fontname = (U1 *) malloc (fontname_len * sizeof (U1));
        if (fontname == NULL)
        {
            printf ("load_font_ttf: can't allocate %i bytes for fontname!\n", fontname_len);
            return (FALSE);
        }

        strcpy (fontname, getenv (NANOVM_ROOT_SB));
		strcat (fontname, "/fonts/");
        strcat (fontname, screen[screennum].fontname);
    }
    else
    {
        /* env variable not set */

        fontname_len = strlen (screen[screennum].fontname) + 1;

        /* allocate buffer for fontname */

        fontname = (U1 *) malloc (fontname_len * sizeof (U1));
        if (fontname == NULL)
        {
            printf ("load_font_ttf: can't allocate %i bytes for fontname!\n", fontname_len);
            return (FALSE);
        }

        strcpy (fontname, screen[screennum].fontname);
    }

#endif
    
    screen[screennum].font_ttf.font = TTF_OpenFont (fontname, screen[screennum].font_ttf.size);
    if (screen[screennum].font_ttf.font == NULL)
    {
        printf ("load_font_ttf: can't load font %s size %i !\n", fontname, screen[screennum].font_ttf.size);
        free (fontname);
        return (FALSE);
    }
    else
    {
        free (fontname);
        return (TRUE);
    }
}

U1 draw_text_ttf (S2 screennum, U1 *textstr, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Surface *text;
    SDL_Rect dstrect;
    SDL_Color color;

    color.r = r;
    color.g = g;
    color.b = b;

    if (screen[screennum].font_ttf.font == NULL)
    {
        printf ("draw_text_ttf: can't draw text. No font opened!\n");
        return (FALSE);
    }

    if (! set_ttf_style (screennum))
    {
        return (FALSE);
    }

    text = TTF_RenderText_Blended (screen[screennum].font_ttf.font, textstr, color);
    if (text == NULL)
    {
        printf ("draw_text_ttf: can't render text! %s\n", SDL_GetError ());
        return (FALSE);
    }

    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = text->w;
    dstrect.h = text->h;

    SDL_BlitSurface (text, NULL, screen[screennum].bmap, &dstrect);
    SDL_FreeSurface (text);

    return (TRUE);
}


U1 load_picture (S2 screennum)
{
    SDL_Surface *picture;
    SDL_Rect dstrect;

    dstrect.x = screen[screennum].x;
    dstrect.y = screen[screennum].y;
    dstrect.w = 0;
    dstrect.h = 0;

    picture = IMG_Load (screen[screennum].picture_name);
    if (picture == NULL)
    {
        printf ("load_picture: can't load picture %s !\n", screen[screennum].picture_name);
        return (FALSE);
    }

    SDL_BlitSurface (picture, NULL, screen[screennum].bmap, &dstrect);
    SDL_FreeSurface (picture);

    return (TRUE);
}

U1 save_picture (S2 screennum)
{
	SDL_Surface *output_surf;
	Uint32 rmask, gmask, bmask, amask;

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
	#else
	rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
	#endif
	
	update_screen (screennum);
	
	/* Creating the output surface to save */
//	output_surf = SDL_CreateRGBSurface (screen[screennum].bmap->flags, screen[screennum].bmap->w, screen[screennum].bmap->h, screen[screennum].bmap->format->BitsPerPixel, rmask, gmask, bmask, amask);
	output_surf = SDL_CreateRGBSurface (screen[screennum].bmap->flags, screen[screennum].bmap->w, screen[screennum].bmap->h, screen[screennum].bmap->format->BitsPerPixel, 0, 0, 0, 0);
	copy_surface (screen[screennum].bmap, NULL, output_surf, NULL);

	if (SDL_SaveBMP (output_surf, screen[screennum].picture_name) < 0)
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

void get_pixelcolor (S2 screennum)
{
    Uint32 pixel;
    Uint8 r, g, b;

    if (SDL_LockSurface (screen[screennum].bmap) < 0)
    {
        printf ("get_pixelcolor: can't lock surface!\n");
    }

    pixel = getpixel (screen[screennum].bmap, screen[screennum].x, screen[screennum].y);
    SDL_UnlockSurface (screen[screennum].bmap);
    SDL_GetRGB (pixel, screen[screennum].bmap->format, &r, &g, &b);

    /* send data */
    send_8 (new_tcpsock, OK);
    send_16 (new_tcpsock, r);
    send_16 (new_tcpsock, g);
    send_16 (new_tcpsock, b);
}

void get_mouse (S2 screennum)
{
    Uint8 buttonmask, button = 0;
    Uint16 x, y;

    /* check mouse status */

    printf ("get_mouse: SDL_GetMouseState ... ");

    SDL_PumpEvents ();
    buttonmask = SDL_GetMouseState (&x, &y);

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
    send_8 (new_tcpsock, OK);
    send_16 (new_tcpsock, x);
    send_16 (new_tcpsock, y);
    send_16 (new_tcpsock, button);
}


/* read_command () neu ------------------------------------------------- */
/* get the command and call the function to do it */



U1 restore_screen ()
{
	copy_surface (screen_copy, NULL, screen[screennum].bmap, NULL);
	SDL_FreeSurface (screen_copy);
	screen_copy = NULL;
	update_screen (screennum);
	
	return (TRUE);
}
    
    

U1 read_command ()
{
    U1 arg[256], endconn = FALSE;
    U2 buf2;
    S2 i;
	S2 command, ret_command = 0;
	U2 ret;
    SDL_Event event;
	U1 resized = 0;

    printf ("\nread_command\n");

    while (! endconn)
    {
wait_command:
#if __ANDROID__

		/* SDL event handling stuff... */
		
		while (SDL_PollEvent (&event))
		{
			if (event.type == SDL_ACTIVEEVENT)
			{
				while (resized == 0)
				{
					while (SDL_PollEvent (&event))
					{
						if (event.type == SDL_VIDEORESIZE)
						{
							//If the window was iconified or restored
				
							reopen_screen (0, event.resize.w, event.resize.h);
							restore_screen ();
				
							resized = 1;
						}
					}
				}
			}
		}

		/* draw mouse cursor */
		
		// SDL_GetMouseState (&mouse_x, &mouse_y);
		// draw_cursor (cursor, mouse_x, mouse_y);
		// update_screen (0);
		
#endif		
        if (! read_16 (new_tcpsock, &command))
        {
            printf ("error: can't read command!\n");
			
#if __ANDROID__
			LOGE("read-command: error!\n");
#endif
			
            goto wait_command;
        }

        printf ("command: %i\n", command);
		
#if __ANDROID__
		LOGD("read-command: %i\n", command);
#endif

        if (command >= END && command <= SAVE_PICTURE)
        {
            switch (command)
            {
                case END:
                    endconn = TRUE;
                    break;

                case SHUTDOWN:
                    endconn = TRUE;
                    ret_command = command;
                    break;

                case OPEN_SCREEN:
                    endconn = TRUE;
                    ret_command = command;
                    break;

                case CLOSE_SCREEN:
                    endconn = TRUE;
                    ret_command = command;
                    break;

                case CLEAR_SCREEN:
                    clear_screen (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case UPDATE_SCREEN:
                    update_screen (screennum);
					
                    send_8 (new_tcpsock, OK);
                    break;

                case PIXEL:
                    draw_pixel (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case LINE:
                    draw_line (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case RECTANGLE:
                    draw_rectangle (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case RECTANGLE_FILL:
                    draw_rectangle_fill (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case CIRCLE:
                    draw_circle (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case CIRCLE_FILL:
                    draw_circle_fill (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case ELLIPSE:
                    draw_ellipse (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case ELLIPSE_FILL:
                    draw_ellipse_fill (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case PIE:
                    draw_pie (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case PIE_FILL:
                    draw_pie_fill (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case TRIGON:
                    draw_trigon (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case TRIGON_FILL:
                    draw_trigon_filled (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case POLYGON:
                    if (draw_polygon (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case POLYGON_FILL:
                    if (draw_polygon_filled (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case BEZIER:
                    if (draw_bezier (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case TEXT_BMAP:
                    draw_text_bitmap (screennum);
                    send_8 (new_tcpsock, OK);
                    break;

                case LOADFONT_BMAP:
                    if (load_font_bitmap (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case TEXT_TTF:
                    if (draw_text_ttf (screennum, screen[screennum].text, screen[screennum].x, screen[screennum].y, screen[screennum].r, screen[screennum].g, screen[screennum].b))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case LOADFONT_TTF:
                    if (load_font_ttf (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                        send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case LOAD_PICTURE:
                    if (load_picture (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                        send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case GETPIXEL:
                    get_pixelcolor (screennum);
                    break;

                case GETMOUSE:
                    get_mouse (screennum);
                    break;

                case GADGET_COLOR_BORDER_LIGHT:
                    set_gadget_color_border_light (screennum);
                    break;

                case GADGET_COLOR_BORDER_SHADOW:
                    set_gadget_color_border_shadow (screennum);
                    break;

                case GADGET_COLOR_BACKGR_LIGHT:
                    set_gadget_color_backgr_light (screennum);
                    break;

                case GADGET_COLOR_BACKGR_SHADOW:
                    set_gadget_color_backgr_shadow (screennum);
                    break;

                case GADGET_COLOR_TEXT_LIGHT:
                    set_gadget_color_text_light (screennum);
                    break;

                case GADGET_COLOR_TEXT_SHADOW:
                    set_gadget_color_text_shadow (screennum);
                    break;

                case GADGET_BUTTON:
                    if (set_gadget_button (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case GADGET_CHECKBOX:
                    if (set_gadget_checkbox (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case GADGET_CYCLE:
                    if (set_gadget_cycle (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case GADGET_STRING:
                    if (set_gadget_string (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case GADGET_BOX:
                    if (set_gadget_box (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;
					
				case GADGET_PROGRESS_BAR:
					if (set_gadget_progress_bar (screennum))
					{
						 send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;
					
                case GADGET_EVENT:
                    gadget_event (screennum);
                    break;

                case GADGET_GET_X2Y2:
                    get_gadget_x2y2 (screennum);
                    break;

                case GADGET_CHECKBOX_CHANGE:
                    if (change_gadget_checkbox (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;
					
				case GADGET_CYCLE_CHANGE:
					if (change_gadget_cycle (screennum))
					{
						send_8 (new_tcpsock, OK);
					
					}
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;
					
                case GADGET_STRING_CHANGE:
                    if (change_gadget_string (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case GADGET_BOX_CHANGE:
                    if (change_gadget_box (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;
					
				case GADGET_PROGRESS_BAR_CHANGE:
					if (change_gadget_progress_bar (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;

                case SOUND_PLAY_WAV:
                    printf ("DEBUG: playing wav...\n");
                    play_wav (screennum);
                    break;

                case SOUND_STOP_CHANNEL:
                    printf ("DEBUG: stopping channel...\n");
                    stop_sound (screennum);
                    break;

                case SOUND_PLAY_MUSIC:
                    printf ("DEBUG: playing music...\n");
                    play_music (screennum);
                    break;

                case SOUND_STOP_MUSIC:
                    printf ("DEBUG: stopping music...\n");
                    stop_music (screennum);
                    break;
					
				case RS232_OPEN_COMPORT:
					if (RS232_OpenComport (rs232.comport, rs232.baudrate) == 0)
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;
					
				case RS232_POLL_COMPORT:
					ret = RS232_PollComport (rs232.comport, &rs232.buf, rs232.bufsize);
					printf ("pollcomport: %i\n", ret);
					
					if (ret > 0)
					{
                        send_16 (new_tcpsock, ret);
						
						for (i = 0; i < ret; i++)
						{
							send_8 (new_tcpsock, rs232.buf[i]);
							printf ("buf: %i\n", rs232.buf[i]);
						}
                    }
                    else
                    {
                       send_16 (new_tcpsock, 0);
                    }
                    break;
					
				case RS232_SEND_BYTE:
					if (RS232_SendByte (rs232.comport, rs232.buf[0]) == 0)
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;
					
				case RS232_SEND_BUF:
					if (RS232_SendBuf (rs232.comport, rs232.buf, rs232.bufsize) == 0)
					{
						send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                       send_8 (new_tcpsock, ERROR);
                    }
                    break;
					
				case RPI_GPIO_START:
					send_8 (rpi_gpio_start ());
                    break;
					
				case RPI_GPIO_MODE:
					rpi_gpio_mode (rpi.pin, rpi.value);
					break;
					
				case RPI_GPIO_READ:
					ret = rpi_gpio_read ();
					send_16 (new_tcpsock, ret);
					break;
					
				case RPI_GPIO_WRITE:
					rpi_gpio_write (rpi.value);
					break;
					
				case SAVE_PICTURE:
					if (save_picture (screennum))
                    {
                        send_8 (new_tcpsock, OK);
                    }
                    else
                    {
                        send_8 (new_tcpsock, ERROR);
                    }
                    break;
            }
        }

        if (command >= SCREENNUM && command < TEXT)
        {
            if (! read_16 (new_tcpsock, &buf2))
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
                    screen[screennum].width = buf2;
                    break;

                case HEIGHT:
                    screen[screennum].height = buf2;
                    break;

                case BIT:
                    screen[screennum].video_bpp = buf2;
                    break;

                case X:
                    screen[screennum].x = buf2;
                    break;

                case Y:
                    screen[screennum].y = buf2;
                    break;

                case X2:
                    screen[screennum].x2 = buf2;
                    break;

                case Y2:
                    screen[screennum].y2 = buf2;
                    break;

                case X3:
                    screen[screennum].x3 = buf2;
                    break;

                case Y3:
                    screen[screennum].y3 = buf2;
                    break;

                case VECTORS:
                    /* allocate space for polygon x/y coordinates */

                    if (screen[screennum].vx)
                    {
                        free (screen[screennum].vx);
                    }

                    if (screen[screennum].vy)
                    {
                        free (screen[screennum].vy);
                    }

                    screen[screennum].vx = (Sint16 *) malloc (buf2 * sizeof (Sint16));
                    if (screen[screennum].vx == NULL)
                    {
                        printf ("error: can't allocate polygon vx!\n");
                    }

                    screen[screennum].vy = (Sint16 *) malloc (buf2 * sizeof (Sint16));
                    if (screen[screennum].vy == NULL)
                    {
                        printf ("error: can't allocate polygon vy!\n");
                    }

                    screen[screennum].vectors = buf2;
                    screen[screennum].vx_index = 0;
                    screen[screennum].vy_index = 0;
                    break;

                case STEPS:
                    screen[screennum].steps = buf2;
                    break;

                case RADIUS:
                    screen[screennum].radius = buf2;
                    break;

                case XRADIUS:
                    screen[screennum].xradius = buf2;
                    break;

                case YRADIUS:
                    screen[screennum].yradius = buf2;
                    break;

                case STARTANGLE:
                    screen[screennum].startangle = buf2;
                    break;

                case ENDANGLE:
                    screen[screennum].endangle = buf2;
                    break;

                case R:
                    screen[screennum].r = buf2;
                    break;

                case G:
                    screen[screennum].g = buf2;
                    break;

                case B:
                    screen[screennum].b = buf2;
                    break;

                case R_BG:
                    screen[screennum].r_bg = buf2;
                    break;

                case G_BG:
                    screen[screennum].g_bg = buf2;
                    break;

                case B_BG:
                    screen[screennum].b_bg = buf2;
                    break;

                case ALPHA:
                    screen[screennum].alpha = buf2;

                case FONTWIDTH:
                    screen[screennum].font_bitmap.width = buf2;
                    break;

                case FONTHEIGHT:
                    screen[screennum].font_bitmap.height = buf2;
                    break;

                case FONTSIZE:
                    screen[screennum].font_ttf.size = buf2;
                    break;

                case FONTSTYLE:
                    switch (buf2)
                    {
                        case FONTSTYLE_NORMAL:
                            /* set normal style. reset other styles */

                            screen[screennum].font_ttf.style_normal = TRUE;
                            screen[screennum].font_ttf.style_bold = FALSE;
                            screen[screennum].font_ttf.style_italic = FALSE;
                            screen[screennum].font_ttf.style_underline = FALSE;
                            break;

                        case FONTSTYLE_BOLD:
                            screen[screennum].font_ttf.style_bold = TRUE;
                            break;

                        case FONTSTYLE_ITALIC:
                            screen[screennum].font_ttf.style_italic = TRUE;
                            break;

                        case FONTSTYLE_UNDERLINE:
                            screen[screennum].font_ttf.style_underline = TRUE;
                            break;
                    }
                    break;

                case GADGET:
                    screen[screennum].gadget_index = buf2;
                    break;

                case GADGETS:
                    /* allocate space for gadgets */

                    init_gadgets (screennum, buf2);
                    break;

                case GADGET_STATUS:
                    screen[screennum].gadget_status = buf2;
                    break;

                case GADGET_INT_VALUE:
                    screen[screennum].gadget_int_value = buf2;
                    break;

                case GADGET_CYCLE_ENTRIES:
                    /* allocate space for menu text array */

                    if (screen[screennum].gadget_cycle_text)
                    {
                        dealloc_array_U1 (screen[screennum].gadget_cycle_text, screen[screennum].gadget_cycle_menu_entries);
                    }

                    screen[screennum].gadget_cycle_text = (U1 **) malloc (buf2 * sizeof (screen[screennum].gadget_cycle_text));
                    if (screen[screennum].gadget_cycle_text == NULL)
                    {
                        printf ("error: can't allocate cycle gadget text!\n");
                    }
                    else
                    {
                        for (i = 0; i < buf2; i++)
                        {
                            screen[screennum].gadget_cycle_text[i] = NULL;
                        }
                    }

                    screen[screennum].gadget_cycle_menu_entries = buf2;
                    screen[screennum].gadget_cycle_text_index = 0;
                    break;

                case GADGET_STRING_STR_LEN:
                    screen[screennum].gadget_string_string_len = buf2;
                    break;

                case GADGET_STRING_VIS_LEN:
                    screen[screennum].gadget_string_visible_len = buf2;
                    break;

                case VX:
                    if (screen[screennum].vx_index < screen[screennum].vectors)
                    {
                        if (screen[screennum].vx)
                        {
                            screen[screennum].vx[screen[screennum].vx_index] = buf2;
                            screen[screennum].vx_index++;
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
                    if (screen[screennum].vy_index < screen[screennum].vectors)
                    {
                        if (screen[screennum].vy)
                        {
                            screen[screennum].vy[screen[screennum].vy_index] = buf2;
                            screen[screennum].vy_index++;
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
            if (! read_line_string (new_tcpsock, arg, 255))
            {
                printf ("error: can't read text!\n");
                endconn = TRUE;
                break;
            }

            switch (command)
            {
                case TEXT:
                    strcpy (screen[screennum].text, arg);
                    break;

                case SCREENTITLE:
                    strcpy (screen[screennum].title, arg);
                    break;

                case SCREENICON:
                    strcpy (screen[screennum].icon, arg);
                    break;

                case PICTURENAME:
                    strcpy (screen[screennum].picture_name, arg);
                    break;

                case FONTNAME:
                    strcpy (screen[screennum].fontname, arg);
                    break;

                case GADGET_STRING_VALUE:
                    strcpy (screen[screennum].gadget_string_value, arg);
                    break;

                case GADGET_CYCLE_TEXT:
                    /* add menu texts */

                    if (screen[screennum].gadget_cycle_text_index < screen[screennum].gadget_cycle_menu_entries)
                    {
                        if (screen[screennum].gadget_cycle_text)
                        {
                            screen[screennum].gadget_cycle_text[screen[screennum].gadget_cycle_text_index] = (U1 *) malloc ((strlen (arg) + 1) * sizeof (U1));
                            if (screen[screennum].gadget_cycle_text[screen[screennum].gadget_cycle_text_index])
                            {
                                strcpy (screen[screennum].gadget_cycle_text[screen[screennum].gadget_cycle_text_index], arg);
                                screen[screennum].gadget_cycle_text_index++;
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

        if (command >= SOUND_WAV_FILE && command <= SOUND_MUSIC_FILE)
        {
            switch (command)
            {
                case SOUND_WAV_FILE:
                    if (! read_line_string (new_tcpsock, arg, 255))
                    {
                        printf ("error: can't read soundfile name!\n");
                        endconn = TRUE;
                        break;
                    }

                    strcpy (screen[screennum].sound.wav_filename, arg);
                    break;

                case SOUND_CHANNEL:
                    if (! read_16 (new_tcpsock, &buf2))
                    {
                        printf ("error: can't read data!\n");
                        break;
                    }

                    screen[screennum].sound.channel = buf2;
                    break;

                case SOUND_LOOPS:
                    if (! read_16 (new_tcpsock, &buf2))
                    {
                        printf ("error: can't read data!\n");
                        break;
                    }

                    screen[screennum].sound.loops = buf2;
                    break;

                case SOUND_MUSIC_FILE:
                    if (! read_line_string (new_tcpsock, arg, 255))
                    {
                        printf ("error: can't read musicfile name!\n");
                        endconn = TRUE;
                        break;
                    }

                    strcpy (screen[screennum].sound.music_filename, arg);
                    break;
			}
		}
		
		if (command >= RS232_COMPORT_NUMBER && command <= GADGET_PROGRESS_BAR_VALUE)
		{
			switch (command)
			{		
				case RS232_COMPORT_NUMBER:
					if (! read_16 (new_tcpsock, &buf2))
					{
						printf ("error: can't read data!\n");
                        break;
                    }
                    
                    rs232.comport = buf2;
					break;
					
				case RS232_BAUDRATE:
					if (! read_16 (new_tcpsock, &buf2))
					{
						printf ("error: can't read data!\n");
                        break;
                    }
                    
                    rs232.baudrate = buf2;
					break;
                    
				case RS232_BUF_SIZE:
					if (! read_16 (new_tcpsock, &buf2))
					{
						printf ("error: can't read data!\n");
                        break;
                    }
                    
                    rs232.bufsize = buf2;
					break;
					
				case RS232_BYTE:
					if (! read_8 (new_tcpsock, &buf2))
					{
						printf ("error: can't read data!\n");
                        break;
                    }
                    rs232.buf[0] = buf2;
					break;
					
				case RS232_BUF:
					for (i = 0; i < rs232.bufsize; i++)
					{
						if (! read_8 (new_tcpsock, &buf2))
						{
							printf ("error: can't read data!\n");
							break;
						}
						rs232.buf[i] = buf2;
					}
					break;
					
				case RPI_GPIO_PIN:
					if (! read_16 (new_tcpsock, &buf2))
					{
						printf ("error: can't read data!\n");
						break;
					}
					
					rpi.pin = buf2;
					break;
					
				case RPI_GPIO_VALUE:
					if (! read_16 (new_tcpsock, &buf2))
					{
						printf ("error: can't read data!\n");
						break;
					}
					
					rpi.value = buf2;
					break;
					
				case GADGET_PROGRESS_BAR_VALUE:
					if (! read_16 (new_tcpsock, &buf2))
					{
						printf ("error: can't read data!\n");
                        break;
                    }
                    
					screen[screennum].gadget_progress_value = buf2;
					break;
            }
        }
    }
    return (ret_command);
}


S2 start_menu_event (void)
{
	S2 screennum = 0;
    SDL_Event event;
    U1 wait, cycle_menu;
    Uint8 buttonmask, mouse_button, double_click, select;
    U2 gadget, i;
    S4 value, old_value;
    int x, y;

	U1 resized = 0;
	
    struct gadget_button *button;

    if (! screen[screennum].gadget)
    {
        printf ("gadget_event: error gadget list not allocated!\n");
        return;
    }

    printf ("waiting for menu choice...\n");
    
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

            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == 1)
                    {
						printf ("MOUSEBUTTON PRESSED\n");
						
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
				
								reopen_screen (0, event.resize.w, event.resize.h);
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
                                        return (-1);
                                    }
                                }
                                else
                                {
                                    printf ("gadget_event: drawing button %i normal.\n", i);

                                    if (! draw_gadget_button (screennum, i, GADGET_NOT_SELECTED))
                                    {
                                        printf ("gadget_event: error can't draw gadget!\n");
                                        return (-1);
                                    }
                                }
                            }

                            if (! draw_gadget_button (screennum, i, GADGET_NOT_SELECTED))
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
	
	screen[0].r = 0;
	screen[0].g = 0;
	screen[0].b = 0;
	
	clear_screen (0);
	
	
	/* load 260 x 244 splash screen logo (black/white) */
	load_scan ("/sdcard/nanovm/nano-android-splash.scn", screen[0].bmap);
	update_screen (0);
	
	/* wait 5 secs */
	
	SDL_Delay (5000);
	
	
	/* read startmenu.txt */
	
#if ! (__ANDROID__)
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

	init_gadgets (0, 10);
	
	
	// set colors

	screen[0].gadget_color.backgr_light.r = 232;
	screen[0].gadget_color.backgr_light.g = 239;
	screen[0].gadget_color.backgr_light.b = 247;
	
	screen[0].gadget_color.backgr_shadow.r = 194;
	screen[0].gadget_color.backgr_shadow.g = 201;
    screen[0].gadget_color.backgr_shadow.b = 209;
    
	screen[0].gadget_color.border_light.r = 143;
	screen[0].gadget_color.border_light.g = 147;
	screen[0].gadget_color.border_light.b = 151;
	
	screen[0].gadget_color.border_shadow.r = 85;
	screen[0].gadget_color.border_shadow.r = 87;
    screen[0].gadget_color.border_shadow.r = 90;
    
    screen[0].gadget_color.text_light.r = 0;
	screen[0].gadget_color.text_light.g = 0;
	screen[0].gadget_color.text_light.b = 0;
	
    screen[0].gadget_color.text_shadow.r = 255;
	screen[0].gadget_color.text_shadow.g = 255;
	screen[0].gadget_color.text_shadow.b = 255;
	
	
	// load font
	
	strcpy (screen[0].fontname, "truetype/freefont/FreeMono.ttf");
	screen[0].font_ttf.size = 25;
	
	if (load_font_ttf (0) == FALSE)
	{
		printf ("ERROR: can't load font!\n");
		return (1);
	}
	
	// set background color
	
	screen[0].r = 232;
	screen[0].g = 239;
	screen[0].b = 247;
	
	clear_screen (0);
	
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
	
	
	strcpy (menu[i], "EXIT"); i++;
	strcpy (menu[i], "connect remote VM...");
	
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
		screen[0].x = x;
		screen[0].y = y;
		screen[0].gadget_status = 1;
		screen[0].gadget_index = g;
		
		strcpy (screen[0].text, menu[g]);
		
		set_gadget_button (0);
		
		y = y + 45;
	}
	
	update_screen (0);
	
	gadget = start_menu_event ();
	
	return (gadget);
}


S2 get_server_ip (U1 *ip)
{
	Uint8 request[256];
	SDL_Event event;
    SDLKey key;
	Uint8 wait;
	Sint16 ip_len;
	Uint8 ip_str[80];
	
	strcpy (ip_str, "192.168.1.1");
	strcpy (request, "enter client ip: ");
	strcat (request, ip_str);
	ip_len = strlen (ip_str);
	
	
	draw_text_ttf (0, request, 50, 50, 255, 255, 255);
	update_screen (0);
	
	SDL_Delay (4000);
	
#if __ANDROID__
	/* show on screen keyboard */
	SDL_ANDROID_GetScreenKeyboardTextInput (ip_str, 79);
	
	strcpy (request, "server ip: ");
	strcat (request, ip_str);
		
	boxRGBA (screen[0].bmap, 0, 0, screen[0].width - 1, screen[0].height - 1, 0, 0, 0, 255);
	draw_text_ttf (0, request, 50, 50, 255, 255, 255);
	update_screen (0);
	
	SDL_Delay (4000);
	
	strcpy (ip, ip_str);
	return (0);
}
	
#else
	
	wait = TRUE;

    while (wait)
    {
		SDL_Delay (50);
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					key = event.key.keysym.sym;
					switch (key)
					{
						case SDLK_BACKSPACE:
							printf ("BACKSPACE\n");

							ip_len = strlen (ip_str);
							if (ip_len > 0)
							{
								ip_str[ip_len - 1] = '\0';
							}
							break;
						
						case SDLK_RETURN:
							wait = FALSE;
							break;
						
						default:
							ip_len = strlen (ip_str);
							if (ip_len < 256)
							{
								ip_str[ip_len] = event.key.keysym.unicode;
								ip_str[ip_len + 1] = '\0';
							}
							break;
					}
					break;
			}
		}
		
		strcpy (request, "enter server ip: ");
		strcat (request, ip_str);
		
		boxRGBA (screen[0].bmap, 0, 0, screen[0].width - 1, screen[0].height - 1, 0, 0, 0, 255);
		draw_text_ttf (0, request, 50, 50, 255, 255, 255);
		update_screen (0);
	}
	
	strcpy (ip, ip_str);
	return (0);
}		

#endif


	


/* read_command () neu ENDE ------------------------------------------- */

 #ifdef _WIN32
 //int WinMain(int ac, char * av[]) {
 int main (int ac, char *av[]) {
 #else ifdef _LINUX || ifdef __ANDROID__
 int main (int ac, char *av[]) {
 #endif

    const SDL_VideoInfo *info;
    Uint8 video_bpp;
    U1 run = TRUE;
    S2 command;
	U2 port;
    SDL_Event event;
	U1 wait_for_screen = 0;
	
	int x = 10, y = 10;
	int show_start_menu = FALSE;
	
	/* native sockets stuff */
	int yes = 1;
    S2 server, error;
    struct sockaddr_in address;
    S4 i, socketh = -1;

    struct sockaddr_in client;
    socklen_t len = sizeof (client);
	
	/* Android start menu */
	S2 program = 0;
	U1 command_shell[256];
	pid_t pID;
	
    printf ("flow 0.6.3\n");

	
    if (ac < 2)
    {
       printf ("flow <port> || <-menu>\n");
       exit (1);
    }

    if (strcmp (av[1], "-h") == 0)
    {
       printf ("flow <port> || <-menu>\n");
       exit (1);
    }

    if (strcmp (av[1], "-menu") == 0)
	{
		/* user wants the start menu, set port to 2000 */
		port = 2000;
		show_start_menu = TRUE;
	}
	else
	{
		port = atoi (av[1]);    /* get port */
	}
	
    /* initialize copy bitmap */
    bmap_copy = NULL;

    init_screens ();

    printf ("Initializing SDL.\n");

    /* Initialize the SDL library */

    /* Same setup as before */
    int audio_rate = 44100;
    Uint16 audio_format = AUDIO_S16; 
    int audio_channels = 2;
    int audio_buffers = 4096;

#if WITH_SOUND
    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        fprintf (stderr, "Couldn't initialize SDL: %s\n", SDL_GetError ());
#if __ANDROID__
		LOGE("SDL init error!\n");
#endif
        exit (1);
    }
    
#else

	if (SDL_Init (SDL_INIT_VIDEO) < 0)
    {
        fprintf (stderr, "Couldn't initialize SDL: %s\n", SDL_GetError ());
#if __ANDROID__
		LOGE("SDL init error!\n");
#endif
        exit (1);
    }
#endif


#if WITH_SOUND
    if (Mix_OpenAudio (audio_rate, audio_format, audio_channels, audio_buffers))
    {
        fprintf (stderr, "SDL Unable to open audio!\n");
#if __ANDROID__
		LOGE("SDL no audio!\n");
#endif
        exit(1);
    }
#endif
    
	if (show_start_menu == TRUE)
	{
		screen[0].width = 640;
		screen[0].height = 480;
	
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

		if (! open_new_screen (0, 16))
		{
			printf ("openscreen error!\n");

#if __ANDROID__
			LOGE("openscreen error!\n");
#endif
			
			exit (1);
		}
	}

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

    /* key input settings */

    SDL_EnableUNICODE (SDL_ENABLE);
    SDL_EnableKeyRepeat (500, 125);

	// mouse_cursor = init_system_cursor (&arrow);
	// SDL_SetCursor (mouse_cursor);
	
	// set_simple_cursor ();
	
    printf ("SDL initialized.\n");

    /* Initialize the TTF library */

    if (TTF_Init () < 0)
    {
        fprintf (stderr, "Couldn't initialize TTF: %s\n", SDL_GetError ());
#if __ANDROID__
	LOGE("SDL Unable to open TTF!");
#endif	
        exit (1);
    }

    
    /* Clean up on exit */
    atexit (SDL_Quit);

	if (show_start_menu == TRUE)
	{
		/* run the program menu: launch Nano VM */
	
		program = start_menu ();

		/* check for EXIT gadget selected */
		if (program == 0)
		{
			exit (0);
		}
	
		if (program == 1)
		{
			// user want to connect to remote VM, ask IP of the Nano VM
			
			get_server_ip (ip);
		}
		else
		{
			draw_text_ttf (0, "starting Nano VM...", 50, 400, 0, 0, 0);
	
			// strcpy (command_shell, "/data/data/jackpal.androidterm/kbox2/bin/nanovm ");
			strcpy (command_shell, nanovm_path);
			strcat (command_shell, " ");
			strcat (command_shell, menu[program]);
			strcat (command_shell, " 127.0.0.1 2000 &");
		}
	}
		
#if SOCKETS_NATIVE
	printf ("starting native socket...\n");

	if (program != 1)
	{
		// Nano VM runs on this machine, use localhost for connection
		strcpy (ip, "127.0.0.1");
	}
		
	//exe_gethostbyaddr (ip, &addr);
	
#if _WIN32	
	if (! init_winsock ())
	{
		printf ("ERROR: can't init winsock!\n");
		exit (1);
	}
#endif
	
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

    if (SDLNet_ResolveHost (&ip, NULL, port) == -1)
    {
        printf ("SDLNet_ResolveHost: %s\n", SDLNet_GetError ());
#if __ANDROID__
	LOGE("SDL Unable to open SDL_Net_ResolveHost!");
#endif	
        exit (1);
    }

    tcpsock = SDLNet_TCP_Open (&ip);
    if (!tcpsock)
    {
        printf ("SDLNet_TCP_Open: %s\n", SDLNet_GetError ());
#if __ANDROID__
	LOGE("SDL Unable to SDLNet_TCP_Open!");
#endif	
        exit (1);
    }

#endif   
    
    
	if (show_start_menu == TRUE)
	{
#if __ANDROID__
    LOGD("waiting...\n");
#endif
		
		system (command_shell);
	
		draw_text_ttf (0, "waiting for connection...", 50, 440, 0, 0, 0);
	}
	
	
    while (run)
    {
        printf ("waiting...........\n");

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

			#if __ANDROID__
				LOGD("reading data...\n");
			#endif
			
            command = read_command ();
            if (command > 0);
            {
                switch (command)
                {
                    case OPEN_SCREEN:
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

						if (show_start_menu == FALSE)
						{
							/* no screen open yet, open new screen */
							
							if (! open_new_screen (screennum, video_bpp))
							{
								send_8 (new_tcpsock, ERROR);

								printf ("openscreen error!\n");
								exit (1);
							}
							else
							{
								send_8 (new_tcpsock, OK);

								printf ("sended open screen OK.\n");
							}
						}
						else
						{
							/* ANDROID or start menu set: screen already opened on program start: */
							send_8 (new_tcpsock, OK); /* just send ok: screen is open */
						}
						break;

                    case CLOSE_SCREEN:
                        if (! close_screen (screennum))
                        {
                            printf ("closescreen error!\n");
                            exit (1);
                        }
                        break;

                    case SHUTDOWN:
                        send_8 (new_tcpsock, OK);

                        printf ("shutdown...\n");

                        run = FALSE;
                        break;
                }
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
#else
	close (server);
	CLOSEWINSOCK();
#endif
	
    TTF_Quit ();
    SDL_Quit ();
	
    printf ("Quiting...\n");
    exit (0);
}

