#
# oTOSis -- gemdos subdir makefile
#

OBJS = dispatch_gemdos.o chardev.o file.o mem.o proc.o

CSRC = dispatch_gemdos.c chardev.c file.c mem.c proc.c

all: gemdos.a

gemdos.a: $(OBJS)
	$(AR) rcs $@ $^

dep depend .depend:
	$(CC) $(CFLAGS) -M $(CSRC) >.depend

clean:
	rm -f $(OBJS) gemdos.a .depend

reallyclean: clean

ifeq (.depend,$(wildcard .depend))
include .depend
endif
