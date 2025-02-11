/*
 * FREE - Utility function to free a memory buffer allocated by ALLOC
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(FREE)
#else
#pragma prolog(FREE, main)
#pragma epilog(FREE, main)
#endif

long FREE(
        MACHO_POINTER   pbuffer,       /* (input)        buffer to be freed */
        MACHO_INTEGER   pbufferLength, /* (input)        buffer length */
        MACHO_INTEGER   preturnCode,   /* (output)       return code from STORAGE RELEASE macro */
        MACHO_INTEGER   pticks)        /* (output)       elapsed clock ticks */
{
    unsigned char       *buffer;
    unsigned long       bufferLength, returnCode, ticks;
    unsigned long long  start, end;

    buffer = *pbuffer;
    bufferLength = *pbufferLength;

    __asm(" SYSSTATE ARCHLVL=2                                 \n"
          " STCKF %[start]                                 Get start timestamp\n"
          " STORAGE RELEASE,ADDR=(%[addr]),LENGTH=(%[len]) Release memory     \n"
          " STCKF %[end]                                   Get end timestamp    "
          : [start]      "=m"(start),
            [end]        "=m"(end),
                    "=NR:r15"(returnCode)
          : [addr]        "r"(buffer),
            [len]         "r"(bufferLength)
          : "r0", "r1", "r14", "r15");

    ticks = (unsigned long)(end - start);
    *pticks = ticks;
    *preturnCode = returnCode;

    return returnCode;
}
