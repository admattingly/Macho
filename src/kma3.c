/*
 * KMA3 - CIPHER MESSAGE WITH AUTHENTICATION with CC=3 loop and static buffers
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(KMA3)
#else
#pragma prolog(KMA3, main)
#pragma epilog(KMA3, main)
#endif

long KMA3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_STRING    R1static,      /* (input/output) R1   */
        MACHO_STRING    R3static,      /* (input/output) R3   */
        MACHO_INTEGER   pR3plus1,      /* (input/output) R3+1 */
        MACHO_STRING    R2static,      /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       GR0, R3plus1, R2plus1, CC, invokes, dummy;
    unsigned long long  start, end, ticks;
    unsigned char       *R1, *R3, *R2;

    GR0 = *pGR0;
    R1 = R1static;
    R3 = R3static;
    R3plus1 = *pR3plus1;
    R2 = R2static;
    R2plus1 = *pR2plus1;

    /* loop until condition code is not equal to 3 */
    CC = 3;
    invokes = 0;
    ticks = 0;
    while (CC == 3) {

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

        invokes++;
        ticks += end - start;
    }

    *pR3plus1 = R3plus1;
    *pR2plus1 = R2plus1;
    *pCC = CC;
    *pinvokes = invokes;
    *pticks = ticks;

    return CC;
}
