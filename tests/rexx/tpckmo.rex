/* REXX */
/* invoke PCKMO to wrap a 128-bit key */
/*
   To run:
   
   $ STEPLIB=MACHO.PDSE ./tpckmo

*/
GR0 = d2c(2, 4)           /* wrap 128-bit DES key */
/* clear key in 40-byte parameter block */
GR1 = left('CCC9BB47599972534BC600608B7E5918'x, 40, '00'x)
PcRc = d2c(0, 4)
ticks = d2c(0, 4)

address linkpgm "PCKMO" ,
  "GR0 GR1 PcRc ticks"

say 'PcRc: ' c2d(PcRc)
say 'ticks:' c2d(ticks) / 4096.0 'microseconds'
say 'GR1:  ' c2x(GR1)
