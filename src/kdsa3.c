/*
 * KDSA3 - COMPUTE DIGITAL SIGNATURE AUTHENTICATION with CC=3 loop and static buffers
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(KDSA3)
#else
#pragma prolog(KDSA3, main)
#pragma epilog(KDSA3, main)
#endif

long KDSA3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_STRING    R1static,      /* (ignored)      R1   */
        MACHO_INTEGER   pR1plus1,      /* (ignored)      R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       GR0, R1plus1, R2plus1, CC, invokes, ticks;
    unsigned long long  start, end;
    unsigned char       *R1, *R2;

    GR0 = *pGR0;
//  R1 = R1static;
//  R1plus1 = *pR1plus1;
    R2 = R2static;
    R2plus1 = *pR2plus1;

    /* loop until condition code is not equal to 3 and R2+1 != 0 (in case usage counter could not be updated) */
    CC = 3;
    invokes = 0;
    ticks = 0;
    while (CC == 3 && R2plus1 != 0) {

        __asm(" STCKF %[start]        Get start timestamp            \n"
              " XGR   %[cc],%[cc]     Set CC register to zero        \n"
              " KDSA  0,%[in]         Invoke KDSA instruction        \n"
              " IPM   %[cc]           Catch program mask             \n"
              " SRL   %[cc],28        Extract condition code from it \n"
              " STCKF %[end]          Get end timestamp                "
              : [start]      "=m"(start),
                [end]        "=m"(end),
                [cc]         "=r"(CC),
//              [out]   "+RP:r:e"(R1),
//                           "+r"(R1plus1),
                [in]    "+RP:r:e"(R2),
                             "+r"(R2plus1)
              :           "NR:r0"(GR0),
                          "NR:r1"(GR1)
              : );

        invokes++;
        ticks += (unsigned long)(end - start);

    }

//  *pR1plus1 = R1plus1;
    *pR2plus1 = R2plus1;
    *pCC = CC;
    *pinvokes = invokes;
    *pticks = ticks;

    return CC;
}
