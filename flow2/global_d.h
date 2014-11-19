/****************************************************************************
*
* Filename: global_d.h
*
* Author:   Stefan Pietzonke
* Project:  flow, the gfx plugin for nano VM
*
* Purpose:  global definitions
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

/* use normal type sockets instead of SDL_Net if set to 1 */
#define SOCKETS_NATIVE	1


/* set to 1 to show Android start menu on other OS */
#define START_MENU	0

/* set this to 1 on ANDROID: doubleclick to select */

#if __ANDROID__
#define GADGET_CYCLE_DOUBLECLICK	1
#else
#define GADGET_CYCLE_DOUBLECLICK	0
#endif


/* set to one for one client or zero for MAXAPPS */

#define SINGLE_APP_MODE 0

#if SINGLE_APP_MODE
	#define MAXAPPS	    1
#else
	#define MAXAPPS		256
#endif


#define WITH_SOUND		0

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>            /* for POSIX threads */

#if __ANDROID__

#include <SDL.h>
#include <SDL_byteorder.h>
#include <SDL_net.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL2_ttf.h>
#include <SDL_image.h>

#if WITH_SOUND
	#include <SDL_mixer.h>
#endif

#else

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#if WITH_SOUND
	#include <SDL2/SDL_mixer.h>
#endif

#endif

#include <errno.h>
#include <string.h>

#define TRUE                1
#define FALSE               0
#define BINUL               '\0'


#define BACKLOG             1                       /* number of pending connections */




#define MAXSCREEN           8
#define BLITSCREEN          MAXSCREEN - 1
#define OPEN                1
#define CLOSED              0


#define MAXGADGET           256


/* commands */

#define END                                 1
#define END_SB                              "*end"                  /* end connection to server */

#define SHUTDOWN                            2
#define SHUTDOWN_SB                         "*shutdown"             /* shutdown server */

#define OPEN_SCREEN                         3
#define OPEN_SCREEN_SB                      "*openscreen"

#define CLOSE_SCREEN                        4
#define CLOSE_SCREEN_SB                     "*closescreen"

#define CLEAR_SCREEN                        5
#define CLEAR_SCREEN_SB                     "*clearscreen"

#define UPDATE_SCREEN                       6
#define UPDATE_SCREEN_SB                    "*updatescreen"

#define COLOR                               7
#define COLOR_SB                            "*color"

#define PIXEL                               8
#define PIXEL_SB                            "*pixel"

#define LINE                                9
#define LINE_SB                             "*line"

#define RECTANGLE                           10
#define RECTANGLE_SB                        "*rectangle"

#define RECTANGLE_FILL                      11
#define RECTANGLE_FILL_SB                   "*rectangle-f"

#define CIRCLE                              12
#define CIRCLE_SB                           "*circle"

#define CIRCLE_FILL                         13
#define CIRCLE_FILL_SB                      "*circle-f"

#define ELLIPSE                             14
#define ELLIPSE_SB                          "*ellipse"

#define ELLIPSE_FILL                        15
#define ELLIPSE_FILL_SB                     "*ellipse-f"

#define PIE                                 16
#define PIE_SB                              "*pie"

#define PIE_FILL                            17
#define PIE_FILL_SB                         "*pie-f"

#define TRIGON                              18
#define TRIGON_SB                           "*trigon"

#define TRIGON_FILL                         19
#define TRIGON_FILL_SB                      "*trigon-f"

#define POLYGON                             20
#define POLYGON_SB                          "*polygon"

#define POLYGON_FILL                        21
#define POLYGON_FILL_SB                     "*polygon-f"

#define BEZIER                              22
#define BEZIER_SB                           "*bezier"

#define TEXT_BMAP                           23
#define TEXT_BMAP_SB                        "*text-bmap"

#define LOADFONT_BMAP                       24
#define LOADFONT_BMAP_SB                    "*loadfont-bmap"

#define TEXT_TTF                            25
#define TEXT_TTF_SB                         "*text-ttf"

#define LOADFONT_TTF                        26
#define LOADFONT_TTF_SB                     "*loadfont-ttf"

