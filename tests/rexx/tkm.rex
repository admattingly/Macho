/* REXX */
/* Invoke KM to compute the KVC/KCV of a DES key */
/*
   To run:
   
   $ STEPLIB=MACHO.PDSE ./tkm hex_key

*/
/* get key value from command line */
arg key .
if key = '' then do
  say 'Usage: tkm key_hex'
  return 8
end

/* convert hex string to binary */
key = x2c(key)

/* select KM function code depending on key size */
klen = length(key)
if klen = 8 then do
  functionCode = 1
end
else if klen = 16 then do
  functionCode = 2
end
else if klen = 24 then do
  functionCode = 3
end
else do
  say 'ERROR: key length ('klen') is invalid'
  return 12
end

/* allocate buffer for input */
inBuffer = d2c(0, 4)
bufferLength = d2c(8, 4)
returnCode = d2c(0, 4)
ticks = d2c(0, 8)
address linkpgm "ALLOC inBuffer bufferLength returnCode ticks"
say "ALLOC  inBuffer@="c2x(inBuffer)", length="c2d(bufferLength)", rc="c2d(returnCode)", time="c2d(ticks)/4096.0" microseconds"
if c2d(returnCode) \= 0 then do
  say "ERROR: ALLOC of input buffer failed"
  return 12
end

/* allocate buffer for output (same size as input) */
outBuffer = d2c(0, 4)
address linkpgm "ALLOC outBuffer bufferLength returnCode ticks"
say "ALLOC outBuffer@="c2x(outBuffer)", length="c2d(bufferLength)", rc="c2d(returnCode)", time="c2d(ticks)/4096.0" microseconds"
if c2d(returnCode) \= 0 then do
  address linkpgm "FREE inBuffer bufferLength returnCode ticks"
  say "FREE   inBuffer@="c2x(inBuffer)", length="c2d(bufferLength)", rc="c2d(returnCode)", time="c2d(ticks)/4096.0" microseconds"
  say "ERROR: ALLOC of output buffer failed"
  return 12
end

/* prime input buffer with nulls */
/* x = storage(c2x(inBuffer), 8, copies('00'x, 8)) */
string = copies('00'x, 8)
stringlen = d2c(8, 4)
address linkpgm "COPYIN string stringlen inBuffer ticks"

/* prepare to call KM */
functionCode = d2c(functionCode, 4)
parameterBlock = key
inPointer = inBuffer
inLength = bufferLength
outPointer = outBuffer
outLength = bufferLength  /* dummy parameter - not expecting KM to modify its value */

/* loop until condition code is not equal to 3 (i.e. partial completion) */
conditionCode = d2c(3, 4)
do while c2d(conditionCode) = 3
  say " pre KM fc="c2d(functionCode)", inPtr="c2x(inPointer)", inLen="c2d(inLength) || ,
      ", outPtr="c2x(outPointer)", outLen="c2d(outLength)", cc="c2d(conditionCode)
  address linkpgm "KM functionCode parameterBlock outPointer outLength" ,
                     "inPointer inLength conditionCode ticks"
  say "post KM fc="c2d(functionCode)", inPtr="c2x(inPointer)", inLen="c2d(inLength) || ,
      ", outPtr="c2x(outPointer)", outLen="c2d(outLength)", cc="c2d(conditionCode)", time="c2d(ticks)/4096.0" microseconds"
end

/* copy contents of output buffer */
bufferlen = d2c(8, 4)
string = copies('00'x, 8)
address linkpgm "COPYOUT outBuffer bufferlen string ticks"

/* display KVC if KM was successful */
if c2d(conditionCode) = 0 then do
  say " "
  say "KVC: "c2x(left(string, 3))
  say " "
end

/* free buffers */
address linkpgm "FREE inBuffer bufferLength returnCode ticks"
say "FREE   inBuffer@="c2x(inBuffer)", length="c2d(bufferLength)", rc="c2d(returnCode)", time="c2d(ticks)/4096.0" microseconds"
address linkpgm "FREE outBuffer bufferLength returnCode ticks"
say "FREE  outBuffer@="c2x(outBuffer)", length="c2d(bufferLength)", rc="c2d(returnCode)", time="c2d(ticks)/4096.0" microseconds"
