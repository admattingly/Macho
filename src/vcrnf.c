/*
 * VCRNF - VECTOR FP CONVERT AND ROUND TO NNP
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(VCRNF)
#else
#pragma prolog(VCRNF, main)
#pragma epilog(VCRNF, main)
#endif

long VCRNF(
        MACHO_STRING    V1,            /* (output)       V1 */
        MACHO_STRING    V2,            /* (input)        V2 */
        MACHO_STRING    V3,            /* (input)        V3 */
        MACHO_INTEGER   pM4,           /* (input)        M4 */
        MACHO_INTEGER   pM5,           /* (input)        M5 */
        MACHO_INTEGER   pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       ticks, M4, M5;
    unsigned long long  start, end;

    M4 = *pM4;
    M5 = *pM5;

    switch(0 * M4 + M5) {
    case 16 * 0 + 2:
        __asm(" STCKF  %[start]       Get start timestamp            \n"
              " VL     2,%[src2]      Load source vector             \n"
              " VL     3,%[src3]      Load source vector             \n"
              " VCRNF  1,2,3,0,2      Invoke VCRNF instruction       \n"
              " VST    1,%[dest]      Store result vector            \n"
              " STCKF %[end]          Get end timestamp                "
              : [start]      "=m"(start),
                [end]        "=m"(end),
                [dest]       "=m"(V1[0])
              : [src2]        "m"(V2[0]),
                [src3]        "m"(V3[0])
              : );
        break;
    default:
        __asm(" VCRNF 1,1,1,15,15     Generate exception"
              :
              :
              : );
        break;
    }

    ticks = (unsigned long)(end - start);
    *pticks = ticks;

    return 0;
}