#define LOAD_PICTURE                        27
#define LOAD_PICTURE_SB                     "*load-picture"

#define GETPIXEL                            28
#define GETPIXEL_SB                         "*getpixel"

#define GETMOUSE                            29
#define GETMOUSE_SB                         "*getmouse"

#define GADGET_COLOR_BORDER_LIGHT           30
#define GADGET_COLOR_BORDER_LIGHT_SB        "*gadget-color-border-light"

#define GADGET_COLOR_BORDER_SHADOW          31
#define GADGET_COLOR_BORDER_SHADOW_SB       "*gadget-color-border-shadow"

#define GADGET_COLOR_BACKGR_LIGHT           32
#define GADGET_COLOR_BACKGR_LIGHT_SB        "*gadget-color-backgr-light"

#define GADGET_COLOR_BACKGR_SHADOW          33
#define GADGET_COLOR_BACKGR_SHADOW_SB       "*gadget-color-backgr-shadow"

#define GADGET_COLOR_TEXT_LIGHT             34
#define GADGET_COLOR_TEXT_LIGHT_SB          "*gadget-color-text-light"

#define GADGET_COLOR_TEXT_SHADOW            35
#define GADGET_COLOR_TEXT_SHADOW_SB         "*gadget-color-text-shadow"

#define GADGET_BUTTON                       36
#define GADGET_BUTTON_SB                    "*gadget-button"

#define GADGET_CHECKBOX                     37
#define GADGET_CHECKBOX_SB                  "*gadget-checkbox"

#define GADGET_CYCLE                        38
#define GADGET_CYCLE_SB                     "*gadget-cycle"

#define GADGET_STRING                       39
#define GADGET_STRING_SB                    "*gadget-string"

#define GADGET_BOX                          40
#define GADGET_PROGRESS_BAR					41

#define GADGET_EVENT                        42
#define GADGET_EVENT_SB                     "*gadget-event"

#define GADGET_GET_X2Y2                     43
#define GADGET_GET_X2Y2_SB                  "*gadget-get-x2y2"

#define GADGET_CHECKBOX_CHANGE              44
#define GADGET_CHECKBOX_CHANGE_SB           "*gadget-checkbox-change"

#define GADGET_STRING_CHANGE                45
#define GADGET_STRING_CHANGE_SB             "*gadget-string-change"

#define GADGET_BOX_CHANGE                   46
#define GADGET_PROGRESS_BAR_CHANGE			47

#define SOUND_PLAY_WAV                      48
#define SOUND_STOP_CHANNEL                  49

#define SOUND_PLAY_MUSIC                    50
#define SOUND_STOP_MUSIC                    51

 
#define RS232_OPEN_COMPORT					52
#define RS232_POLL_COMPORT					53
#define RS232_SEND_BYTE						54
#define RS232_SEND_BUF						55
#define RS232_CLOSE_COMPORT					56

/* types */

#define SCREENNUM                           57
#define WIDTH                               58
#define HEIGHT                              59
#define BIT                                 60

#define X                                   61
#define Y                                   62
#define X2                                  63
#define Y2                                  64
#define X3                                  65
#define Y3                                  66

#define VECTORS                             67
#define STEPS                               68
#define RADIUS                              69
#define XRADIUS                             70
#define YRADIUS                             71
#define STARTANGLE                          72
#define ENDANGLE                            73


/* normal colors */

#define R                                   74
#define G                                   75
#define B                                   76


/* background colors (unused) */

#define R_BG                                77
#define G_BG                                78
#define B_BG                                79


/* alpha channel */

#define ALPHA                               80


#define FONTWIDTH                           81
#define FONTHEIGHT                          82
#define FONTSIZE                            83
#define FONTSTYLE                           84

/* true type font styles */

#define FONTSTYLE_NORMAL                    85
#define FONTSTYLE_BOLD                      86
#define FONTSTYLE_ITALIC                    87
#define FONTSTYLE_UNDERLINE                 88


