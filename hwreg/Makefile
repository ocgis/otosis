#
# oTOSis -- hwreg subdir makefile
#

OBJS = dispatch_hwreg.o

CSRC = dispatch_hwreg.c

all: hwreg.a

hwreg.a: $(OBJS)
	$(AR) rcs $@ $^

dep depend .depend:
	$(CC) $(CFLAGS) -M $(CSRC) >.depend

clean:
	rm -f $(OBJS) hwreg.a .depend

reallyclean: clean

ifeq (.depend,$(wildcard .depend))
include .depend
endif
