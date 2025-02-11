/*
 * STCKF - STORE CLOCK FAST
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(STCKF)
#else
#pragma prolog(STCKF, main)
#pragma epilog(STCKF, main)
#endif

long STCKF(
        MACHO_STRING    D2,            /* (output)       8-byte (64-bit) buffer */
        MACHO_INTEGER   pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       ticks;
    unsigned long long  start, end;

    __asm(" STCKF %[start]        Get start timestamp            \n"
          " STCKF %[clock]        Invoke STCKF instruction       \n"
          " STCKF %[end]          Get end timestamp                "
          : [start]      "=m"(start),
            [end]        "=m"(end),
            [clock]      "=m"(D2[0])
          :
          : );

    ticks = (unsigned long)(end - start);
    *pticks = ticks;

    return 0;
}
