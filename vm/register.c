/****************************************************************************
*
* Filename: register.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  online registering: statistical functions
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2011
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


#include "host.h"
#include "global_d.h"

/* fallback for Amiga OS or any other without MSG_NOSIGNAL */
#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
#endif

#if ONLINE_REGISTER

int register_online (void)
{
    S2 client, error, regopen = 0, ok = 0, i;
    struct sockaddr_in address;

    FILE *reg; S2 code;

    U1 regfile[512];
    U1 getstr[4096];
    U1 buf[256];

    strcpy (getstr, "GET http://diy-2010.net/nano/registry HTTP/1.1\nHost: diy-2010.net\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\nAccept-Language: de-de,de;q=0.8,en-us;q=0.5,en;q=0.3\nAccept-Encoding: *\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\nKeep-Alive: 300\nConnection: keep-alive\n\n");

    if (getenv (NANOVM_ROOT_SB) == NULL)
    {
        printf ("register: error NANOVM_ROOT env variable is not set!\nusing default path!\n");
        
		if (OS_AMIGA || OS_AROS)
        {
            strcpy (regfile, "Work:nanovm/home/register");
        }

        if (OS_LINUX)
        {
            strcpy (regfile, "~/nanovm/home/register");
        }

        if (OS_ANDROID)
		{
			#if ANDROID_DEBUG
				strcpy (regfile, "/data/local/tmp/nanovm/home/register");
			#else
				strcpy (regfile, "/sdcard/nanovm/home/register");
			#endif
		}
            
        if (OS_WINDOWS)
        {
            strcpy (regfile, "C:/nanovm/home/register");
        }
		
    }
    else
	{
		strcpy (regfile, getenv (NANOVM_ROOT_SB));
		strcat (regfile, "/home/register");
	}
	
    reg = fopen (regfile, "rb");
    if (reg == NULL)
    {
        printf ("register: error can't open file %s\n", regfile);
        goto register_online;
    }

    code = fgetc (reg);
    if (code == EOF)
    {
        printf ("register: error can't read code in file %s\n", regfile);
        regopen = 1;
        goto register_online;
    }
    if (code == 42)
    {
        fclose (reg);
        return (0);
    }

register_online:
    if (regopen) fclose (reg);

    reg = fopen (regfile, "wb");
    if (reg == NULL)
    {
        printf ("register: error can't open file %s\n", regfile);
        goto register_online;
    }

    printf ("registering online:\nthis will not send any private data!\nAnd is for statistics only.\n");
    printf ("connecting to diy-2010.net...\n");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr ("178.254.10.206");
    address.sin_port = htons (80);
    memset (&(address.sin_zero), BINUL, 8);             /* zero the rest */

    client = socket (PF_INET, SOCK_STREAM, 0);
    if (client == -1)
    {
        printf ("register: error can't open socket!\n");
        fclose (reg);
        return (1);
    }

    error = connect (client, (struct sockaddr *) &address, sizeof (struct sockaddr));
    if (error == -1)
    {
        printf ("register: error can't connect!\n");
        fclose (reg);
        return (1);
    }

    error = send (client, getstr, strlen (getstr) + 1, MSG_NOSIGNAL);
    if (error == -1)
    {
        printf ("register: error can't request registry!\n");
        fclose (reg);
        close (client);
        return (1);
    }

    /* get answer */
    i = 0;
    while (! ok)
    {
        error = recv (client, &(buf[i]), 1, MSG_NOSIGNAL);
        if (error == -1)
        {
            printf ("register: error can't read server answer!\n");
            fclose (reg);
            close (client);
            return (1);
        }

        if (buf[i] == '\r')
        {
            buf[i] = BINUL;
            ok = 1;
        }
        i++;
    }

    printf ("[SERVER: %s]\n", buf);

    /* all ok: save registry code: */

    fseek (reg, 0, SEEK_SET);
    clearerr (reg);

    if (fputc (42, reg) == EOF)
    {
        printf ("register: error can't save registry code to file %s!\n", regfile);
        fclose (reg);
        close (client);
        return (1);
    }

    printf ("register: Nano is succesfully registered\ncode is in file: %s\n", regfile);

    fclose (reg);
    close (client);
    return (2);
}

#else

int register_online (void)
{
    return (0);
}

#endif