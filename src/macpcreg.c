/* register Macho PC routine */

#define _POSIX_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/__messag.h>

#include "iezcom.h"
#include "ieantc.h"

#include "machopc.h"

#define PC_ROUTINE      "MACHOPC"

static void dump(unsigned char *buffer, int bufferLength);
static void *loadModule(char *module);
static int reserveIndex(unsigned int *lxlist);
static int createEntryTable(unsigned int *ptoken, unsigned int ep);
static int connectEntryTable(unsigned int *lxlist, unsigned int *tklist);
static int createNameTokenPair(char *name, unsigned char *token);
static void *getCommSched(void);
static int waitEcb(void *ecbptr);
static int deleteNameTokenPair(char *name);
static int destroyEntryTable(int token);
static int freeIndex(unsigned int *lxlist);
static int unloadModule(char *module);
static int setprob(void);
static int setsup(void);
static int say(char *msg);

int main(void)
{
    int             rc;
    unsigned int    ep, lxlist[3], tklist[2];
    struct com      *pcom;
    unsigned char   token[16];

    say("MACPCREG: Registering PC routine...");
    ep = (unsigned int)loadModule(PC_ROUTINE);
    if (ep != 0) {
        printf("PC routine loaded at %08X\n", ep);
        lxlist[0] = 1;      /* number of LX entries required */
        rc = reserveIndex(lxlist);
        if (rc == 0) {
            rc = createEntryTable(&tklist[1], ep);
            if (rc == 0) {
                tklist[0] = 1;  /* one token in the list */
                rc = connectEntryTable(lxlist, tklist);
                if (rc == 0) {
                    /* copy first (only) extended linkage index into token value */
                    memcpy(token, &lxlist[1], 8);
                    rc = createNameTokenPair(PC_TOKEN_NAME, token);
                    if (rc == 0) {

                        say("MACPCREG: Ready to service requests.");

                        /* wait around until told to stop */
                        pcom = getCommSched();
                        if (pcom != NULL) {
                            waitEcb(pcom->comecbpt);
                        }
                        else {
                            fprintf(stderr, "No COM available - not running as started task?\n");
                        }

                        say("MACPCREG: Shutting down...");

                        rc = deleteNameTokenPair(PC_TOKEN_NAME);
                    }
                }
                rc = destroyEntryTable(tklist[1]);
            }
            rc = freeIndex(lxlist);
        }
        rc = unloadModule(PC_ROUTINE);
    }

    say("MACPCREG: Shutdown complete.");

    return 0;
}

void *loadModule(char *module)
{
    char    ename[9];
    int     rc;
    void    *ep;

    memset(ename, ' ', 8);
    strncpy(ename, module, (strlen(module) < 8) ? strlen(module) : 8);

    setsup();
    __asm(" SYSSTATE ARCHLVL=2\n"
          " LOAD EPLOC=(%[name]),GLOBAL=YES"
          :         "=NR:r0"(ep),
                   "=NR:r15"(rc)
          : [name]       "r"(ename)
          : "r0", "r1", "r14", "r15");
    setprob();
    printf("LOAD EP=%s rc=%d\n", module, rc);
    if (rc != 0) {
        ep = NULL;
    }
    return  ep;
}

int reserveIndex(unsigned int *lxlist)
{
    int             rc;

    setsup();
    __asm(" SYSSTATE ARCHLVL=2\n"
          " IEABRCX DEFINE\n"
          " BALR  14,0\n"
          " USING *,14\n"
          " LXRES ELXLIST=%[list],REUSABLE=YES,SYSTEM=YES\n"
          " DROP  14"
          :        "=NR:r15"(rc)
          : [list]       "m"(lxlist[0])
          : "r0", "r1", "r14", "r15");
    setprob();
    printf("LXRES rc=%d\n", rc);
    if (rc == 0) {
        printf("system ELX=%08X %08X\n", lxlist[1], lxlist[2]);
    }

    return rc;
}