#define GADGET                              89
#define GADGETS                             90
#define GADGET_STATUS                       91
#define GADGET_INT_VALUE                    92
#define GADGET_CYCLE_ENTRIES                93
#define GADGET_STRING_STR_LEN               94
#define GADGET_STRING_VIS_LEN               95


/* vectors (x, y pairs) */

#define VX                                  96
#define VY                                  97

/* string data */

#define TEXT                                98
#define SCREENTITLE                         99
#define SCREENICON                          100
#define PICTURENAME                         101
#define FONTNAME                            102
#define GADGET_STRING_VALUE                 103
#define GADGET_CYCLE_TEXT                   104

/* sound data */

#define SOUND_WAV_FILE                      105
#define SOUND_CHANNEL                       106
#define SOUND_LOOPS                         107
#define SOUND_MUSIC_FILE                    108

/* RS232 data */

#define RS232_COMPORT_NUMBER				109
#define RS232_BAUDRATE						110
#define RS232_BUF_SIZE						111
#define RS232_BYTE							112
#define RS232_BUF							113

/* progress bar */

#define GADGET_PROGRESS_BAR_VALUE			114


#define OK                                  0
#define ERROR                               1


/* gadget event stuff */

#define GADGET_NOT_SELECTED 0
#define GADGET_SELECTED     1
#define GADGET_MENU_DOWN    2
#define GADGET_MENU_UP      3

/* gadget status */

#define GADGET_NOT_ACTIVE   0
#define GADGET_ACTIVE       1

/* checkbox value */

#define GADGET_CHECKBOX_FALSE   0
#define GADGET_CHECKBOX_TRUE    1


/* font path env variable */

#define FONT_DIR_SB                         "NANOGFXFONT"
#define SOUND_DIR_SB                        "NANOSFXSOUND"

#define NANOVM_ROOT_SB						"NANOVM_ROOT"

typedef unsigned char   U1;     /* UBYTE   */
typedef signed short    S2;     /* INT     */
typedef unsigned short  U2;     /* UINT    */
typedef signed int      S4;     /* LONGINT */
typedef double          F8;     /* DOUBLE  */
typedef int             NINT;


/* fonts -------------------------------------------------- */

struct font_bitmap
{
    Sint16 width;
    Sint16 height;
    U1 *buf;                            /* buffer for font data */
};

struct font_ttf
{
    TTF_Font *font;
    U2 size;
    U1 style_normal;
    U1 style_bold;
    U1 style_italic;
    U1 style_underline;
};

/* gadgets ------------------------------------------------ */

struct gadget_button
{
    U1 *text;
    U1 status;
    Sint16 x;
    Sint16 y;
    Sint16 x2;
    Sint16 y2;
    Sint16 text_x;
    Sint16 text_y;
};

struct gadget_progress_bar
{
    U1 *text;
    U1 status;
    Sint16 x;
    Sint16 y;
    Sint16 x2;
    Sint16 y2;
    Sint16 text_x;
    Sint16 text_y;
	U2 value;
};

struct gadget_checkbox
{
    U1 *text;
    U1 status;
    U1 value;
    Sint16 x;
    Sint16 y;
    Sint16 x2;
    Sint16 y2;
    Sint16 text_x;
    Sint16 text_y;
    Sint16 text_x2;
    Sint16 text_y2;
};

struct gadget_cycle
{
    U1 menu;
    U1 **text;
    U1 status;
    U1 value;
    U2 menu_entries;
    Sint16 x;
    Sint16 y;
    Sint16 x2;
    Sint16 y2;
    Sint16 text_x;
    Sint16 text_y;
    Sint16 text_height;
    Sint16 menu_x;
    Sint16 menu_y;
    Sint16 menu_x2;
    Sint16 menu_y2;
    Sint16 arrow_x;
    Sint16 arrow_y;
};

