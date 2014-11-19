/****************************************************************************
*
* Filename: msg.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  status-, error messages
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2003
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

extern U1 *msgstr[];
extern U1 *errstr[];
extern struct plist *plist;
extern S4 **cclist;
extern U1 printmsg_no_msg;
extern struct includes *includes;

void printmsg (S2 msgno, U1 *str)
{
    if (! printmsg_no_msg)
    {
        printf ("%s\n", msgstr[msgno]);
        if (strlen (str) != 0) printf ("%s\n", str);
    }
}

void printerr (S2 errnum, S4 pos, U1 status, U1 *str)
{
    if (status == ST_PRE)
    {
        if (pos != NOTDEF)
        {
            printf ("error: line %li: %s\n", plist[pos].srcline, errstr[errnum]);

            /* print source line */

            printf ("[%s]", plist[pos].memptr);
            printf ("in file: %s\n", includes[plist[pos].include].name);
        }
        else
        {
            printf ("error: %s\n", errstr[errnum]);
        }
    }
    else
    {
        if (pos != NOTDEF)
        {
            printf ("error: line %li: %s\n", cclist[pos][BCELIST], errstr[errnum]);

            if (includes != NULL)
            {
                printf ("in file: %s\n", includes[cclist[pos][INCLUDELIST]].name);
            }
        }
        else
        {
            printf ("error: %s\n", errstr[errnum]);
        }
    }
    if (strlen (str) != 0) printf (" %s\n\n", str);
}

void printcompilermsg (void)
{
    /* print out gcc version and compile date */

    #if __GNUC__ >= 3
        printf ("compiled by gcc version: %i.%i.%i on %s\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__, __DATE__);
    #else
        printf ("compiled by gcc version: %i.%i on %s\n", __GNUC__, __GNUC_MINOR__, __DATE__);
    #endif
}
