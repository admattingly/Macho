/* machopc.c - PC routine */

#define _MI_BUILTIN  1
#include <builtins.h>
#include <stdio.h>
#include <string.h>

#include "machopc.h"

#define PERMIT_CLASS            "FACILITY"
#define PERMIT_PROFILE_PREFIX   "MACHO"

/* RACF profile name suffixes protecting verbs */
#define MACPROF_PCKMO           "PCKMO"
#define MACPROF_QEDIT           "QEDIT"

#pragma prolog(main, "MACPCPRO")
#pragma epilog(main, "MACPCEPI")

static int pckmo(void *parm);
static int qedit(void *parm);
static int fastauth(char *verb);

int main(void *work_area, void *parm)
{
    PBLOCK_COMMON   pb;

    pb = (PBLOCK_COMMON)parm;
    pb->return_code = MACERR_NOTAUTH;
    switch (pb->verb) {
    case MACVERB_PCKMO:
        if (fastauth(MACPROF_PCKMO)) {
            pb->return_code = pckmo(parm);
        }
        break;
    case MACVERB_QEDIT:
        if (fastauth(MACPROF_QEDIT)) {
            pb->return_code = qedit(parm);
        }
        break;
    default:
        pb->return_code = MACERR_NOTIMPL;
        break;
    }

    return 0;
}

int pckmo(void *parm)
{
    PBLOCK_PCKMO    pb;

    pb = (PBLOCK_PCKMO)parm;
    __asm(" PCKMO"
          :
          : "NR:r0"(pb->gr0),
            "NR:r1"(pb->gr1)
          : );

    return MACERR_SUCCESS;
}

int qedit(void *parm)
{
    PBLOCK_QEDIT    pb;
    char            save;

    /* switch to key 0 for QEDIT call */
    pb = (PBLOCK_QEDIT)parm;
    __asm(" MODESET EXTKEY=ZERO,SAVEKEY=%[save],WORKREG=2\n"
          " QEDIT ORIGIN=(%[origin])                     \n"
          " MODESET KEYADDR=%[save],WORKREG=2              "
          : [save]        "=m"(save),
                     "=NR:r15"(pb->gr15)
          : [origin]       "r"(pb->origin)
          : "r0", "r1", "r2", "r14", "r15");

    return MACERR_SUCCESS;
}

__asm(" RACROUTE REQUEST=FASTAUTH,RELEASE=2.6,MF=L" : "DS"(LfFast));

int fastauth(char *verb)
{
    unsigned int    wa[128], wa2[16], safRc, *prc;
    unsigned char   class[9], profile[256];
    __asm(" RACROUTE REQUEST=FASTAUTH,RELEASE=2.6,MF=L" : "DS"(plist));

    plist = LfFast;
    memset(class, ' ', sizeof(class));
    memcpy(class, PERMIT_CLASS, strlen(PERMIT_CLASS));
    sprintf(profile + 4, "%s.%s", PERMIT_PROFILE_PREFIX, verb);
    *(unsigned short *)(void *)profile = (unsigned short)strlen(profile + 4);
    *(unsigned short *)(void *)(profile + 2) = (unsigned short)strlen(profile + 4);
    __asm(" SAM31\n"
          " RACROUTE REQUEST=FASTAUTH,"
                    "RELEASE=2.6,"
                    "CLASS=%[class],"
                    "ENTITYX=%[prof],"
                    "WORKA=%[work],"
                    "WKAREA=%[work2],"
                    "MF=(E,(%[parm]))\n"
          " SAM64"
          :         "=NR:r15"(safRc)
          : [class]       "m"(class),
            [prof]        "m"(profile),
            [work]        "m"(wa),
            [work2]       "m"(wa2),
            [parm]        "r"(&plist)
          : "r0", "r1", "r14", "r15");
    prc = (void *)&plist;

    return (safRc == 0 && prc[0] == 0 && prc[1] == 0);
}
