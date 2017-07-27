/****************************************************************************
*
* Filename: arch.h
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  architecture dependent macros
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

#if OS_WINDOWS
    #include <windows.h>
#endif


/* exe_main.c ------------------------------------------------------------ */

/* wait_sec -------------------------------------------------------------- */

#if OS_AMIGA || OS_AROS
    #define EXE_WAIT_SEC();  Delay (vmreg.l[ARG1] * NANO_TIMER_TICKS);
#endif

#if OS_WINDOWS
    #define EXE_WAIT_SEC();  Sleep (vmreg.l[ARG1] * CLOCKS_PER_SEC);
#endif

#if OS_LINUX || OS_WINDOWS_CYGWIN || OS_ANDROID
    #define EXE_WAIT_SEC();  sleep (vmreg.l[ARG1]);
#endif

/* wait_tick ------------------------------------------------------------- */

#if OS_AMIGA || OS_AROS
    #define EXE_WAIT_TICK();  Delay (vmreg.l[ARG1]);
#endif

#if OS_WINDOWS
    #define EXE_WAIT_TICK();  Sleep (vmreg.l[ARG1]);
#endif

#if OS_LINUX || OS_WINDOWS_CYGWIN || OS_ANDROID
    #define EXE_WAIT_TICK();  usleep (vmreg.l[ARG1] * 1000);
#endif


/* wait_one_tick ---------------------------------------------------------- */

#if OS_AMIGA || OS_AROS
    #define PWAIT_TICK();  Delay (1);
#endif

#if OS_WINDOWS
    #define PWAIT_TICK();  Sleep (40 * CLOCKS_PER_SEC / NANO_TIMER_TICKS);
#endif

#if OS_LINUX || OS_WINDOWS_CYGWIN || OS_ANDROID
    #define PWAIT_TICK();  usleep (40 * CLOCKS_PER_SEC / NANO_TIMER_TICKS);
#endif
