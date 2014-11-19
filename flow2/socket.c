/****************************************************************************
*
* Filename: socket.c
*
* Author:   Stefan Pietzonke
* Project:  flow, the gfx plugin for nano VM
*
* Purpose:  socket functions
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

/* socket send  ---------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>



/* #include <winsock.h> */
#include "global_d.h"
#include "socket.h"



#if SOCKETS_NATIVE

U1 exe_gethostbyaddr (U1 *s_reg1, U1 *s_reg2)
{
    struct hostent *hp;
    struct in_addr hostaddr;

    hostaddr.s_addr = inet_addr (s_reg1);
    hp = (struct hostent *) gethostbyaddr ((U1 *) &hostaddr, sizeof (struct in_addr), AF_INET);
    if (hp == NULL)
    {
        return (1);
    }

    strcpy (s_reg2, hp->h_name);
    return (0);
}

U1 read_8 (int fd, U1 *num)
{
	if (recv (fd, num, 1, MSG_NOSIGNAL) == -1)
	{
		return (FALSE);
	}
	else
	{
		return (TRUE);
	}
}

U1 send_8 (int fd, U1 num)
{
	if (send (fd, &num, 1, MSG_NOSIGNAL) == -1)
	{
		return (FALSE);
	}
	else
	{
		return (TRUE);
	}
}

U1 read_16 (int fd, U2 *num)
{
	U2 conv;
	
	if (recv (fd, &conv, 2, MSG_NOSIGNAL) == -1)
	{
		return (FALSE);
	}
	else
	{
		*num = ntohs (conv);
		return (TRUE);
	}
}

U1 send_16 (int fd, U2 num)
{
	U2 conv;
	
	conv = htons (num);
	
	if (send (fd, &conv, 2, MSG_NOSIGNAL) == -1)
	{
		return (FALSE);
	}
	else
	{
		return (TRUE);
	}
}


U1 read_line_string (int fd, U1 *string, S4 len)
{
    /* read CRLF or LF terminated line */

    S2 ret;
    U1 ch;
    U1 end = FALSE;
    U1 error = FALSE;

    S4 i = 0;

    while (! end)
    {
		ret = recv (fd, &ch, 1, MSG_NOSIGNAL);
        if (ret <= 0)
        {
            error = TRUE; end = TRUE;
        }

        if (ch != '\n')
        {
            if (i <= len)
            {
                string[i] = ch;
                i++;
            }
            else
            {
               error = TRUE; end = TRUE;
            }
        }
        else
        {
            /* line end */
            /* check if last char was a CR */

            if (string[i - 1] == '\r')
            {
                i--;
            }

            string[i] = BINUL;

            end = TRUE;
        }
    }

    if (error)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 send_line_string (int fd, U1 *string)
{
    /* send line terminated with CRLF */

    U1 buf[256];
    S2 ret;
    S4 len;

    len = strlen (string);
    if (len > (255 - 2))
    {
        printf ("send_line_string error: string overflow!\n");
        return (FALSE);
    }

    strcpy (buf, string);
    buf[len] = '\r';
    buf[len + 1] = '\n';

    len += 2;

	ret = send (fd, &buf, len, MSG_NOSIGNAL);
    if (ret < len)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}


#else

U1 read_8 (TCPsocket fd, U1 *num)
{
    if (SDLNet_TCP_Recv (fd, num, 1) <= 0)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 send_8 (TCPsocket fd, U1 num)
{
    if (SDLNet_TCP_Send (fd, &num, 1) != 1)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 read_16 (TCPsocket fd, U2 *num)
{
    U2 buf[2];

    if (SDLNet_TCP_Recv (fd, buf, 2) <= 0)
    {
        return (FALSE);
    }

    *num = (U2) SDLNet_Read16 (buf);
    return (TRUE);
}

U1 send_16 (TCPsocket fd, U2 num)
{
    U2 buf[2];

    SDLNet_Write16 (num, buf);

    if (SDLNet_TCP_Send (fd, buf, 2) != 2)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}



U1 read_line_string (TCPsocket fd, U1 *string, S4 len)
{
    /* read CRLF or LF terminated line */

    S2 ret;
    U1 ch;
    U1 end = FALSE;
    U1 error = FALSE;

    S4 i = 0;

    while (! end)
    {
        ret = SDLNet_TCP_Recv (fd, &ch, 1);
        if (ret <= 0)
        {
            error = TRUE; end = TRUE;
        }

        if (ch != '\n')
        {
            if (i <= len)
            {
                string[i] = ch;
                i++;
            }
            else
            {
               error = TRUE; end = TRUE;
            }
        }
        else
        {
            /* line end */
            /* check if last char was a CR */

            if (string[i - 1] == '\r')
            {
                i--;
            }

            string[i] = BINUL;

            end = TRUE;
        }
    }

    if (error)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

U1 send_line_string (TCPsocket fd, U1 *string)
{
    /* send line terminated with CRLF */

    U1 buf[256];
    S2 ret;
    S4 len;

    len = strlen (string);
    if (len > (255 - 2))
    {
        printf ("send_line_string error: string overflow!\n");
        return (FALSE);
    }

    strcpy (buf, string);
    buf[len] = '\r';
    buf[len + 1] = '\n';

    len += 2;

    ret = SDLNet_TCP_Send (fd, buf, len);
    if (ret < len)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

#endif