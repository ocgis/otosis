CFLAGS = @CFLAGS@ -Wall

SUBDIRS = lib

INCLUDES = -I../include -Ilib

bin_PROGRAMS = tos

tos_SOURCES = \
	emulate.c emulate.h tos.c version.c load.c traps.c trace.c strace.c

tos_LDADD = lib/libotosis.la

# FIXME
#tos_LDFLAGS = -Wl,-Telf-ldscript.x

