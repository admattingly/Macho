/*
 * FLOATOUT - Utility function to convert an IEEE 754 single-length floating-point value to a string
 */

#include <stdio.h>
#include <string.h>

#include "macho.h"

#ifdef __64BIT__
#pragma export(FLOATOUT)
#else
#pragma prolog(FLOATOUT, main)
#pragma epilog(FLOATOUT, main)
#endif

long FLOATOUT(
        MACHO_STRING    binary,        /* (input)        4-byte binary IEEE 754 single-length floating-point value */
        MACHO_STRING    text,          /* (output)       string */
        MACHO_INTEGER   ptextLength,   /* (input/output) (maximum on input, actual on ouput) string length */
        MACHO_INTEGER   pticks)        /* (output)       elapsed clock ticks */
{
    unsigned char       buffer[1024];
    unsigned long       textLength, outLength, ticks;
    unsigned long long  start, end;
    float               fp;

    textLength = *ptextLength;
    fp = *(float *)(void *)binary;

    __asm(" STCKF %[start]                  Get start timestamp"
          : [start] "=m"(start)
          :
          : );

    sprintf(buffer, "%.10g", fp);
    outLength = textLength < strlen(buffer) ? textLength : strlen(buffer);
    memcpy(text, buffer, outLength);

    __asm(" STCKF %[end]                    Get end timestamp"
          : [end]   "=m"(end)
          :
          : );

    *ptextLength = outLength;
    ticks = (unsigned long)(end - start);
    *pticks = ticks;

    return 0;
}
