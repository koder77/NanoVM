#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>   /* All SDL App's need this */
#include <SDL/SDL_byteorder.h>

typedef unsigned char   U1;     /* UBYTE   */
typedef signed short    S2;     /* INT     */
typedef unsigned short  U2;     /* UINT    */
typedef signed long     S4;     /* LONGINT */
typedef double          F8;     /* DOUBLE  */
typedef int             NINT;


#define 				FALSE 0
#define 				TRUE 1
#define                 PWIDTH 640
#define                 PHEIGHT 480


#if SDL_BYTEORDER == SDL_LIL_ENDIAN

U2 conv_int (U2 num)
{
    U1 *num_ptr, *new_ptr;
    U2 new;

    num_ptr = (U1 *) &num;
    new_ptr = (U1 *) &new;

    new_ptr[0] = num_ptr[1];
    new_ptr[1] = num_ptr[0];

    return (new);
}

#else

U2 conv_int (U2 num)
{
    return (num);
}

#endif


/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void putpixel (SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *) p = pixel;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } 
            else 
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 * )p = pixel;
            break;
    }
}

U1 setpixel (SDL_Surface *surface, U2 x, U2 y, Uint32 color)
{
    /* Lock the screen for direct access to the pixels */
    if (SDL_MUSTLOCK (surface))
    {
        if (SDL_LockSurface (surface) < 0)
        {
            fprintf (stderr, "Can't lock screen: %s\n", SDL_GetError ());
            return (FALSE);
        }
    }

    putpixel (surface, x, y, color);

    if (SDL_MUSTLOCK (surface))
    {
        SDL_UnlockSurface (surface);
    }

    /* Update just the part of the display that we've changed */
    SDL_UpdateRect (surface, x, y, 1, 1);
    return (TRUE);
}

void clear_surface (SDL_Surface *surface, Uint32 color)
{
    SDL_Rect rect = {0, 0, surface->w, surface->h};
	SDL_FillRect (surface, &rect, color);
}

U1 load_scan (U1 *file, SDL_Surface *screen)
{
    Uint32 black, white, pixelcolor;
    S4 curr_pos, size;
    U2 x, y, xstart, ystart, width, height, color, pixel, i, pos;
    U1 bitmap[PWIDTH * PHEIGHT], buf[6], line_ok;

    FILE *scanf;
    
    if (! (scanf = fopen (file, "rb")))
    {
        fprintf (stderr, "Couldn't open scan-file %s\n", file);
        return (FALSE);
    }

    if (fread (buf, sizeof (U1), 5, scanf) != 5)
    {
    	fprintf (stderr, "Couldn't read from scan-file %s\n", file);
        fclose (scanf);
        return (FALSE);
    }
    buf[5] = '\0';

    if (strcmp (buf, "SCAN2") != 0)
    {
        fprintf (stderr, "Scan-file %s, wrong file format!\n", file);
        fclose (scanf);
        return (FALSE);
    }

    /* width, height */
    
    if (fread (&i, sizeof (U2), 1, scanf) != 1)
    {
        fprintf (stderr, "Couldn't read 'width' from scan-file %s\n", file);
        fclose (scanf);
        return (FALSE);
    }
    width = conv_int (i);
    
    if (fread (&i, sizeof (U2), 1, scanf) != 1)
    {
        fprintf (stderr, "Couldn't read 'height' from scan-file %s\n", file);
        fclose (scanf);
        return (FALSE);
    }
    height = conv_int (i);


    curr_pos = ftell (scanf);                    /* save current position */

    fseek (scanf, (long) NULL, SEEK_END);
    size = ftell (scanf);                        /* get filesize */

    fseek (scanf, curr_pos, SEEK_SET);          /* restore position */


    size = size - 9;

    /* read bitmap */
    if (fread (bitmap, sizeof (U1), size, scanf) != size)
    {
        fprintf (stderr, "Couldn't read 'bitmap' from scan-file %s\n", file);
        fclose (scanf);
        return (FALSE);
    }
    fclose (scanf);

    printf ("Picture size %i x %i\n", width, height);

    /* set colors */
    black = SDL_MapRGB (screen->format, 0, 0, 0);
    white = SDL_MapRGB (screen->format, 255, 255, 255);

    if (width < PWIDTH)
    {
    	xstart = (PWIDTH - width) / 2;
    }
    else
    {
    	xstart = 0;
    }

    if (height < PHEIGHT)
    {
    	ystart = (PHEIGHT - height) / 2;
    }
    else
    {
    	ystart = 0;
    }

    clear_surface (screen, white);

    pos = 0;
    for (y = ystart; y <= ystart + height - 1; y++)
    {
        printf ("y: %i\n", y);

        x = xstart;
        line_ok = FALSE;

        while (! line_ok)
        {
            printf ("pos: %i\n", pos);

        	if (bitmap[pos] > 1)
        	{
                pixel = bitmap[pos] / 2;
                color = bitmap[pos] - (pixel * 2);
            }
            else
            {
            	pixel = 1;
            	color = bitmap[pos];
            }
            
            if (color == 0)
            {
            	pixelcolor = black;
            }
            else
            {
            	pixelcolor = white;
            }

            if (x + pixel - 1 <= xstart + width - 1)
            {
                for (i = 1; i <= pixel; i++)
                {
            	   if (! setpixel (screen, x, y, pixelcolor))
            	   {
            		  return (FALSE);
            	   }
            	   x++;
                }

                pos++;
            }
            else
            {
            	line_ok = TRUE;
            }
        }
    }
    return (TRUE);
}