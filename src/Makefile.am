SUBDIRS = lib

INCLUDES = -I../include

bin_PROGRAMS = tos

BUILT_SOURCES = scancode_us.c optiondef.c

tos_SOURCES = tos.c version.c load.c optfile.c traps.c file_emu.c \
	malloc.c trace.c strace.c \
	scancode_us.c optiondef.c

tos_LDADD = lib/libotosis.a

# FIXME
#tos_LDFLAGS = -Wl,-Telf-ldscript.x

scancode_us.c: ../tools/make_scancode ../scancode.us
	../tools/make_scancode ../scancode.us > scancode_us.c

optiondef.c: ../tools/options_to_c.awk ../options.def
	$(AWK) -f ../tools/options_to_c.awk < ../options.def > optiondef.c

