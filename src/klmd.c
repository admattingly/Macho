/*
 * KLMD - COMPUTE LAST MESSAGE DIGEST
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(KLMD)
#else
#pragma prolog(KLMD, main)
#pragma epilog(KLMD, main)
#endif

long KLMD(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       GR0, R1plus1, R2plus1, CC;
    unsigned long long  start, end, ticks;
    unsigned char       *R1, *R2;

    GR0 = *pGR0;
    R1 = *pR1;
    R1plus1 = *pR1plus1;
    R2 = *pR2;
    R2plus1 = *pR2plus1;

    __asm(" STCKF %[start]        Get start timestamp            \n"
          " XGR   %[cc],%[cc]     Set CC register to zero        \n"
          " KLMD  %[out],%[in]    Invoke KLMD instruction        \n"
          " IPM   %[cc]           Catch program mask             \n"
          " SRL   %[cc],28        Extract condition code from it \n"
          " STCKF %[end]          Get end timestamp                "
          : [start]      "=m"(start),
            [end]        "=m"(end),
            [cc]         "=r"(CC),
            [out]   "+RP:r:e"(R1),
                         "+r"(R1plus1),
            [in]    "+RP:r:e"(R2),
                         "+r"(R2plus1)
          :           "NR:r0"(GR0),
                      "NR:r1"(GR1)
          : );

    ticks = end - start;
    *pR1 = R1;
    *pR1plus1 = R1plus1;
    *pR2 = R2;
    *pR2plus1 = R2plus1;
    *pCC = CC;
    *pticks = ticks;

    return CC;
}
