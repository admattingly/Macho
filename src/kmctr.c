/*
 * KMCTR - CIPHER MESSAGE WITH COUNTER
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(KMCTR)
#else
#pragma prolog(KMCTR, main)
#pragma epilog(KMCTR, main)
#endif

long KMCTR(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_POINTER   pR3,           /* (input/output) R3   */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       GR0, R2plus1, CC, dummy1, dummy3;
    unsigned long long  start, end, ticks;
    unsigned char       *R1, *R3, *R2;

    GR0 = *pGR0;
    R1 = *pR1;
    R3 = *pR3;
    R2 = *pR2;
    R2plus1 = *pR2plus1;

    __asm(" STCKF %[start]             Get start timestamp            \n"
          " XGR   %[cc],%[cc]          Set CC register to zero        \n"
          " KMCTR %[out],%[ctr],%[in]  Invoke KMCTR instruction       \n"
          " IPM   %[cc]                Catch program mask             \n"
          " SRL   %[cc],28             Extract condition code from it \n"
          " STCKF %[end]               Get end timestamp                "
          : [start]      "=m"(start),
            [end]        "=m"(end),
            [cc]         "=r"(CC),
            [out]   "+RP:r:e"(R1),
#ifndef __64BIT__
                         "=r"(dummy1),
#endif
            [ctr]   "+RP:r:e"(R3),
#ifndef __64BIT__
                         "=r"(dummy3),
#endif
            [in]    "+RP:r:e"(R2),
                         "+r"(R2plus1)
          :           "NR:r0"(GR0),
                      "NR:r1"(GR1)
          : );

    ticks = end - start;
    *pR1 = R1;
    *pR3 = R3;
    *pR2 = R2;
    *pR2plus1 = R2plus1;
    *pCC = CC;
    *pticks = ticks;

    return CC;
}
