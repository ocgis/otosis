#
# oTOSis -- bios subdir makefile
#

OBJS = dispatch_bios.o chardev.o misc.o vt52.o

CSRC = dispatch_bios.c chardev.c misc.c vt52.c

all: bios.a

bios.a: $(OBJS)
	$(AR) rcs $@ $^

dep depend .depend:
	$(CC) $(CFLAGS) -M $(CSRC) >.depend

clean:
	rm -f $(OBJS) bios.a .depend

reallyclean: clean

ifeq (.depend,$(wildcard .depend))
include .depend
endif
