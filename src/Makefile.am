bin_PROGRAMS = tos

CPPFLAGS = -DDEBUG

tos_SOURCES = \
	compile.h debug.h emulate.c emulate.h tos.c version.c load.c \
	traps.c trace.c strace.c

BUILT_SOURCES = compile.h

compile.h:
	@echo "Creating compile.h"
	@echo \#define OTOSIS_COMPILE_TIME \"`date '+%D %T'`\" > compile.h
	@echo \#define OTOSIS_COMPILE_BY \"`whoami`\" >> compile.h
	@echo \#define OTOSIS_COMPILE_HOST \"`hostname`\" >> compile.h
	@echo \#define OTOSIS_COMPILER \"`$(CC) -v 2>&1 | tail -1`\" >> compile.h

