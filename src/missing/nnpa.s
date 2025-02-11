         MACRO
&LABEL   NNPA
         PUSH  PRINT,NOPRINT
         PRINT NOGEN,NOPRINT
         DC    X'B39B'
         POP   PRINT,NOPRINT
         MEND
