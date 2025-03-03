/*
 * COPYOUT - Utility function to copy from a buffer address held in a pointer to a string
 */

#include <string.h>

#include "macho.h"

#ifdef __64BIT__
#pragma export(COPYOUT)
#else
#pragma prolog(COPYOUT, main)
#pragma epilog(COPYOUT, main)
#endif

long COPYOUT(
        MACHO_POINTER   pbuffer,       /* (input)        buffer containing string to be copied */
        MACHO_INTEGER   pbufferLength, /* (input)        string length */
        MACHO_STRING    string,        /* (output)       string */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       bufferLength;
    unsigned long long  start, end, ticks;

    bufferLength = *pbufferLength;

    __asm(" STCKF %[start]                  Get start timestamp"
          : [start] "=m"(start)
          :
          : );

    memcpy(string, *pbuffer, bufferLength);

    __asm(" STCKF %[end]                    Get end timestamp"
          : [end]   "=m"(end)
          :
          : );

    ticks = end - start;
    *pticks = ticks;

    return 0;
}