struct gadget_string
{
    U1 *text;                           /* info text */
    U1 *value;                          /* input string */
    U1 *display;                        /* display-buffer string */
    U1 status;
    U2 string_len;                      /* max input string length */
    U2 visible_len;                     /* visible chars of input string */
    Sint16 x;
    Sint16 y;
    Sint16 x2;
    Sint16 y2;
    Sint16 text_x;                      /* gadget info text */
    Sint16 text_y;
    Sint16 text_x2;
    Sint16 text_y2;
    Sint16 input_x;                     /* gadget entry text */
    Sint16 input_y;
    Sint16 cursor_width;
    Sint16 cursor_height;
    S2 cursor_pos;                      /* cursor position */
    S2 insert_pos;                      /* text insert position */
};

struct gadget_box
{
    U1 status;
    Sint16 x;
    Sint16 y;
    Sint16 x2;
    Sint16 y2;
};

struct gadget
{
    void *gptr;                         /* pointer to gadget structure */
    U1 type;                            /* gadget type */
};

struct color
{
    Uint8 r;                            /* colors */
    Uint8 g;
    Uint8 b;
};

struct gadget_color
{
    struct color border_light;
    struct color border_shadow;
    struct color backgr_light;
    struct color backgr_shadow;
    struct color text_light;
    struct color text_shadow;
};


struct sound
{
    U1 wav_filename[512];
    U1 music_filename[512];
    S2 channel;
    S2 loops;
	
#if WITH_SOUND
    Mix_Music *music;
#else
	int *music;
#endif
};

/* -------------------------------------------------------- */

struct screen
{
	SDL_Window *window;
	SDL_Surface *surface;
	SDL_Renderer *renderer;
	SDL_Surface *copy_surface;			/* backup surface for example menues pixel overdraw */
	SDL_Renderer *copy_renderer;
	SDL_Surface *temp_surface;			/* new surface for menu */
	SDL_Renderer *temp_renderer;
    Sint16 width;
    Sint16 height;
    Sint16 x;
    Sint16 y;
    Sint16 x2;
    Sint16 y2;
    Sint16 x3;
    Sint16 y3;
    Sint16 *vx;                         /* for polygon x coord */
    Sint16 *vy;                         /* for polygon y coord */
    U2 vectors;                         /* number of vectors (vx/vy pairs) */
    U2 vx_index;                        /* vector x index */
    U2 vy_index;                        /* vector y index */
    U2 steps;
    Sint16 radius;
    Sint16 xradius;
    Sint16 yradius;
    Sint16 startangle;
    Sint16 endangle;
    Uint8 r;                            /* normal colors */
    Uint8 g;
    Uint8 b;
    Uint8 r_bg;                         /* background colors (unused) */
    Uint8 g_bg;
    Uint8 b_bg;
    Uint8 alpha;
    Uint8 video_bpp;
    U1 status;
    U1 title[256];                      /* window title */
    U1 icon[256];                       /* icon name */
    U1 text[256];                       /* text to output */
    U1 fontname[512];                   /* font to load */
    U1 picture_name[512];               /* picture to load */
    

    struct sound sound;                 /* sound data */

    struct font_bitmap font_bitmap;
    struct font_ttf font_ttf;
    struct gadget *gadget;              /* gadget list */
    struct gadget_color gadget_color;   /* gadget colors */
    U2 gadgets;                         /* number of gadgets */
    U2 gadget_index;                    /* gadget index for setting gadget */
    U1 gadget_status;                   /* gadget status for new gadget */
    U1 gadget_string_value[256];        /* gadget string value */
    S4 gadget_int_value;                /* gadget integer value */
    U1 **gadget_cycle_text;             /* menu text */
    U2 gadget_cycle_menu_entries;       /* number of menu entries */
    U2 gadget_cycle_text_index;
    U2 gadget_string_string_len;        /* max input string length */
    U2 gadget_string_visible_len;       /* visible chars of input string */
   
	U2 gadget_progress_value;
};

struct rs232
{
	S4 comport;
	S4 baudrate;
	U1 buf[4096];
	S4 bufsize;
};



/* application data, for handling more than one client at a time */

struct app
{
	U1 in_use;
	pthread_t thread;
	S2 tcpsock;
	
	struct screen screen[MAXSCREEN];
	
	struct rs232 rs232;
};
