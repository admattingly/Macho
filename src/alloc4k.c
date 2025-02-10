/*
 * ALLOC4K - Utility function to allocate a memory buffer on a page boundary
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(ALLOC4K)
#else
#pragma prolog(ALLOC4K, main)
#pragma epilog(ALLOC4K, main)
#endif

long ALLOC4K(
        MACHO_POINTER   pbuffer,       /* (output)       buffer pointer */
        MACHO_INTEGER   pbufferLength, /* (input)        buffer length required */
        MACHO_INTEGER   preturnCode,   /* (output)       return code from STORAGE OBTAIN macro */
        MACHO_INTEGER   pticks)        /* (output)       elapsed clock ticks */
{
    unsigned char       *buffer;
    unsigned long       bufferLength, returnCode, ticks;
    unsigned long long  start, end;

    bufferLength = *pbufferLength;

    __asm(" SYSSTATE ARCHLVL=2                                            \n"
          " STCKF %[start]                             Get start timestamp\n"
          " STORAGE OBTAIN,BNDRY=PAGE,LENGTH=(%[len])  Allocate memory    \n"
          " STCKF %[end]                               Get end timestamp    "
          : [start]      "=m"(start),
            [end]        "=m"(end),
                     "=NR:r1"(buffer),
                    "=NR:r15"(returnCode)
          : [len]         "r"(bufferLength)
          : "r0", "r1", "r14", "r15");

    ticks = (unsigned long)(end - start);
    *pbuffer = buffer;
    *preturnCode = returnCode;
    *pticks = ticks;

    return returnCode;
}
