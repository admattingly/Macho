/*
 * VCLFNH - VECTOR FP CONVERT AND LENGTHEN FROM NNP HIGH
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(VCLFNH)
#else
#pragma prolog(VCLFNH, main)
#pragma epilog(VCLFNH, main)
#endif

long VCLFNH(
        MACHO_STRING    V1,            /* (output)       V1 */
        MACHO_STRING    V2,            /* (input)        V2 */
        MACHO_INTEGER   pM3,           /* (input)        M3 */
        MACHO_INTEGER   pM4,           /* (input)        M4 */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       M3, M4;
    unsigned long long  start, end, ticks;

    M3 = *pM3;
    M4 = *pM4;

    switch(16 * M3 + M4) {
    case 16 * 2 + 0:
        __asm(" STCKF  %[start]       Get start timestamp            \n"
              " VL     2,%[src]       Load source vector             \n"
              " VCLFNH 1,2,2,0        Invoke VCLFNH instruction      \n"
              " VST    1,%[dest]      Store result vector            \n"
              " STCKF %[end]          Get end timestamp                "
              : [start]      "=m"(start),
                [end]        "=m"(end),
                [dest]       "=m"(V1[0])
              : [src]         "m"(V2[0])
              : );
        break;
    default:
        __asm(" VCLFNH 1,1,15,15      Generate exception"
              :
              :
              : );
        break;
    }

    ticks = end - start;
    *pticks = ticks;

    return 0;
}
