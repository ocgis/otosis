#
# oTOSis toplevel makefile
#

# select whether to support debugging ('yes' or 'no')
DEBUG = yes

# set to 'no' if you don't have oVDIsis, or don't want to use it,
# otherwise 'yes'. Be sure OVDISISDIR is set correctly if 'yes'.
XGEMDOS = yes
OVDISISDIR = /usr/local/src/osis/ovdisis
OAESISDIR = /usr/local/src/osis/noaesis

# compilers and flags:
CC      = gcc
CFLAGS  = -Wall -Wstrict-prototypes -I$(shell pwd)/include
LD      = gcc
LDFLAGS = 
AR      = ar
LIBS    = -lncurses


# different CFLAGS depending on DEBUG
ifeq ($(DEBUG),yes)
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2 -fomit-frame-pointer
endif

# test for cross compiling
COMPILE_ARCH = $(shell uname -m)
ifneq ($(COMPILE_ARCH),m68k)
	CROSSPREFIX=m68k-linux-
	CC := $(CROSSPREFIX)$(CC)
	LD := $(CROSSPREFIX)$(LD)
	AR := $(CROSSPREFIX)$(AR)
endif

DO_MAKE = $(MAKE) 'CC=$(CC)' 'CFLAGS=$(CFLAGS)' 'LD=$(LD)' \
                  'LDFLAGS=$(LDFLAGS)' 'AR=$(AR)'

# determine output binary format, and modify linker options for ELF
BINFMT=$(shell $(CC) -E -P test-elf.c | grep -v ^$$)
ifeq ($(BINFMT),ELF)
LDFLAGS += -Wl,-Telf-ldscript.x
endif

.PHONY: all subdirs clean reallyclean dep depend tar force

# scancode table conversion program
MAKE_SCANCODE = make_scancode
MAKE_SCANCODE_OBJS = make_scancode.o

# overall target:
EXEC = tos

# sub-directories:
SUBDIRS = bios xbios gemdos mint hwreg doc
SUBDIROBJS = bios/bios.a xbios/xbios.a gemdos/gemdos.a mint/mint.a \
             hwreg/hwreg.a

# objects in main directory
OBJS = tos.o version.o load.o optfile.o traps.o file_emu.o malloc.o \
       trace.o strace.o scancode_us.o

# C sources in main directory
CSRC = tos.c version.c  load.c optfile.c traps.c file_emu.c malloc.c \
       trace.c strace.c

# Add some things if xgemdos is to be used
ifeq ($(XGEMDOS),yes)
CFLAGS += -I$(OVDISISDIR) -I$(OAESISDIR) -DUSE_XGEMDOS
LIBS += -lfb
SUBDIRS += xgemdos
SUBDIROBJS += xgemdos/xgemdos.a $(OVDISISDIR)/src/ovdisis.a \
	$(OAESISDIR)/src/oaesis.a
endif


# determine prerequisites for 'make all' to be done before making
# target $(EXEC). These could be .depend and optiondef.[ch]
PREREQ :=
ifneq (optiondef.c,$(wildcard optiondef.c))
PREREQ += optiondef.c
endif
ifneq (include/compile.h,$(wildcard include/compile.h))
PREREQ += include/compile.h
endif
ifneq (include/optiondef.h,$(wildcard include/optiondef.h))
PREREQ += include/optiondef.h
endif
ifneq (.depend,$(wildcard .depend))
PREREQ += .depend
endif


# general target:
all: $(PREREQ) $(EXEC)

$(EXEC): include/compile.h $(OBJS) subdirs
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(SUBDIROBJS) $(LIBS)

subdirs: force
	set -e; for dir in $(SUBDIRS); do \
		$(DO_MAKE) -C $$dir; \
	done

include/option.h: include/optiondef.h

# special rule to build optiondef.[ch] from options.def
optiondef.c include/optiondef.h: options.def options.awk
	awk -f options.awk <$<

include/compile.h: force
	@echo \#define OTOSIS_COMPILE_TIME \"`date '+%D %T'`\" > include/compile.h
	@echo \#define OTOSIS_COMPILE_BY \"`whoami`\" >> include/compile.h
	@echo \#define OTOSIS_COMPILE_HOST \"`hostname`\" >> include/compile.h
	@echo \#define OTOSIS_COMPILER \"`$(CC) -v 2>&1 | tail -1`\" >> include/compile.h

scancode_us.c: $(MAKE_SCANCODE) scancode.us
	./$(MAKE_SCANCODE) scancode.us > $@

$(MAKE_SCANCODE): $(MAKE_SCANCODE_OBJS)
	$(CC) -o $(MAKE_SCANCODE) $(MAKE_SCANCODE_OBJS)

clean:
	rm -f *.o *~ optiondef.c include/optiondef.h .depend TAGS
	rm -f $(MAKE_SCANCODE) $(MAKE_SCANCODE_OBJS) scancode_us.c
	set -e; for dir in $(SUBDIRS); do \
		$(DO_MAKE) -C $$dir clean; \
	done

reallyclean:
	rm -f *.o *~ optiondef.c include/optiondef.h .depend TAGS
	rm -f $(EXEC) include/compile.h
	set -e; for dir in $(SUBDIRS); do \
		$(DO_MAKE) -C $$dir reallyclean; \
	done

dep depend .depend:
	$(CC) $(CFLAGS) -M $(CSRC) >.depend
	set -e; for dir in $(SUBDIRS); do \
		$(DO_MAKE) -C $$dir dep; \
	done

TAGS: etags `find . -name '*.[ch]'`

tar:
	cd ..; tar cvzf $(notdir $(shell pwd)).tar.gz \
		`find $(notdir $(shell pwd)) -type f -a ! -name '*.[oa]' \
              -a ! -name .depend -a ! -name tos -a ! -name 'optiondef.[ch]' \
			  -a ! -name 'OSis.??' -a ! -name 'OSis.???' \
			  -a ! -name 'OSis.info*' -print`

# usage: make diff OLDVER=<dir-of-old-version-relative-to-parent>
diff:
	-cd ..; diff -u --recursive --new-file \
	    '--exclude=*.[oa]' '--exclude=*.orig' '--exclude=*.rej' \
	    '--exclude=.*' '--exclude=*~' '--exclude=#*#' '--exclude=tos' \
	    '--exclude=optiondef.[ch]' '--exclude=compile.h' '--exclude=TAGS' \
	    '--exclude=OSis.??' '--exclude=OSis.???' '--exclude=OSis.info*' \
	    $(OLDVER) $(notdir $(shell pwd)) >$(notdir $(shell pwd)).diff
	rm -f ../$(notdir $(shell pwd)).diff.gz
	gzip -9 ../$(notdir $(shell pwd)).diff

force:

ifeq (.depend,$(wildcard .depend))
include .depend
endif