int createEntryTable(unsigned int *ptoken, unsigned int ep)
{
    int             rc, etplen, etpc1off, token;
    unsigned char   etparm[256], *etaddr;

    /*
     * Construct Entry Table for the PC routine
     */
    __asm("         SYSSTATE ARCHLVL=2\n"
          "         IEABRCX DEFINE\n"
          "         BALR  14,0\n"
          "         USING *,14\n"
          "         J     SETPARM\n"
          "ETPARMS  ETDEF TYPE=INITIAL\n"
          "ETPC1    ETDEF TYPE=ENTRY,ROUTINE=0,AKM=0\n"
          "         ETDEF TYPE=FINAL\n"
          "ETPLEN   EQU   *-ETPARMS\n"
          "ETPC1OFF EQU   ETPC1-ETPARMS\n"
          "SETPARM  LA    %[plen],ETPLEN\n"
          "         LA    %[p1off],ETPC1OFF\n"
          "         MVC   0(ETPLEN,%[parm]),ETPARMS\n"
          "         DROP  14"
          : [plen]      "=r"(etplen),
            [p1off]     "=r"(etpc1off),
                   "=NR:r15"(rc)
          : [parm]       "r"(etparm),
            [ep]         "r"(ep)
          : "r0", "r1", "r14", "r15");
    etaddr = etparm + etpc1off;
    __asm(" ETDEF TYPE=SET,ETEADR=(%[addr]),ROUTINE=(%[ep]),"
                  "SSWITCH=NO,AKM=(0:15),STATE=SUPERVISOR,"
                  "PC=STACKING,RAMODE=64"
          :
          : [addr] "r"(etaddr),
            [ep]   "r"(ep)
          : );
    printf("ETPARMS (%d bytes):\n", etplen);
    dump(etparm, 44);
    /*
     * Create an entry table for the PC routine
     */
    setsup();
    __asm(" SYSSTATE ARCHLVL=2\n"
          " ETCRE ENTRIES=(%[parm])"
          :         "=NR:r0"(token),
                   "=NR:r15"(rc)
          : [parm]       "r"(etparm)
          : "r0", "r1", "r14", "r15");
    setprob();
    printf("ETCRE rc=%d\n", rc);
    if (rc == 0) {
        *ptoken = token;
    }

    return rc;
}

int connectEntryTable(unsigned int *lxlist, unsigned int *tklist)
{
    int     rc;
    /*
     * Connect token list to LX list
     */
    setsup();
    __asm(" SYSSTATE ARCHLVL=2\n"
          " IEABRCX DEFINE\n"
          " BALR  14,0\n"
          " USING *,14\n"
          " ETCON ELXLIST=%[lx],TKLIST=%[tk]\n"  /* modifies R2 */
          " DROP  14"
          : "=NR:r15"(rc)
          : [lx] "m"(lxlist[0]),
            [tk] "m"(tklist[0])
          : "r0", "r1", "r2", "r14", "r15");
    setprob();
    printf("ETCON rc=%d\n", rc);

    return rc;
}

int createNameTokenPair(char *name, unsigned char *token)
{
    int             level, persist, rc;

    level = IEANT_SYSTEM_LEVEL;
    persist = IEANT_NOPERSIST;
    setsup();
    IEANTCR(&level, name, token, &persist, &rc);
    setprob();
    printf("IEANTCR rc=%d\n", rc);

    return rc;
}

void *getCommSched(void)
{
    unsigned int    parm[4], answer[2];

    memset(answer, 0x00, sizeof(answer));
    __asm(" EXTRACT %[answer],'S',FIELDS=(COMM,ASID),MF=(E,%[parm])"
          :
          : [answer] "m"(answer),
            [parm]   "m"(parm)
          : "r0", "r1", "r14", "r15");

    return (void *)(answer[0]);
}

int waitEcb(void *ecbptr)
{
    __asm(" WAIT ECB=(%[ecb])"
          :
          : [ecb] "r"(ecbptr)
          : "r0", "r1", "r14", "r15");

    return 0;
}

int deleteNameTokenPair(char *name)
{
    int     level, rc;

    level = IEANT_SYSTEM_LEVEL;
    setsup();
    IEANTDL(&level, name, &rc);
    setprob();
    printf("IEANTDL rc=%d\n", rc);

    return rc;
}

int destroyEntryTable(int token)
{
    int     rc;

    setsup();
    __asm(" SYSSTATE ARCHLVL=2\n"
          " IEABRCX DEFINE\n"
          " BALR  14,0\n"
          " USING *,14\n"
          " ETDES TOKEN=%[tk],PURGE=YES\n"
          " DROP  14"
          :      "=NR:r15"(rc)
          : [tk]       "m"(token)
          : "r0", "r1", "r14", "r15");
    setprob();
    printf("ETDES rc=%d\n", rc);

    return rc;
}

int freeIndex(unsigned int *lxlist)
{
    int     rc;

    setsup();
    __asm(" SYSSTATE ARCHLVL=2\n"
          " IEABRCX DEFINE\n"
          " BALR  14,0\n"
          " USING *,14\n"
          " LXFRE ELXLIST=%[list],FORCE=YES\n"
          " DROP  14"
          :    "=NR:r15"(rc)
          : [list]  "m"(lxlist[0])
          : "r0", "r1", "r14", "r15");
    setprob();
    printf("LXFRE rc=%d\n", rc);

    return rc;
}

int unloadModule(char *module)
{
    char    ename[9];
    int     rc;

    memset(ename, ' ', 8);
    strncpy(ename, module, (strlen(module) < 8) ? strlen(module) : 8);

    setsup();
    __asm(" SYSSTATE ARCHLVL=2\n"
          " DELETE EPLOC=(%[name])"
          :        "=NR:r15"(rc)
          : [name]       "r"(ename)
          : "r0", "r1", "r14", "r15");
    setprob();
    printf("DELETE EP=%s rc=%d\n", module, rc);

    return rc;
}

