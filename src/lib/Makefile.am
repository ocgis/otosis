SUBDIRS = bios gemdos hwreg mint xbios xgemdos

INCLUDES = -I../../include

noinst_LIBRARIES = libotosis.a

libotosis_a_SOURCES = biosbind.c gemdosbind.c mintbind.c \
	xbiosbind.c xgemdosbind.c

libotosis_a_LIBADD = bios/*.o gemdos/*.o hwreg/*.o \
	mint/*.o xbios/*.o xgemdos/*.o

