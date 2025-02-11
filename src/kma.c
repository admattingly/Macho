/*
 * KMA - CIPHER MESSAGE WITH AUTHENTICATION
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(KMA)
#else
#pragma prolog(KMA, main)
#pragma epilog(KMA, main)
#endif

long KMA(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_POINTER   pR3,           /* (input/output) R3   */
        MACHO_INTEGER   pR3plus1,      /* (input/output) R3+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       GR0, R3plus1, R2plus1, CC, ticks, dummy;
    unsigned long long  start, end;
    unsigned char       *R1, *R3, *R2;

    GR0 = *pGR0;
    R1 = *pR1;
    R3 = *pR3;
    R3plus1 = *pR3plus1;
    R2 = *pR2;
    R2plus1 = *pR2plus1;

    __asm(" STCKF %[start]             Get start timestamp            \n"
          " XGR   %[cc],%[cc]          Set CC register to zero        \n"
          " KMA   %[out],%[aad],%[in]  Invoke KMA instruction         \n"
          " IPM   %[cc]                Catch program mask             \n"
          " SRL   %[cc],28             Extract condition code from it \n"
          " STCKF %[end]               Get end timestamp                "
          : [start]      "=m"(start),
            [end]        "=m"(end),
            [cc]         "=r"(CC),
            [out]   "+RP:r:e"(R1),
#ifndef __64BIT__
                         "=r"(dummy),
#endif
            [aad]   "+RP:r:e"(R3),
                         "+r"(R3plus1),
            [in]    "+RP:r:e"(R2),
                         "+r"(R2plus1)
          :           "NR:r0"(GR0),
                      "NR:r1"(GR1)
          : );

    ticks = (unsigned long)(end - start);
    *pR1 = R1;
    *pR3 = R3;
    *pR3plus1 = R3plus1;
    *pR2 = R2;
    *pR2plus1 = R2plus1;
    *pCC = CC;
    *pticks = ticks;

    return CC;
}
