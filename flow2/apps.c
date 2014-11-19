/****************************************************************************
*
* Filename: apps.c
*
* Author:   Stefan Pietzonke
* Project:  flow, the gfx plugin for nano VM
*
* Purpose:  app pool handling functions
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

#include "global_d.h"

extern struct app app[MAXAPPS];

S2 get_free_app_handle (void)
{
	S2 i, app_handle = -1;
	
	for (i = 0; i < MAXAPPS; i++)
	{
		if (app[i].in_use == FALSE)
		{
			app_handle = i;
			app[i].in_use = TRUE;
			break;
		}
	}
	
	return (app_handle);
}

void free_app_handle (S2 app_handle)
{
	app[app_handle].in_use == FALSE;
}
	
	