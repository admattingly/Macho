/*
 * VCNF - VECTOR FP CONVERT TO NNP
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(VCNF)
#else
#pragma prolog(VCNF, main)
#pragma epilog(VCNF, main)
#endif

long VCNF(
        MACHO_STRING    V1,            /* (output)       V1 */
        MACHO_STRING    V2,            /* (input)        V2 */
        MACHO_INTEGER   pM3,           /* (input)        M3 */
        MACHO_INTEGER   pM4,           /* (input)        M4 */
        MACHO_INTEGER   pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       ticks, M3, M4;
    unsigned long long  start, end;

    M3 = *pM3;
    M4 = *pM4;

    switch(16 * M3 + M4) {
    case 16 * 0 + 1:
        __asm(" STCKF  %[start]       Get start timestamp            \n"
              " VL     2,%[src]       Load source vector             \n"
              " VCNF   1,2,1,0        Invoke VCNF instruction        \n"
              " VST    1,%[dest]      Store result vector            \n"
              " STCKF %[end]          Get end timestamp                "
              : [start]      "=m"(start),
                [end]        "=m"(end),
                [dest]       "=m"(V1[0])
              : [src]         "m"(V2[0])
              : );
        break;
    default:
        __asm(" VCNF   1,1,15,15      Generate exception"
              :
              :
              : );
        break;
    }

    ticks = (unsigned long)(end - start);
    *pticks = ticks;

    return 0;
}
