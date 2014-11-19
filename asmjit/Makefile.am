# AsmJit makefile

noinst_LIBRARIES = libAsmJit.a

libAsmJit_a_SOURCES = base/assembler.cpp \
	base/assert.cpp \
	base/codegen.cpp \
	base/compiler.cpp \
	base/context.cpp  \
	base/cpu.cpp \
	base/defs.cpp \
	base/error.cpp \
	base/func.cpp \
	base/logger.cpp \
	base/memorymanager.cpp \
	base/podvector.cpp \
	base/runtime.cpp \
	base/string.cpp \
	base/vmem.cpp \
	base/zone.cpp \
	x86/x86assembler.cpp \
	x86/x86compiler.cpp \
	x86/x86context.cpp \
	x86/x86cpu.cpp \
	x86/x86defs.cpp \
	x86/x86func.cpp
  
noinst_HEADERS = x86/x86assembler.h \
	x86/x86compiler.h \
	x86/x86context_p.h \
	x86/x86cpu.h \
	x86/x86defs.h \
	x86/x86func.h \
	asmjit.h \
	base.h \
	build.h \
	config.h \
	host.h \
	x86.h \
	base/apibegin.h \
	base/apiend.h \
	base/assembler.h \
	base/assert.h \
	base/codegen.h \
	base/context_p.h \
	base/cpu.h \
	base/defs.h \
	base/error.h \
	base/func.h \
	base/globals.h \
	base/intutil.h \
	base/lock.h \
	base/logger.h \
	base/memorymanager.h \
	base/podlist.h \
	base/podvector.h \
	base/runtime.h \
	base/string.h \
	base/vectypes.h \
	base/vmem.h \
	base/zone.h
	

AM_CFLAGS = -O0 -g -fPIC

AM_CPPFLAGS = -fPIC
AM_LDFLAGS =

SUBDIRS = 

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath
