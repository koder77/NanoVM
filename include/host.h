/****************************************************************************
*
* Filename: host.h
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  host definitions
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

#define TRUE            1
#define FALSE           0


/* switch nano -d option (debug printfs) ---------------------------- */

#define DEBUG          0

/* switch ANDROID debugging: if set to one then programs will be searched in:
 * (slash)data/local/tmp/nanovm/prog/
 *
 * if set to null then in:
 * (slash)mnt/sdcard/nanovm/prog/
 *
 * with the same path for includes also!
 */

#define ANDROID_DEBUG	0

/* set Android SD card path for Nano VM root */

#define ANDROID_SDCARD	"/sdcard/"

#define ANDROID_TERMUX  "~/storage/external-1/"

/* should be only one of this two settings */
#define ANDROID_SDCARD_SET  0
#define ANDROID_TERMUX_SET  1


#define OS_LINUX_RPI	0			/* Raspberry Pi Raspbian: used for exe_socket.c, include IP V 4 sockets if set to 1. */


/* switch online registering: default on at firstrun ---------------- */

#define ONLINE_REGISTER 0


/* posix threads CPU cores (Linux) ---------------------------------- */

/* set this to 0 to do autoconfig at runtime */
#define CPU_CORES   0

#define HYPERTHREADING  1


/* switch JIT compiler on/off --------------------------------------- */

#define HAVE_JIT_COMPILER	0

#define JIT_X86_64 		TRUE
#define JIT_X86_32		FALSE

#define JIT_X86 TRUE

/* set machine type ------------------------------------------------- */

#define M_PC            FALSE
#define M_AMIGA         FALSE
#define M_ARM			FALSE


#if _X86_ || __i386__ || __x86_64__
    #undef M_PC
    #undef M_AMIGA
	#undef M_ARM

    #define M_PC        TRUE

    #define M_AMIGA     FALSE

	#define M_ARM		FALSE

	#undef JIT_X86
	#define JIT_X86		TRUE

    #pragma message("MACHINE = X86")

#elif __mc68000__
    #undef M_PC
    #undef M_AMIGA
	#undef M_ARM

    #define M_AMIGA     TRUE

    #define M_PC        FALSE

	#define M_ARM 		FALSE

    #pragma message("MACHINE = MC68000")

#elif __arm__
	#undef M_PC
	#undef M_AMIGA
	#undef M_ARM

	#define M_AMIGA FALSE

	#define M_PC	FALSE

	#define M_ARM	TRUE

	#pragma message("MACHINE = ARM")
#endif


/* set os type ------------------------------------------------------ */

#define OS_TYPE_AMIGA   1
#define OS_TYPE_AROS    2
#define OS_TYPE_LINUX   3
#define OS_TYPE_WINDOWS 4
#define OS_TYPE_ANDROID 5


/* unset all */

#define OS_AMIGA            FALSE
#define OS_AROS             FALSE
#define OS_LINUX            FALSE
#define OS_WINDOWS          FALSE
#define OS_WINDOWS_CYGWIN   FALSE
#define OS_ANDROID			FALSE


#if __amigaos__
    #undef  OS_AMIGA
    #define OS_AMIGA    TRUE
    #define OS_TYPE     OS_TYPE_AMIGA

    #pragma message("OS = AMIGA")

#elif __AROS__
    #undef  OS_AROS
    #define OS_AROS     TRUE
    #define OS_TYPE     OS_TYPE_AROS

    #pragma message("OS = AROS")

#elif __ANDROID__
	#undef  OS_ANDROID
	#define OS_ANDROID   TRUE
	#define OS_TYPE      OS_TYPE_ANDROID

	#pragma message("OS = ANDROID")

#elif __linux__
    #undef  OS_LINUX
    #define OS_LINUX    TRUE
    #define OS_TYPE     OS_TYPE_LINUX

    #pragma message("OS = LINUX")

#elif _WIN32
    #undef  OS_WINDOWS
    #define OS_WINDOWS  TRUE
    #define OS_TYPE     OS_TYPE_WINDOWS

    #pragma message("OS = WINDOWS")

#elif _CYGWIN
    #undef  OS_WINDOWS_CYGWIN
    #define OS_WINDOWS_CYGWIN  TRUE
    #define OS_TYPE     OS_TYPE_WINDOWS

    #pragma message("OS = WINDOWS CYGWIN")
#endif




/* machine number --------------------------------------------------- */

#define MACHINE_AMIGA   1
#define MACHINE_PC      2
#define MACHINE_ARM		3


/* endianess -------------------------------------------------------- */

#if M_AMIGA
    #define BS_LITTLE_ENDIAN    FALSE
    #define MACHINE_TYPE        MACHINE_AMIGA
    
    #pragma message("BIG ENDIAN")
#endif

#if M_PC
    #define BS_LITTLE_ENDIAN    TRUE
    #define MACHINE_TYPE        MACHINE_PC
    
    #pragma message("LITTLE ENDIAN")
#endif

#if M_ARM
	#define BS_LITTLE_ENDIAN	TRUE
	#define MACHINE_TYPE		MACHINE_ARM
	
	#pragma message("LITTLE ENDIAN")
#endif

/* if os uses slash "/" in paths, then set to FALSE ----------------- */

#if OS_AMIGA
    #define PATH_SLASH_CONV     FALSE
#endif

#if OS_AROS
    #define PATH_SLASH_CONV     FALSE
#endif

#if OS_WINDOWS || OS_WINDOWS_CYGWIN
    #define PATH_SLASH_CONV     TRUE
#endif

#if OS_LINUX || OS_ANDROID
    #define PATH_SLASH_CONV     FALSE
#endif


/* file newline (write only) ---------------------------------------- */

#if OS_AMIGA
    #define NEWLINE_SB          "\n"
#endif

#if OS_AROS
    #define NEWLINE_SB          "\n"
#endif

#if OS_WINDOWS || OS_WINDOWS_CYGWIN
    #define NEWLINE_SB          "\r\n"
#endif

#if OS_LINUX || OS_ANDROID
    #define NEWLINE_SB          "\n"
#endif


/* THREADING config ------------------------------------------------- */

#if OS_LINUX
	#define HAVE_THREADING			1
#endif

#if OS_WINDOWS
	#define HAVE_THREADING			1
#endif

#if OS_ANDROID
	#define HAVE_THREADING			0
#endif


/* do sane check... ------------------------------------------------- */

#if M_PC == FALSE && M_AMIGA == FALSE && M_ARM == FALSE
    #error "host.h: FATAL ERROR can't detect machine type!!!"
#endif

#if OS_AMIGA == FALSE && OS_AROS == FALSE && OS_WINDOWS == FALSE && OS_WINDOWS_CYGWIN == FALSE && OS_LINUX == FALSE && OS_ANDROID == FALSE
    #error "host.h: FATAL ERROR can't detect OS type!!!"
#endif

#if JIT_X86_32 == TRUE && JIT_X86_64 == TRUE
	#error "host.h FATAL ERROR set JIT_X86 TYPE: 32 OR 64 bit!!!"
#endif

#define JIT_CPP        0
/* END */

