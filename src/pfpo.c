/*
 * PFPO - PERFORM FLOATING-POINT OPERATION
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(PFPO)
#else
#pragma prolog(PFPO, main)
#pragma epilog(PFPO, main)
#endif

long PFPO(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_INTEGER   pGR1,          /* (output)       GR1 */
        MACHO_STRING    FPR0,          /* (input/output) 8-byte (64 bit) buffer */
        MACHO_STRING    FPR2,          /* (input/output) 8-byte (64 bit) buffer */
        MACHO_STRING    FPR4,          /* (input/output) 8-byte (64 bit) buffer */
        MACHO_STRING    FPR6,          /* (input/output) 8-byte (64 bit) buffer */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       GR0, GR1, CC;
    unsigned long long  start, end, ticks;

    GR0 = *pGR0;
    GR1 = 0;        /* to avoid confusion */

    __asm(" STCKF %[start]        Get start timestamp            \n"
          " LD    0,%[fpr0]       Load floating-point register   \n"
          " LD    2,%[fpr2]       Load floating-point register   \n"
          " LD    4,%[fpr4]       Load floating-point register   \n"
          " LD    6,%[fpr6]       Load floating-point register   \n"
          " XGR   %[cc],%[cc]     Set CC register to zero        \n"
          " PFPO                  Invoke PFPO instruction        \n"
          " IPM   %[cc]           Catch program mask             \n"
          " SRL   %[cc],28        Extract condition code from it \n"
          " STD   0,%[fpr0]       Store floating-point register  \n"
          " STD   2,%[fpr2]       Store floating-point register  \n"
          " STD   4,%[fpr4]       Store floating-point register  \n"
          " STD   6,%[fpr6]       Store floating-point register  \n"
          " STCKF %[end]          Get end timestamp                "
          : [start]      "=m"(start),
            [end]        "=m"(end),
            [fpr0]       "+m"(FPR0[0]),
            [fpr2]       "+m"(FPR2[0]),
            [fpr4]       "+m"(FPR4[0]),
            [fpr6]       "+m"(FPR6[0]),
            [cc]         "=r"(CC),
                     "+NR:r1"(GR1)
          :           "NR:r0"(GR0)
          : );

    ticks = end - start;
    *pGR1 = GR1;
    *pCC = CC;
    *pticks = ticks;

    return CC;
}
