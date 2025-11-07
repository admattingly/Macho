/*
 * EXTRACT - (z/OS Macro) Extract TCB Information
 */

/* Format of answer area is 12 fullwords with contents as follows:
 *
 *    Word   Content
 *      0    GRS       Address of general register save area
 *      1    FRS       Address of floating-point register save area
 *      2    reserved
 *      3    AETX      Address of end-of-task exit routine
 *      4    PRI       The current limit (third byte) and dispatching (fourth byte) priorities of the task
 *      5    CMC       Task completion code
 *      6    TIOT      Address of task input-output table
 *      7    COMM      Address of command scheduler communications list
 *      8    TSO       Address of TSO byte (high order bit is 1 if foreground TSO task, else zero)
 *      9    PSB       Address of TSO protected step control block
 *     10    TJID      ASID for foreground TSO task, else zero
 *     11    ASID      Address space identifier
 */

#include <string.h>

#include "macho.h"

#ifdef __64BIT__
#include <stdio.h>
#pragma export(EXTRACT)
#else
#pragma prolog(EXTRACT, main)
#pragma epilog(EXTRACT, main)
#endif

long EXTRACT(
        MACHO_STRING    answerarea,    /* (output)       12-fullword (48-byte) area to receive output */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned char       *aa = answerarea;
    unsigned long long  start, end, ticks, aaticks;
    unsigned char       *parm;
    long                aalen, aarc;

    /* allocate 31-bit answer area and parm block */
    aalen = 48 + 16;
    ALLOC(&aa, &aalen, &aarc, &aaticks);
    parm = aa + 48;

    __asm(" SYSSTATE ARCHLVL=2\n"
          " STCKF    %[start]\n"
#ifdef __64BIT__
          " SAM31\n"
#endif
          " EXTRACT  (%[answer]),'S',FIELDS=(ALL,COMM,TSO,PSB,TJID,ASID),MF=(E,%[parm])\n"
#ifdef __64BIT__
          " SAM64\n"
#endif
          " STCKF    %[end]"
          : [start]  "=m"(start),
            [end]    "=m"(end)
          : [answer]  "r"((int)aa),
            [parm]    "m"(parm[0])
          : "r0", "r1", "r14", "r15");

    memcpy(answerarea, aa, 48);
    MCFREE(&aa, &aalen, &aarc, &aaticks);
    ticks = end - start;
    *pticks = ticks;

    return 0;
}
