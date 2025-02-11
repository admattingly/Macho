/*
 * PCC - PERFORM CRYPTOGRAPHIC COMPUTATION
 */

#include "macho.h"

#ifdef __64BIT__
#pragma export(PCC)
#else
#pragma prolog(PCC, main)
#pragma epilog(PCC, main)
#endif

long PCC(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       GR0, CC, ticks;
    unsigned long long  start, end;

    GR0 = *pGR0;

    __asm(" STCKF %[start]        Get start timestamp            \n"
          " XGR   %[cc],%[cc]     Set CC register to zero        \n"
          " PCC                  Invoke PCC instruction         \n"
          " IPM   %[cc]           Catch program mask             \n"
          " SRL   %[cc],28        Extract condition code from it \n"
          " STCKF %[end]          Get end timestamp                "
          : [start]      "=m"(start),
            [end]        "=m"(end),
            [cc]         "=r"(CC)
          :           "NR:r0"(GR0),
                      "NR:r1"(GR1)
          : );

    ticks = (unsigned long)(end - start);
    *pCC = CC;
    *pticks = ticks;

    return CC;
}
