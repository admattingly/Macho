/*
 * PCC3 - PERFORM CRYPTOGRAPHIC COMPUTATION with CC=3 loop and static buffers
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(PCC3)
#else
#pragma prolog(PCC3, main)
#pragma epilog(PCC3, main)
#endif

long PCC3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       GR0, CC, invokes, ticks;
    unsigned long long  start, end;

    GR0 = *pGR0;

    /* loop until condition code is not equal to 3 and R2+1 != 0 (in case usage counter could not be updated) */
    CC = 3;
    invokes = 0;
    ticks = 0;
    while (CC == 3) {

        __asm(" STCKF %[start]        Get start timestamp            \n"
              " XGR   %[cc],%[cc]     Set CC register to zero        \n"
              " PCC                   Invoke PCC instruction         \n"
              " IPM   %[cc]           Catch program mask             \n"
              " SRL   %[cc],28        Extract condition code from it \n"
              " STCKF %[end]          Get end timestamp                "
              : [start]      "=m"(start),
                [end]        "=m"(end),
                [cc]         "=r"(CC)
              :           "NR:r0"(GR0),
                          "NR:r1"(GR1)
              : );

        invokes++;
        ticks += (unsigned long)(end - start);

    }

    *pCC = CC;
    *pinvokes = invokes;
    *pticks = ticks;

    return CC;
}
