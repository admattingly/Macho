/*
 * STCKE - STORE CLOCK EXTENDED
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(STCKE)
#else
#pragma prolog(STCKE, main)
#pragma epilog(STCKE, main)
#endif

long STCKE(
        MACHO_STRING    D2,            /* (output)       16-byte (128-bit) buffer */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long long  start, end, ticks;

    __asm(" STCKF %[start]        Get start timestamp            \n"
          " STCKE %[clock]        Invoke STCKE instruction       \n"
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
