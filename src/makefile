# Load libraries where executable code is written
AUTH = MACHO.AUTH
LOAD = MACHO.PDSE
LOAD64 = MACHO.PDSE64

# MACLIB where Assembler source members in src/missing directory should be copied
MISSING = MISSING.MACLIB

AS = as
AFLAGS =
CC = xlc
LD = ld
CFLAGS = -qin=all:nostp -qfloat=ieee "-Wc,supp(CCN3415,CCN3571)" -S -qmetal -qnosearch -I /usr/include/metal
CFLAGS64 = -q64 -qfloat=ieee -qasm -qasmlib=$(MISSING):sys1.maclib:sys1.modgen -qin=all:nostp "-Wc,supp(CCN3415,CCN3571)" "-Wl,dll"
LFLAGS = -breus=rent
INCS =

FILES = alloc alloc4k cmpsc cmpsc3 copyin copyout dfltcc dfltcc3 extract floatin floatout mcfree kdsa kdsa3 kimd kimd3 \
        klmd klmd3 km km3 kma kma3 kmac kmac3 kmc kmc3 kmctr kmctr3 kmf kmf3 kmo kmo3 nnpa nnpa3 pcc pcc3 pckmo pfpo \
        prno prno3 qedit sortl sortl3 stck stcke stckf vcfn vclfnh vclfnl vcnf vcrnf wait

macho : $(FILES:+".exe") machopc.exe macpcreg.exe
	touch macho

machopc.exe: machopc.c machopc.h MACPCPRO MACPCEPI
	$(CC) $(CFLAGS) -q64 -o $*.s $(INCS) $*.c
	$(AS) $(AFLAGS) -o $*.o -I .:$(MISSING) $*.s 
	$(LD) $(LFLAGS) -o "//'$(AUTH)($*)'" $*.o
	touch $*.exe

macpcreg.exe: macpcreg.c machopc.h ieantc.h iezcom.h
	$(CC) -qasm -qasmlib=sys1.maclib:sys1.modgen "-Wl,ac=1" -o "//'$(AUTH)($*)'" $*.c
	touch $*.exe

# Inter-module dependencies.  extract.c and wait.c call ALLOC and
# MCFREE, and the bind autocalls $(LOAD) and $(LOAD64), so those two
# modules must already be in the target libraries.  In FILES order
# extract comes before mcfree, so without this a build into an empty
# library leaves MCFREE unresolved - and a build into a populated one
# quietly binds against the previous version.
extract.exe wait.exe: alloc.exe mcfree.exe

# Every recipe includes macho.h, and the stubs that reach the PC routine
# include machopc.h and ieantc.h as well.  The pattern rule below
# supplies the commands; these lines only add the dependencies, so that
# changing a header rebuilds what depends on it.
$(FILES:+".exe"): macho.h
pckmo.exe qedit.exe: machopc.h ieantc.h

%.exe : %.c
	$(CC) $(CFLAGS) -o $*.s $(INCS) $*.c
	$(AS) $(AFLAGS) -o $*.o -I $(MISSING) $*.s 
	$(LD) $(LFLAGS) -o "//'$(LOAD)($*)'" $*.o -l "//'SYS1.CSSLIB'" -l "//'$(LOAD)'"
	$(CC) $(CFLAGS64) -qlist -o "//'$(LOAD64)($*)'" $(INCS) $*.c -l "//'SYS1.CSSLIB'" -l "//'$(LOAD64)'" > $*.list64
	touch $*.exe

clean :
	rm $(FILES:+".s") $(FILES:+".o") $(FILES:+".exe") $(FILES:+".list64") \
	   machopc.s machopc.o machopc.exe macpcreg.exe macho

