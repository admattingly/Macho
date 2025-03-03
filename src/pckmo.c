/*
 * PCKMO - PERFORM CRYPTOGRAPHIC KEY MANAGEMENT OPERATION
 */
#include "ieantc.h"
#include "macho.h"
#include "machopc.h"

#ifdef __64BIT__
#include <stdlib.h>
#pragma export(PCKMO)
#else
#pragma prolog(PCKMO, main)
#pragma epilog(PCKMO, main)
#endif

long PCKMO(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_INTEGER   pPcRc,         /* (output)       PC routine return code */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned long       PcRc;
    unsigned long long  start, end, ticks;
    BLOCK_PCKMO         blk;
    int                 level, rc;
    unsigned char       token[16], work_area[PC_WORK_AREA], *pwork;
    unsigned int        *lx;
    unsigned long long  plist[2];

    blk.common.verb = MACVERB_PCKMO;
    blk.gr0 = (unsigned long long)*pGR0;
    blk.gr1 = (unsigned long long)GR1;

    /* retrieve token value */
    level = IEANT_SYSTEM_LEVEL;
#ifdef __64BIT__
    IEAN4RT(&level, PC_TOKEN_NAME, token, &rc);
#else
    IEANTRT(&level, PC_TOKEN_NAME, token, &rc);
#endif
    ticks = 0;
    if (rc == 0) {
        lx = (unsigned int *)(void *)token;     /* map token as array of fullwords */
        /* call the PC routine */
        rc = lx[0];     /* first word of extended linkage index */
        pwork = work_area;
#ifdef __64BIT__
        pwork = __malloc31(PC_WORK_AREA);
#endif
        plist[0] = (unsigned long long)(void *)pwork;
        plist[1] = (unsigned long long)(void *)&blk;
        __asm(" STCKF %[start]\n"
              " SLLG  15,15,32  Place first word of LX into bits 0..31 of GR15\n"
              " PC    0(%[pc])\n"
              " STCKF %[end]"
              : [start]      "=m"(start),
                [end]        "=m"(end),
                        "+NR:r15"(rc)
              : [pc]          "r"(lx[1]),
                          "NR:r1"(plist)
              : "r1", "r15");
        PcRc = blk.common.return_code;
        ticks = end - start;
#ifdef __64BIT__
        free(pwork);
#endif
    }
    else {
        // can't retrieve name/token pair, so MACPCREG task not running?
        PcRc = MACERR_NOTREG;
    }
    *pPcRc = PcRc;
    *pticks = ticks;

    return *pPcRc;
}
