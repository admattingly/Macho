AS = as
AFLAGS =
CC = xlc
LD = ld
CFLAGS = -qin=all:nostp -qfloat=ieee "-Wc,supp(CCN3415,CCN3571)" -S -qmetal -qnosearch -I /usr/include/metal
CFLAGS64 = -q64 -qfloat=ieee -qasm -qasmlib=agilify.maclib:sys1.maclib:sys1.modgen -qin=all:nostp "-Wc,supp(CCN3415,CCN3571)" "-Wl,dll"
LFLAGS =
FILES = alloc alloc4k cmpsc cmpsc3 copyin copyout dfltcc dfltcc3 floatin floatout free kdsa kdsa3 kimd kimd3 \
        klmd klmd3 km km3 kma kma3 kmac kmac3 kmc kmc3 kmctr kmctr3 kmf kmf3 kmo kmo3 nnpa nnpa3 pcc pcc3 pfpo \
        prno prno3 sortl sortl3 stck stcke stckf vcfn vclfnh vclfnl vcnf vcrnf pckmo

macho : $(FILES:+".exe") machopc.exe macpcreg.exe
	touch macho

machopc.exe: machopc.c machopc.h MACPCPRO MACPCEPI
	$(CC) $(CFLAGS) -q64 -o $*.s $(INCS) $*.c
	$(AS) $(AFLAGS) -o $*.o -I .:AGILIFY.MACLIB $*.s 
	$(LD) $(LFLAGS) -breus=rent -o "//'MACHO.AUTH($*)'" $*.o
	touch $*.exe

macpcreg.exe: macpcreg.c machopc.h ieantc.h iezcom.h
	xlc -qasm -qasmlib=sys1.maclib "-Wl,ac=1" -o "//'MACHO.AUTH($*)'" $*.c
	touch macpcreg.exe
	
%.exe : %.c
	$(CC) $(CFLAGS) -o $*.s $(INCS) $*.c
	$(AS) $(AFLAGS) -o $*.o -I AGILIFY.MACLIB $*.s 
	$(LD) $(LFLAGS) -o "//'MACHO.PDSE($*)'" $*.o -l "//'SYS1.CSSLIB'"
	$(CC) $(CFLAGS64) -o "//'MACHO.PDSE64($*)'" $(INCS) $*.c -l "//'SYS1.CSSLIB'"
	touch $*.exe
	
clean :
	rm $(FILES:+".s") $(FILES:+".o") $(FILES:+".exe") machopc.exe macpcreg.exe macho

