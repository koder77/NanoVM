/****************************************************************************
*
* Filename: exe_time.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  set time variables
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

extern struct varlist *varlist;
extern struct tm *tm;

void exe_time (void)
{
    time_t secs;

    time (&secs);               /* get time */
    tm = localtime (&secs);

    *varlist[TIME_HOUR].i_m = tm->tm_hour;
    *varlist[TIME_MIN].i_m = tm->tm_min;
    *varlist[TIME_SEC].i_m = tm->tm_sec;
    *varlist[DATE_YEAR].i_m = tm->tm_year + 1900;
    *varlist[DATE_MONTH].i_m = tm->tm_mon + 1;
    *varlist[DATE_DAY].i_m = tm->tm_mday;
    *varlist[DATE_WDAY].i_m = tm->tm_wday + 1;
    *varlist[DATE_YDAY].i_m = tm->tm_yday + 1;
}

