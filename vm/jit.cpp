/****************************************************************************
*
* Filename: jit.cpp
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  AsmJit JIT compiler: JIT include glue code
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


#include "host.h"

#define JIT_CPP  1

#include "global_d.h"

#if HAVE_JIT_COMPILER

#if DEBUG
    extern U1 print_debug;
    #define PRINTD(s);  if (print_debug) { printf ("%s\n", s); }
#else
    #define PRINTD(s);
#endif

#if OS_AROS
#undef bind
#endif

#include <asmjit/src/asmjit/asmjit.h>


	
#if JIT_X86
	#include "jit_x86.h"
#endif

#endif
				