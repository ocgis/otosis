CFLAGS = @CFLAGS@ -Wall

INCLUDES = -I../../include

osisdir = @OSISINCLUDE@
osis_HEADERS = biosbind.h gemdosbind.h mintbind.h xbiosbind.h

lib_LTLIBRARIES = libotosis.la

bios_sources = \
	bios_chardev.c bios_dispatch.c bios_misc.c bios_vt52.c biosbind.c

gemdos_sources = \
	gemdos_chardev.c gemdos_dispatch.c gemdos_file.c gemdos_mem.c \
	gemdos_proc.c gemdosbind.c

hwreg_sources = \
	hwreg_dispatch.c

mint_sources = \
	mint_dispatch.c mint_file.c mint_proc.c mintbind.c

xbios_sources = \
	xbios_dispatch.c xbios_hw.c xbiosbind.c

xgemdos_sources = \
	xgemdos.c xgemdos_dispatch.c xgemdosbind.c

common_sources = \
	file_emu.c init.c init.h malloc.c optfile.c optiondef.c scancode_us.c \
	tty.c

BUILT_SOURCES =  optiondef.c scancode_us.c

libotosis_la_SOURCES = \
	${bios_sources} \
	${gemdos_sources} \
	${hwreg_sources} \
	${mint_sources} \
	${xbios_sources} \
	${common_sources}

optiondef.c: ../../tools/options_to_c.awk ../../options.def
	$(AWK) -f ../../tools/options_to_c.awk < ../../options.def > optiondef.c

scancode_us.c: ../../tools/make_scancode ../../scancode.us
	../../tools/make_scancode ../../scancode.us > scancode_us.c

libotosis_la_LDFLAGS = @LT_VERSION_INFO@

EXTRA_DIST =\
	${xgemdos_sources}
