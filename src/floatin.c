/*
 * FLOATIN - Utility function to convert a string to IEEE 754 single-length floating-point value
 */

#define _ISOC99_SOURCE
#include <stdlib.h>
#include <string.h>

#include "macho.h"

#ifdef __64BIT__
#pragma export(FLOATIN)
#else
#pragma prolog(FLOATIN, main)
#pragma epilog(FLOATIN, main)
#endif

long FLOATIN(
        MACHO_STRING    text,          /* (input)        string */
        MACHO_INTEGER   ptextLength,   /* (input)        string length */
        MACHO_STRING    binary,        /* (output)       4-byte binary IEEE 754 single-length floating-point value */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned char       buffer[1024];
    unsigned long       textLength;
    unsigned long long  start, end, ticks;
    float               fp;

    textLength = *ptextLength;

    __asm(" STCKF %[start]                  Get start timestamp"
          : [start] "=m"(start)
          :
          : );

    memcpy(buffer, text, textLength);
    buffer[textLength] = '\0';
    fp = strtof(buffer, NULL);

    __asm(" STCKF %[end]                    Get end timestamp"
          : [end]   "=m"(end)
          :
          : );

    ticks = end - start;
    memcpy(binary, &fp, sizeof(float));
    *pticks = ticks;

    return 0;
}
