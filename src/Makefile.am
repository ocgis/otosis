CFLAGS = -Wall

SUBDIRS = lib

INCLUDES = -I../include -Ilib

bin_PROGRAMS = tos

tos_SOURCES = \
	tos.c version.c load.c optfile.c traps.c trace.c strace.c

tos_LDADD = lib/libotosis.la

# FIXME
#tos_LDFLAGS = -Wl,-Telf-ldscript.x

