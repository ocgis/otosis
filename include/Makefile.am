noinst_HEADERS = fd.h option.h sysvars.h version.h \
	toserrors.h vt52.h div.h prototypes.h traps.h \
	xgemdos.h bios.h gemdos.h xbios.h mint.h \
	optiondef.h compile.h


optiondef.h: ../tools/options_to_h.awk ../options.def
	$(AWK) -f ../tools/options_to_h.awk < ../options.def > optiondef.h

compile.h: force
	@echo "Creating compile.h"
	@echo \#define OTOSIS_COMPILE_TIME \"`date '+%D %T'`\" > compile.h
	@echo \#define OTOSIS_COMPILE_BY \"`whoami`\" >> compile.h
	@echo \#define OTOSIS_COMPILE_HOST \"`hostname`\" >> compile.h
	@echo \#define OTOSIS_COMPILER \"`$(CC) -v 2>&1 | tail -1`\" >> compile.h


force:

 