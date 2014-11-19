/****************************************************************************
*
* Filename: rights.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  execution rights of a program
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2013
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

/* Read rights file for example foo.no -> foo.nr.
 * network=on -> allow socket access.
 * file=on -> allow file access inside nanovm/home directory.
 * fileroot=on -> allow file access in nanovm/ directory and access to all sub-directories.
 * 
 * Settings must be at the start of a line and written like in this example.
 * With =off they are switched off.
 * 
 * Default setting is to allow file access inside nanovm/home directory
 * and to disallow fileroot and sockets.
 * 
 * I added this to make it possible to run programs in "sandbox" mode.
 * And the "virtual" filesystem.
 */


#include "host.h"
#include "global_d.h"

extern struct rights rights;

U1 load_rights (U1 *filename)
{
	FILE *rightsfile;
	U1 *read;
	U1 rbuf[MAXLINELEN + 1];
	U1 ok = TRUE;
	S2 slen;
	
	if ((rightsfile = fopen (filename, "r")) != NULL)
	{
		/* file opened, read rights */
		
		while (ok == TRUE)
		{
			read = fgets_uni (rbuf, MAXLINELEN, rightsfile);
			
			slen = strlen (rbuf);
			
			if (rbuf[slen - 1] == '\n')
			{
				/* cut newline end of */
				
				rbuf[slen - 1] = '\0';
			}
			
			if (read != NULL)
			{
				if (strcmp (rbuf, "network=on") == 0)
				{
					rights.network = TRUE;
				}
			
				if (strcmp (rbuf, "network=off") == 0)
				{
					rights.network = FALSE;
				}
			
				if (strcmp (rbuf, "file=on") == 0)
				{
					rights.file = TRUE;
				}
			
				if (strcmp (rbuf, "file=off") == 0)
				{
					rights.file = FALSE;
				}
				
				if (strcmp (rbuf, "fileroot=on") == 0)
				{
					rights.fileroot = TRUE;
				}
			
				if (strcmp (rbuf, "fileroot=off") == 0)
				{
					rights.fileroot = FALSE;
				}
				
				if (strcmp (rbuf, "process=on") == 0)
				{
					rights.process = TRUE;
				}
			
				if (strcmp (rbuf, "process=off") == 0)
				{
					rights.process = FALSE;
				}
			}
			else
			{
				ok = FALSE;
			}
		}
		fclose (rightsfile);
	}
	return (TRUE);
}
			
			
			
			