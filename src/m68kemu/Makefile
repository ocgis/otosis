CC = cc
CFLAGS = -g -v -I.
LDFLAGS = $(CFLAGS)
LIBS = -lm

PROG = m68k_emu

OBJS = m68k_emu.o cpuemu.o cpustbl.o cpudefs.o fpp.o newcpu.o \
	memory.o readcpu.o support.o malloc.o

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


cpuemu.c cpustbl.c:	$(GENCPU)
		./$(GENCPU)

support.o:	machdep/support.c
		$(CC) $(CFLAGS) -c machdep/support.c -o $@

clean:
		rm -f $(PROG) $(OBJS) $(BUILD68K) $(BUILD68K_OBJS) \
			$(GENCPU) $(GENCPU_OBJS) cpudefs.c cpuemu.c \
			cputbl.h cpustbl.c
