/****************************************************************************
*
* Filename: sound.c
*
* Author:   Stefan Pietzonke
* Project:  flow, the gfx plugin for nano VM
*
* Purpose:  sound functions
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

#if WITH_SOUND
S2 play_wav (S2 screennum)
{
    U1 *wavname;
    S2 wavname_len;

    Mix_Chunk *sound = NULL;

    /* check for sound path env variable */

    if (getenv (SOUND_DIR_SB) != NULL)
    {
        /* env variable set */

        wavname_len = strlen (getenv (SOUND_DIR_SB));
        wavname_len = wavname_len + strlen (screen[screennum].sound.wav_filename) + 1;

        /* allocate buffer for wavname */

        wavname = (U1 *) malloc (wavname_len * sizeof (U1));
        if (wavname == NULL)
        {
            printf ("play_wav: can't allocate %i bytes for wavname!\n", wavname_len);
            return (FALSE);
        }

        strcpy (wavname, getenv (SOUND_DIR_SB));
        strcat (wavname, screen[screennum].sound.wav_filename);
    }
    else
    {
        /* env variable not set */

        printf ("play_wav: env variable  NANOSFXSOUND not set!\n");

        wavname_len = strlen (screen[screennum].sound.wav_filename) + 1;

        /* allocate buffer for wavname */

        wavname = (U1 *) malloc (wavname_len * sizeof (U1));
        if (wavname == NULL)
        {
            printf ("play_wav: can't allocate %i bytes for wavname!\n", wavname_len);
            return (FALSE);
        }

        strcpy (wavname, screen[screennum].sound.wav_filename);
    }

    printf ("DEBUG: play_wav: %s\n", screen[screennum].sound.wav_filename);
    printf ("DEBUG: play_wav: channel: %i, loops: %i\n", screen[screennum].sound.channel, screen[screennum].sound.loops);

    sound = Mix_LoadWAV (wavname);
    Mix_PlayChannel (screen[screennum].sound.channel, sound, screen[screennum].sound.loops);

    free (wavname);
    return (TRUE);
}

S2 play_music (S2 screennum)
{
    U1 *musicname;
    S2 musicname_len;

    Mix_Music *music = NULL;

    if (getenv (SOUND_DIR_SB) != NULL)
    {
        /* env variable set */

        musicname_len = strlen (getenv (SOUND_DIR_SB));
        musicname_len = musicname_len + strlen (screen[screennum].sound.music_filename) + 1;

        /* allocate buffer for musicname */

        musicname = (U1 *) malloc (musicname_len * sizeof (U1));
        if (musicname == NULL)
        {
            printf ("play_music: can't allocate %i bytes for musicname!\n", musicname_len);
            return (FALSE);
        }

        strcpy (musicname, getenv (SOUND_DIR_SB));
        strcat (musicname, screen[screennum].sound.music_filename);
    }
    else
    {
        /* env variable not set */

        printf ("play_music: env variable  NANOSFXSOUND not set!\n");

        musicname_len = strlen (screen[screennum].sound.music_filename) + 1;

        /* allocate buffer for wavname */

        musicname = (U1 *) malloc (musicname_len * sizeof (U1));
        if (musicname == NULL)
        {
            printf ("play_music: can't allocate %i bytes for musicname!\n", musicname_len);
            return (FALSE);
        }

        strcpy (musicname, screen[screennum].sound.music_filename);
    }

    printf ("play_music: file: %s\n", screen[screennum].sound.music_filename);

    music = Mix_LoadMUS (musicname);

    Mix_PlayMusic (music, 0);

    screen[screennum].sound.music = music;
    free (musicname);

    return (TRUE);
}

void stop_music (S2 screennum)
{
    Mix_HaltMusic ();

    Mix_FreeMusic (screen[screennum].sound.music);
}

void stop_sound (S2 screennum)
{
    Mix_HaltChannel (screen[screennum].sound.channel);
}

#else

S2 play_wav (S2 screennum)
{
    return (TRUE);
}

S2 play_music (S2 screennum)
{
    return (TRUE);
}

void stop_music (S2 screennum)
{
   
}

void stop_sound (S2 screennum)
{
    
}

#endif
