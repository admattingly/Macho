/*
 * WAIT - (z/OS Macro) Wait for one or more events
 */

#include "macho.h"

#ifdef __64BIT__
#include <stdio.h>
#include <string.h>
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
    unsigned char       *lowlist;
#ifdef __64BIT__
    unsigned int        *ecb;
    long                lowlen, n, rc;
#endif

    events = *pevents;

    lowlist = ecblist;
#ifdef __64BIT__
    /* how many ECBs in list */
    n = 0;
    ecb = (unsigned int *)(void *)ecblist;
    while ((ecb[n] & 0x80000000) == 0x00000000) n++;
    n++;
    lowlen = 4 * n;
    /* allocate ECB list in 31-bit memory */
    ALLOC(&lowlist, &lowlen, &rc, &ticks);
    /* copy ECB list into 31-bit memory */
    memcpy(lowlist, ecblist, lowlen);
#endif
    __asm(
#ifdef __64BIT__
          " SYSSTATE ARCHLVL=2,AMODE64=YES\n"
#endif
          " STCKF    %[start]\n"
          " WAIT     (%[events]),ECBLIST=(%[ecblst])\n"
          " STCKF    %[end]"
          : [start]  "=m"(start),
            [end]    "=m"(end)
          : [events]  "r"(events),
            [ecblst]  "r"(lowlist)
          : "r0", "r1", "r14", "r15");

    ticks = end - start;
    *pticks = ticks;

#ifdef __64BIT__
    FREE(&lowlist, &lowlen, &rc, &ticks);
#endif

    return 0;
}
