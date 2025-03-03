/* REXX */
/*
 * An example of EXTRACT, QEDIT and WAIT macros being used to implement
 * a started task which reqponds to MODIFY commands and terminates when
 * a STOP command is issued.
 *
 * The PROC for this task would look like this:
 *
 *   //TQEDIT   PROC
 *   //STEP1    EXEC PGM=IRXJCL,PARM=TQEDIT
 *   //STEPLIB  DD  DISP=SHR,DSN=MACHO.PDSE
 *   //SYSEXEC  DD  DISP=SHR,DSN=MY.EXEC.LIB
 *   //SYSTSPRT DD  SYSOUT=*
 *   //SYSTSIN  DD  DUMMY
 *
 */
numeric digits 20

/* call EXTRACT to obtain COMM pointer */
say 'EXTRACTing TCB information...'
answerarea = copies('00'x, 48)
ticks = d2c(0, 8)

address linkpgm 'EXTRACT' ,
  'answerarea ticks'

com = c2x(substr(answerarea, 29, 4))
comecbpt = c2x(storage(com, 4))
comcibpt = c2x(storage(d2x(x2d(com) + 4), 4))
comtoken = c2x(storage(d2x(x2d(com) + 8), 4))
say '  COM addr='com
say '  COMECBPT='comecbpt
say '  COMCIBPT='comcibpt
say '  COMTOKEN='comtoken

/* if CIB pointer is not null, check for parms on the START command */
if comcibpt \= '00000000'x then do
  cibverb = storage(d2x(x2d(comcibpt) + 4), 1)          /* command verb code */
  cibdatln = c2d(storage(d2x(x2d(comcibpt) + 14), 2))   /* length of CIBDATA */
  cibdata = storage(d2x(x2d(comcibpt) + 16), cibdatln)  /* data from command */
  if cibdatln = 0 then
    say '  Invocation: START TQEDIT'
  else
    say '  Invocation: START TQEDIT,,,'cibdata
  
  /* delete START CIB */
  say 'Deleting START CIB...'
  origin = d2c(x2d(com) + 4, 4)
  block = x2c(comcibpt)
  cibctr = d2c(-1, 4)
  MacroRc = d2c(0, 4)
  PcRc = d2c(0, 4)
  ticks = d2c(0, 8)
  
  address linkpgm 'QEDIT' ,
    'origin block cibctr MacroRc PcRc ticks'
  
  say '  QEDIT ORIGIN='c2x(origin)',BLOCK='c2x(block)' rc='c2d(MacroRc)
  
  /* allow MODIFY commands */
  say 'Setting CIBCTR to allow MODIFY commands...'
  cibctr = d2c(5, 4)
  block = d2c(0, 4)

  address linkpgm 'QEDIT' ,
    'origin block cibctr MacroRc PcRc ticks'
  
  say '  QEDIT ORIGIN='c2x(origin)',CIBCTR='c2d(cibctr) 'rc='c2d(MacroRc)

  /* wait for MODIFY or STOP command */
  done = 0
  do while done = 0
    say 'Waiting for MODIFY or STOP command...'
    events = d2c(1, 4)
    ecblist = bitor(x2c(comecbpt), '80000000'x)
    ticks = d2c(0, 8)
    
    address linkpgm 'WAIT' ,
      'events ecblist ticks'
    
    say '  WAIT ended after' c2d(ticks) / 4096000000 'seconds'
    
    /* examine CIB to determine command issued */
    comcibpt = c2x(storage(d2x(x2d(com) + 4), 4))
    cibverb = storage(d2x(x2d(comcibpt) + 4), 1)          /* command verb code */
    cibdatln = c2d(storage(d2x(x2d(comcibpt) + 14), 2))   /* length of CIBDATA */
    cibdata = storage(d2x(x2d(comcibpt) + 16), cibdatln)  /* data from command */
    if cibverb = '40'x then do
      say 'STOP command has been issued'
      done = 1
    end
    else if cibverb = '44'x then do
      say 'MODIFY TQEDIT,'cibdata' has been issued'
    end
    else do
      say 'Unexpected command code '''c2x(cibverb)'''x has been issued - terminating!'
      done = 1
    end
    
    /* delete CIB */
    say 'Deleting CIB...'
    origin = d2c(x2d(com) + 4, 4)
    block = x2c(comcibpt)
    cibctr = d2c(-1, 4)
    MacroRc = d2c(0, 4)
    PcRc = d2c(0, 4)
    ticks = d2c(0, 8)
  
    address linkpgm 'QEDIT' ,
      'origin block cibctr MacroRc PcRc ticks'
  
    say '  QEDIT ORIGIN='c2x(origin)',BLOCK='c2x(block)' rc='c2d(MacroRc)
  end
  
  /* delete entire CIB chain, just to test privileged operation.  This step is not normally required! */
  say 'Deleting entire CIB chain...'
  block = d2c(0, 4)
  cibctr = d2c(-1, 4)

  address linkpgm 'QEDIT' ,
    'origin block cibctr MacroRc PcRc ticks'
  
  say '  QEDIT ORIGIN='c2x(origin)' rc='c2d(MacroRc)', PcRc='c2d(PcRc)
  
end
say 'Processing complete!'
return 0
