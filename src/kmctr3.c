/*
 * KMCTR3 - CIPHER MESSAGE WITH COUNTER with CC=3 loop and static buffers
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(KMCTR3)
#else
#pragma prolog(KMCTR3, main)
#pragma epilog(KMCTR3, main)
#endif

long KMCTR3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_STRING    R1static,      /* (input/output) R1   */
        MACHO_STRING    R3static,      /* (input/output) R3   */
        MACHO_STRING    R2static,      /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       GR0, R2plus1, CC, invokes, ticks, dummy1, dummy3;
    unsigned long long  start, end;
    unsigned char       *R1, *R3, *R2;

    GR0 = *pGR0;
    R1 = R1static;
    R3 = R3static;
    R2 = R2static;
    R2plus1 = *pR2plus1;

    /* loop until condition code is not equal to 3 and R2+1 != 0 (in case usage counter could not be updated) */
    CC = 3;
    invokes = 0;
    ticks = 0;
    while (CC == 3 && R2plus1 != 0) {

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

        invokes++;
        ticks += (unsigned long)(end - start);
    }

    *pR2plus1 = R2plus1;
    *pCC = CC;
    *pinvokes = invokes;
    *pticks = ticks;

    return CC;
}
