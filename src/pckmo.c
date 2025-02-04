/*
 * PCKMO - PERFORM CRYPTOGRAPHIC KEY MANAGEMENT OPERATION
 */
#include "ieantc.h"
#include "machopc.h"

#ifdef __64BIT__
#pragma export(PCKMO)
#else
#pragma prolog(PCKMO, main)
#pragma epilog(PCKMO, main)
#endif

long PCKMO(
        unsigned long   *pGR0,          /* input integer */
        unsigned char   *GR1,           /* input/output string */
        unsigned long   *pPcRc,         /* output integer */
        unsigned long   *pticks)        /* output integer */
{
    unsigned long       PcRc, ticks;
    unsigned long long  start, end;
    BLOCK_PCKMO         blk;
    int                 level, rc;
    unsigned char       token[16], work_area[PC_WORK_AREA];
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
        plist[0] = (unsigned long long)(void *)work_area;
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
        ticks = (unsigned long)(end - start);
    }
    else {
        // can't retrieve name/token pair, so MACPCREG task not running?
        PcRc = MACERR_NOTREG;
    }
    *pPcRc = PcRc;
    *pticks = ticks;

    return *pPcRc;
}
