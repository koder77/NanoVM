/****************************************************************************
*
* Filename: mutex.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  mutex functions
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2015
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

#include "../include/host.h"
#include "../include/global_d.h"


void var_lock (struct varlist *varlist, S4 index)
{
	#if OS_AROS
		LockMutex (varlist[index].mutex);
    #else
        pthread_mutex_lock (&varlist[index].mutex);
    #endif
}
	
void var_unlock (struct varlist *varlist, S4 index)
{
	#if OS_AROS
		UnlockMutex (varlist[index].mutex);
	#else
		pthread_mutex_unlock (&varlist[index].mutex);
    #endif
}
