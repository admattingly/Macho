/*
 * QEDIT - (z/OS Macro) Command input buffer manipulation
 */

/* This macro takes 3 input parameters.  The ORIGIN parameter must be supplied.  This is
 * a MACHO_POINTER to the CIB chain returned by the EXTRACT macro.
 *
 * The following combinations of the BLOCK (MACHO_POIINTER) and CIBCTR (MACHO_INTEGER) parameters are valid:
 *
 * 1)  BLOCK is null address, CIBCTR < 0:
 *         BLOCK and CIBCTR are omitted from QEDIT macro call causing entire CIB chain to
 *         be deleted, provided caller has READ access to MACHO.QEDIT in FACILITY class
 *
 * 2)  BLOCK is not null, CIBCTR is any value:
 *         CIB at BLOCK address is deleted
 *
 * 3)  BLOCK is null, CIBCTR >= 0:
 *         limit is set for the number of CIBs to be chained at any time for this task
 *
 */

#include <string.h>

#include "ieantc.h"
#include "macho.h"
#include "machopc.h"

#ifdef __64BIT__
#include <stdlib.h>
#pragma export(QEDIT)
#else
#pragma prolog(QEDIT, main)
#pragma epilog(QEDIT, main)
#endif

long QEDIT(
        MACHO_POINTER   pOrigin,       /* (input)        CIB address chain pointer */
        MACHO_POINTER   pBlock,        /* (input)        CIB address pointer */
        MACHO_INTEGER   pCibCtr,       /* (input)        limit for the number of CIBs to be chained at any time for this task */
        MACHO_INTEGER   pRc,           /* (output)       return code from QEDIT call */
        MACHO_INTEGER   pPcRc,         /* (output)       PC routine return code (if BLOCK = null and CIBCTR < 0) */
        MACHO_DWORD     pticks)        /* (output)       elapsed clock ticks */
{
    unsigned char       *origin, *block;
    unsigned long       PcRc, MacroRc;
    unsigned long long  start, end, ticks;
    unsigned char       *parm;
    long                cibctr;
    BLOCK_QEDIT         blk;
    int                 level, rc;
    unsigned char       token[16], work_area[PC_WORK_AREA], *pwork;
    unsigned int        *lx;
    unsigned long long  plist[2];

    origin = *pOrigin;
    block = *pBlock;
    cibctr = *pCibCtr;

    if (block == NULL && cibctr < 0) {
        /* Delete entire CIB chain - requires supervisor state */
        blk.common.verb = MACVERB_QEDIT;
        blk.origin = (unsigned long long)(void *)origin;
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
        *pRc = 0;
        if (PcRc == MACERR_SUCCESS) *pRc = blk.gr15;
        *pPcRc = PcRc;
        *pticks = ticks;
    }
    else if (block != NULL) {
        __asm(
#ifdef __64BIT__
              " SYSSTATE ARCHLVL=2,AMODE64=YES\n"
#endif
              " STCKF %[start]                           \n"
              " QEDIT ORIGIN=(%[origin]),BLOCK=(%[block])\n"
              " STCKF %[end]                               "
              : [start]       "=m"(start),
                [end]         "=m"(end),
                         "=NR:r15"(MacroRc)
              : [origin]       "r"(origin),
                [block]        "r"(block)
              : "r0", "r1", "r14", "r15");
        *pRc = MacroRc;
        *pPcRc = MACERR_SUCCESS;
        ticks = end - start;
        *pticks = ticks;
    }
    else if (block == NULL && cibctr >= 0) {
        __asm(
#ifdef __64BIT__
              " SYSSTATE ARCHLVL=2,AMODE64=YES\n"
#endif
              " STCKF %[start]                           \n"
              " QEDIT ORIGIN=(%[origin]),CIBCTR=(%[cibctr])\n"
              " STCKF %[end]                               "
              : [start]       "=m"(start),
                [end]         "=m"(end),
                         "=NR:r15"(MacroRc)
              : [origin]       "r"(origin),
                [cibctr]       "r"(cibctr)
              : "r0", "r1", "r14", "r15");
        *pRc = MacroRc;
        *pPcRc = MACERR_SUCCESS;
        ticks = end - start;
        *pticks = ticks;
    }
    else {
        *pRc = 8;   /* indicate error return from macro */
        *pPcRc = MACERR_SUCCESS;
        *pticks = 0;
    }

    return 0;
}
