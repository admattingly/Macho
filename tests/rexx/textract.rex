/* REXX */
/* invoke EXTRACT macro to retrieve TCB information */
/*
   To run:
   
   $ STEPLIB=MACHO.PDSE ./textract

*/
field.1  = "GRS"
field.2  = "FRS"
field.3  = "reserved"
field.4  = "AETX"
field.5  = "PRI"
field.6  = "CMC"
field.7  = "TIOT"
field.8  = "COMM"
field.9  = "TSO"
field.10 = "PSB"
field.11 = "TJID"
field.12 = "ASID"

answerarea = copies('00'x, 48)
ticks = d2c(0, 4)

address linkpgm "EXTRACT" ,
  "answerarea ticks"

do i = 1 to 12
  say left(field.i, 8) c2x(substr(answerarea, i * 4 - 3, 4))
end
say 'ticks:      ' c2d(ticks) / 4096.0 'microseconds'
