#
# oTOSis -- xbios subdir makefile
#

OBJS = dispatch_xbios.o hw.o

CSRC = dispatch_xbios.c hw.c

all: xbios.a

xbios.a: $(OBJS)
	$(AR) rcs $@ $^

dep depend .depend:
	$(CC) $(CFLAGS) -M $(CSRC) >.depend

clean:
	rm -f $(OBJS) xbios.a .depend

reallyclean: clean

ifeq (.depend,$(wildcard .depend))
include .depend
endif
