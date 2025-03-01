/*
 * WAIT - (z/OS Macro) Wait for one or more events
 */

#include "macho.h"

#ifdef __64BIT__
#include <stdio.h>
#pragma export(WAIT)
#else
#pragma prolog(WAIT, main)
#pragma epilog(WAIT, main)
#endif

long WAIT(
        MACHO_INTEGER   pevents,       /* (input)        number of events to wait for */
        MACHO_STRING    ecblist,       /* (input)        list of 31-bit ECB pointers (set high-order bit on last pointer) */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks (64-bit value to prevent overflow) */
{
    unsigned long       events;
    unsigned long long  start, end, ticks;

    events = *pevents;

    __asm(" SYSSTATE ARCHLVL=2\n"
          " STCKF    %[start]\n"
          " WAIT     (%[events]),ECBLIST=(%[ecblst])\n"
          " STCKF    %[end]"
          : [start]  "=m"(start),
            [end]    "=m"(end)
          : [events]  "r"(events),
            [ecblst]  "r"(ecblist)
          : "r0", "r1", "r14", "r15");

    ticks = end - start;
    *pticks = ticks;

    return 0;
}
