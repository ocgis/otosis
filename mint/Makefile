#
# oTOSis -- mint subdir makefile
#

OBJS = dispatch_mint.o file.o proc.o

CSRC = dispatch_mint.c file.c proc.c

all: mint.a

mint.a: $(OBJS)
	$(AR) rcs $@ $^

dep depend .depend:
	$(CC) $(CFLAGS) -M $(CSRC) >.depend

clean:
	rm -f $(OBJS) mint.a .depend

reallyclean: clean

ifeq (.depend,$(wildcard .depend))
include .depend
endif
