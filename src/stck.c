/*
* STCK - STORE CLOCK
*/

#include "macho.h"

#ifdef __64BIT__
#pragma export(STCK)
#else
#pragma prolog(STCK, main)
#pragma epilog(STCK, main)
#endif

long STCK(
        MACHO_STRING    D2,            /* (output)       8-byte (64-bit) buffer */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long long  start, end, ticks;

    __asm(" STCKF %[start]        Get start timestamp            \n"
          " STCK  %[clock]        Invoke STCK instruction        \n"
          " STCKF %[end]          Get end timestamp                "
          : [start]      "=m"(start),
            [end]        "=m"(end),
            [clock]      "=m"(D2[0])
          :
          : );

    ticks = end - start;
    *pticks = ticks;

    return 0;
}