int setprob(void)
{
    int     rc;

    __asm( " SYSSTATE ARCHLVL=2\n"
           " MODESET MODE=PROB,KEY=NZERO"
         : "=NR:r15"(rc)
         :
         : "r0", "r1", "r14", "r15");

    return rc;
}

int setsup(void)
{
    int     rc;

    __asm( " SYSSTATE ARCHLVL=2\n"
           " MODESET MODE=SUP,KEY=ZERO"
         : "=NR:r15"(rc)
         :
         : "r0", "r1", "r14", "r15" );

    return rc;
}

/* define character for non-displayable code points */
#define NDC '.'

/* ASCII character set */
unsigned char asc[] = {
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
' ', '!', '"', '#', '$', '%', '&', '\'',
'(', ')', '*', '+', ',', '-', '.', '/',
'0', '1', '2', '3', '4', '5', '6', '7',
'8', '9', ':', ';', '<', '=', '>', '?',
'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
'X', 'Y', 'Z', '[', '\\',']', '^', '_',
NDC, 'a', 'b', 'c', 'd', 'e', 'f', 'g',
'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
'x', 'y', 'z', '{', '|', '}', '~', NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC};

/* EBCDIC character set 500 */
unsigned char ebc[] = {
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
' ', NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, '[', '.', '<', '(', '+', '!',
'&', NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, ']', '$', '*', ')', ';', '^',
'-', '/', NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, '|', ',', '%', '_', '>', '?',
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, '`', ':', '#', '@', '\'','=', '"',
NDC, 'a', 'b', 'c', 'd', 'e', 'f', 'g',
'h', 'i', NDC, NDC, NDC, NDC, NDC, NDC,
NDC, 'j', 'k', 'l', 'm', 'n', 'o', 'p',
'q', 'r', NDC, NDC, NDC, NDC, NDC, NDC,
NDC, '~', 's', 't', 'u', 'v', 'w', 'x',
'y', 'z', NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, NDC, NDC, NDC, NDC, NDC,
NDC, NDC, NDC, '|', NDC, NDC, NDC, NDC,
'{', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
'H', 'I', NDC, NDC, NDC, NDC, NDC, NDC,
'}', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
'Q', 'R', NDC, NDC, NDC, NDC, NDC, NDC,
'\\',NDC, 'S', 'T', 'U', 'V', 'W', 'X',
'Y', 'Z', NDC, NDC, NDC, NDC, NDC, NDC,
'0', '1', '2', '3', '4', '5', '6', '7',
'8', '9', NDC, NDC, NDC, NDC, NDC, NDC};

unsigned char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                       '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

void dump(unsigned char *buffer, int bufferLength)
{
    unsigned long       i, i16, i4, donesame;
    unsigned char       line[128], previous[128];
    int                 in = 6;     /* indent        */
    int                 pa = 0;     /* print ASCII?  */
    int                 pe = 1;     /* print EBCDIC? */
    int                 paoff = pa * 19;

    if (bufferLength <= 0) return;

    memset(line, ' ', sizeof(line));

    strcpy(previous, "");
    donesame = 0;
    for (i = 0; i < bufferLength; i++) {
        i16 = i % 16;
        i4  = i16 / 4;
        if (i16 == 0) {
            /* clear the line */
            memset(line, ' ', sizeof(line));
            /* print offset */
            sprintf(line + in, "%06X:", i);
            /* overtype the NUL */
            line[in + 7] = ' ';
            /* add bars */
            if (pa) {
                line[in + 44] = '|';
                line[in + 61] = '|';
            }
            if (pe) {
                line[in + paoff + 44] = '|';
                line[in + paoff + 61] = '|';
            }
        }
        /* write out hex value */
        line[in + 8 + i16 * 2 + i4] = hex[buffer[i] / 16];
        line[in + 9 + i16 * 2 + i4] = hex[buffer[i] % 16];
        if (pa) {
            line[in + 45 + i16] = asc[(int)buffer[i]];
        }
        if (pe) {
            line[in + paoff + 45 + i16] = ebc[(int)buffer[i]];
        }
        if (i16 == 15 || i == bufferLength - 1) {
            /* add a space to the last line, so that compare to previous fails */
            if (i == bufferLength - 1) {
                line[in + paoff + 63] = '\0';
            }
            else {
                line[in + paoff + 62] = '\0';
            }

            if (strcmp(line + in + 8, previous) == 0) {
                if (!donesame) {
                    printf("%*s        --------   same as above   --------\n", in, "");
                    donesame = 1;
                }
            }
            else {
                donesame = 0;
                printf("%s\n", line);
                strcpy(previous, line + in + 8);
            }
        }
    }
}

int say(char *msg)
{
    int                 rc;
    struct __cons_msg   cmsg;

    cmsg.__format.__f1.__msg = msg;
    cmsg.__format.__f1.__msg_length = strlen(msg);
    rc = __console(&cmsg, NULL, NULL);

    printf("%s\n", msg);

    return rc;
}
