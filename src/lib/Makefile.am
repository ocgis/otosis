SUBDIRS = bios gemdos hwreg mint xbios xgemdos

INCLUDES = -I../../include

noinst_LIBRARIES = libotosis.a

libotosis_a_SOURCES = biosbind.c gemdosbind.c mintbind.c \
	xbiosbind.c xgemdosbind.c

libotosis_a_LIBADD = \
	bios/dispatch_bios.o bios/chardev.o bios/misc.o bios/vt52.o \
	gemdos/dispatch_gemdos.o gemdos/chardev.o gemdos/file.o \
		gemdos/mem.o gemdos/proc.o \
	hwreg/dispatch_hwreg.o \
	mint/dispatch_mint.o mint/file.o mint/proc.o \
	xbios/dispatch_xbios.o xbios/hw.o \
	xgemdos/dispatch_xgemdos.o xgemdos/xgemdos.o
