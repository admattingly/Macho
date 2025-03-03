/*
 * COPYIN - Utility function to copy a string to a buffer address held in a pointer
 */

#include <string.h>

#include "macho.h"

#ifdef __64BIT__
#pragma export(COPYIN)
#else
#pragma prolog(COPYIN, main)
#pragma epilog(COPYIN, main)
#endif

long COPYIN(
        MACHO_STRING    string,        /* (input)        string to be copied */
        MACHO_INTEGER   pstringLength, /* (input)        string length */
        MACHO_POINTER   pbuffer,       /* (input)        buffer to receive string */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       stringLength;
    unsigned long long  start, end, ticks;

    stringLength = *pstringLength;

    __asm(" STCKF %[start]                  Get start timestamp"
          : [start] "=m"(start)
          :
          : );

    memcpy(*pbuffer, string, stringLength);

    __asm(" STCKF %[end]                    Get end timestamp"
          : [end]   "=m"(end)
          :
          : );

    ticks = end - start;
    *pticks = ticks;

    return 0;
}
