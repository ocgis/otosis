#
# oTOSis -- xgemdos subdir makefile
#

OBJS = dispatch_xgemdos.o xgemdos.o

CSRC = dispatch_xgemdos.c xgemdos.c

all: xgemdos.a

xgemdos.a: $(OBJS)
	$(AR) rcs $@ $^

dep depend .depend:
	$(CC) $(CFLAGS) -M $(CSRC) >.depend

clean:
	rm -f $(OBJS) xgemdos.a .depend

reallyclean: clean

ifeq (.depend,$(wildcard .depend))
include .depend
endif
