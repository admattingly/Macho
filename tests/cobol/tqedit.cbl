       identification division.
       program-id. tqedit.
      *
      * A simple started task using EXTRACT, QEDIT and WAIT
      *
      * To compile (31-bit):
      *
      * $ cob2 -o "//'MY.PDSE(TQEDIT)'" tqedit.cbl
      *
      * To compile (64-bit):
      *
      * $ cob2 -q64 -qdll -o "//'MY.PDSE(TQEDIT)'" tqedit.cbl
      *
      * The PROC for this task would look like this:
      *
      *   //TQEDIT   PROC
      *   //STEP1    EXEC PGM=TQEDIT
      *   //STEPLIB  DD  DISP=SHR,DSN=MY.PDSE
      *   //         DD  DISP=SHR,DSN=MACHO.PDSE (or MACHO.PDSE64)
      *
       environment division.
       data division.
       working-storage section.
       01  answer-area.
           05 answer   usage pointer-32 occurs 12.
       01  ticks       pic 9(18)  comp-5.
       01  com-ptr     usage pointer-32.
       01  qe-origin   usage pointer.
       01  qe-block    usage pointer.
       01  my-status   pic x.
           88  status-done    value '1'
                   when set to false is '0'.
       01  over-area.
           05  filler      pic x(4).
           05  over-ptr    usage pointer-32.
       01  over-hex    redefines over-area  pic 9(18) comp-5.
       01  wait-intval pic z(6)9.9(6).
       01  extract-fn  pic x(8)    value "EXTRACT".
       01  qedit-fn    pic x(8)    value "QEDIT".
       01  wait-fn     pic x(8)    value "WAIT".
       >>IF IGY-LP = 64
       01  qe-cibctr   pic s9(18)  comp-5.
       01  qe-rc       pic s9(18)  comp-5.
       01  qe-pcrc     pic s9(18)  comp-5.
       01  events      pic 9(18)   comp-5.
       >>ELSE
       01  qe-cibctr   pic s9(9)   comp-5.
       01  qe-rc       pic s9(9)   comp-5.
       01  qe-pcrc     pic s9(9)   comp-5.
       01  events      pic 9(9)    comp-5.
       >>END-IF
       linkage section.
       01  com.
           05  comecbpt  usage pointer-32.
           05  comcibpt  usage pointer-32.
           05  comtoken  usage pointer-32.
       01  cib.
           05  cibnext   usage pointer-32.
           05  cibverb   pic x.
               88  cibstart  value x'04'.
               88  cibstcom  value x'08'.
               88  cibmodfy  value x'44'.
               88  cibstop   value x'40'.
               88  cibmount  value x'0c'.
           05  ciblen    pic x.
           05  cibxoff   pic s9(4) comp-5.
           05  filler    pic x(2).
           05  cibasid   pic 9(4)  comp-5.
           05  filler    pic x.
           05  cibrsv1   pic x.
               88  cibs313   value x'01'.
               88  cibs410   value x'02'.
               88  cib15497  value x'03'.
               88  cibvrsn   value x'03'.
           05  cibdatln  pic 9(4)  comp-5.
           05  cibdata   pic x(128).
       procedure division.
      * call EXTRACT to obtain COMM pointer
           display 'Extracting TCB information...'
           call extract-fn using
               answer-area ticks
           set com-ptr to answer(8)
           set address of com to com-ptr
           display 'COM addr=' function hex-of(com-ptr)
           display 'COMECBPT=' function hex-of(comecbpt)
           display 'COMCIBPT=' function hex-of(comcibpt)
           display 'COMTOKEN=' function hex-of(comtoken)
      * if CIB pointer not null, check for parms on the START command
           if comcibpt is not equal to null then
             set address of cib to comcibpt
             if cibdatln = 0 then
               display 'Invocation: START TQEDIT'
             else
               display 'Invocation: START TQEDIT,,,'
                   cibdata(1:cibdatln)
             end-if
      * delete START CIB
             display 'Deleting START CIB...'
             set qe-origin to address of comcibpt
             set qe-block  to comcibpt
             move -1 to qe-cibctr
             call qedit-fn using
                 qe-origin qe-block qe-cibctr qe-rc qe-pcrc ticks
             display '  QEDIT ORIGIN=' function hex-of(qe-origin)
                 ',BLOCK=' function hex-of(qe-block)
                 ' rc=' qe-rc
      * allow MODIFY commands */
             display 'Setting CIBCTR to allow MODIFY commands...'
             move 5 to qe-cibctr
             set qe-block to null
             call qedit-fn using
                 qe-origin qe-block qe-cibctr qe-rc qe-pcrc ticks
             display '  QEDIT ORIGIN=' function hex-of(qe-origin)
                 ',CIBCTR=' qe-cibctr
                 ' rc=' qe-rc
      * wait for MODIFY or STOP command */
             set status-done to false
             perform until status-done
               display 'Waiting for MODIFY or STOP command...'
               move 1 to events
               move low-values to over-area
               set over-ptr to comecbpt
               add 2147483648 to over-hex
               call wait-fn using
                   events over-ptr ticks
               divide ticks by 4096000000 giving wait-intval
               display '  WAIT ended after ' wait-intval ' seconds'
               set address of cib to comcibpt
               if cibstop then
                 set status-done to true
                 display 'STOP command has been issued'
               else
                 if cibmodfy then
                   display 'MODIFY TQEDIT,' cibdata(1:cibdatln)
                       ' command has been issued'
                 else
                   set status-done to true
                   display 'Unexpected command code 0x'
                       function hex-of(cibverb)
                       ' has been issued - terminating!'
                 end-if
               end-if
      * delete CIB
               display 'Deleting CIB...'
               set qe-origin to address of comcibpt
               set qe-block  to comcibpt
               move -1 to qe-cibctr
               call qedit-fn using
                   qe-origin qe-block qe-cibctr qe-rc qe-pcrc ticks
               display '  QEDIT ORIGIN=' function hex-of(qe-origin)
                   ',BLOCK=' function hex-of(qe-block)
                   ' rc=' qe-rc
             end-perform
      * Delete entire CIB chain, just to test privileged operation.
      * This step is not normally required! */
             display 'Deleting entire CIB chain...'
             set qe-origin to address of comcibpt
             set qe-block to null
             move -1 to qe-cibctr
             display '  Calling QEDIT(ORIGIN='
                 function hex-of(qe-origin)
                 ',BLOCK=' function hex-of(qe-block)
                 ',CIBCTR=' qe-cibctr
             call qedit-fn using
                 qe-origin qe-block qe-cibctr qe-rc qe-pcrc ticks
             display '  QEDIT ORIGIN=' function hex-of(qe-origin)
                 ' rc=' qe-rc ' PcRc=' qe-pcrc
           end-if
           display 'Processing complete!'
           stop run.