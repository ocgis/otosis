CC = cc
CFLAGS = -g -v -I.
LDFLAGS = $(CFLAGS)
LIBS =

PROG = m68k_emu

OBJS = cpuemu.o memory.o

BUILD68K = build68k
BUILD68K_OBJS = build68k.o
TABLE68K = table68k

GENCPU = gencpu
GENCPU_OBJS = gencpu.o readcpu.o cpudefs.o malloc.o

all:		$(PROG)

$(PROG):	$(OBJS)
		$(CC) $(LDFLAGS) -o $(PROG) $(OBJS) $(LIBS)

cpudefs.c:	$(BUILD68K) $(TABLE68K)
		./$(BUILD68K) < $(TABLE68K) > $@

$(BUILD68K):	$(BUILD68K_OBJS)
		$(CC) -o $(BUILD68K) $(BUILD68K_OBJS)

$(GENCPU):	$(GENCPU_OBJS)
		$(CC) -o $(GENCPU) $(GENCPU_OBJS)


cpuemu.c:	$(GENCPU)
		./$(GENCPU)

clean:
		rm -f $(PROG) $(OBJS) $(BUILD68K) $(BUILD68K_OBJS) \
			$(GENCPU) $(GENCPU_OBJS) cpudefs.c cpuemu.c \
			cputbl.h cpustbl.c
