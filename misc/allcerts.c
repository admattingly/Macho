/* get all certificates in RACF database */

/* To compile:
 *
 * $  xlc -qdll -qasm -qasmlib=sys1.maclib -oallcerts allcerts.c /usr/lib/GSKCMS31.x /usr/lib/GSKSSL.x
 */

#pragma langlvl(extc99)     // for stdbool.h

#define _LARGE_TIME_API
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <zos/irrpcomp.h>
#include <gskssl.h>
#include <gskcms.h>

/* storage parameters for R_admin input/output parameter block */
#define ADMIN_STORAGE_POOL      127
#define ADMIN_USER_KEY          8

/* size limits for certificate sizes, allowing for post-quantum bloat */
#define MAX_CERTIFICATE         16384
#define MAX_PRIVATE_KEY         16384
#define MAX_SUBJECTS_DN         1024

/* RACF callable services */

/* R_admin */
void IRRSEQ00(uint8_t   *Work_area,
              uint32_t  *Alet1,
              uint32_t  *SAF_return_code,
              uint32_t  *Alet2,
              uint32_t  *RACF_return_code,
              uint32_t  *Alet3,
              uint32_t  *RACF_reason_code,
              uint8_t   *Function_code,
              uint8_t   *Parm_list,
              uint8_t   *RACF_userID,
              void      *ACEE_ptr,
              uint8_t   *Out_message_subpool,
              void      **Out_message_strings);
#pragma linkage(IRRSEQ00, OS)

/* R_datalib */
void IRRSDL00(unsigned char *Work_area,
              unsigned int  *Alet1,
              unsigned int  *SAF_return_code,
              unsigned int  *Alet2,
              unsigned int  *RACF_return_code,
              unsigned int  *Alet3,
              unsigned int  *RACF_reason_code,
              unsigned char *Function_code,
              unsigned int  *attributes,
              unsigned char *RACF_userID,
              unsigned char *ringName,
              unsigned int  *parmListVersion,
              void          *Parm_list);
#pragma linkage(IRRSDL00, OS)

/* local function prototypes */
static void free31SpK(uint32_t sp, uint32_t key, uint32_t len, void *addr);
static void *get31SpK(uint32_t sp, uint32_t key, uint32_t len);
static uint32_t getCerts(char *user);
static bool getNextUser(char *user, void **pParmBlock, uint32_t *pParmBlockLength);
static void printCert(struct cddl_get_cert *pparm);

int main(int argc, char **argv)
{
    void                *pb;
    uint32_t            pblen;
    char                user[9];
    struct timeval64    tv0, tv1;
    uint32_t            nusers = 0;
    uint32_t            ncerts = 0;

    gettimeofday64(&tv0, NULL);

    /* print CSV heading line */
    printf("User,Label,Status,NotBefore,NotAfter,SerialNumber,Subject's DN,Issuer's DN\n");

    pb = NULL;
    strcpy(user, " ");
    while (getNextUser(user, &pb, &pblen)) {
        nusers++;
        ncerts += getCerts(user);
    }

    gettimeofday64(&tv1, NULL);
    fprintf(stderr, "%u users with %u certificates in %.6f seconds\n",
            nusers, ncerts, (tv1.tv_sec - tv0.tv_sec) + (tv1.tv_usec - tv0.tv_usec) / 1000000.0);

    return 0;
}

void free31SpK(uint32_t sp, uint32_t key, uint32_t len, void *addr)
{
    uint32_t    rc;

    /* Note: If key < 8, caller must be in supervisor state */

    /* release storage */
    key <<= 4;
    __asm( " SYSSTATE ARCHLVL=2\n"
           " STORAGE RELEASE,"
                    "ADDR=%[addr],"
                    "LENGTH=(%[len]),"
                    "SP=(%[sp]),"
                    "KEY=(%[key]),"
                    "COND=YES,"
                    "CALLRKY=NO"
           :        "=NR:r15"(rc)
           : [addr]       "m"(addr),
             [len]        "r"(len),
             [sp]         "r"(sp),
             [key]        "r"(key)
           : "r0", "r1", "r14", "r15");

    if (rc != 0) {
        fprintf(stderr, "FATAL: free31SpK failed, rc=%u\n", rc);
        exit(12);
    }

    return;
}

void *get31SpK(uint32_t sp, uint32_t key, uint32_t len)
{
    uint32_t    rc;
    void        *addr;

    /* Note: If key < 8, caller must be in supervisor state */

    /* obtain storage from nominated pool */
    key <<= 4;
    __asm( " SYSSTATE ARCHLVL=2\n"
           " STORAGE OBTAIN,"
                    "LENGTH=(%[len]),"
                    "SP=(%[sp]),"
                    "KEY=(%[key]),"
                    "LOC=31,"
                    "CALLRKY=NO"
           :        "=NR:r1"(addr),
                   "=NR:r15"(rc)
           : [len]       "r"(len),
             [sp]        "r"(sp),
             [key]       "r"(key)
           : "r0", "r1", "r14", "r15");
    if (rc != 0) {
        fprintf(stderr, "FATAL: get31SpK failed, rc=%u\n", rc);
        exit(12);
    }

    return addr;
}

uint32_t getCerts(char *user)
{
    uint8_t                     workArea[1024] __attribute__((__aligned__(8)));
    uint32_t                    alet1, alet2, alet3;
    uint32_t                    safRc, racfRc, racfReason;
    uint8_t                     fc;
    struct cddl_attributes_map  attributes;
    uint8_t                     racfUser[9];
    uint8_t                     ringName[2];
    uint32_t                    parmVer;
    struct cddl_handle_map      handle;
    struct cddl_get_cert        parm;
    uint8_t                     *certificate, *privateKey, *subjectsDn, *recordId;
    uint8_t                     label[32];
    uint32_t                    ncerts = 0;

    /* allocate space for potentially large variables */
    certificate = malloc(MAX_CERTIFICATE);
    privateKey  = malloc(MAX_PRIVATE_KEY);
    subjectsDn  = malloc(MAX_SUBJECTS_DN);
    recordId    = malloc(MAX_SUBJECTS_DN);  /* subject's DN dominates (typically) record ID */

    /* call R_datalib DataGetFirst function to get first certificate */
    alet1 = alet2 = alet3 = 0;  /* all parameters in primary address space */
    fc = CDDL_DATA_GET_FIRST;
    attributes.cddl_attr_map_dataget = 0;
    racfUser[0] = (uint8_t)strlen(user);
    memcpy(&racfUser[1], user, strlen(user));
    ringName[0] = 1;
    ringName[1] = '*';
    parmVer = 1;
    handle.cddl_predicates   = CDDL_NONE;
    parm.cddl_res_handle     = &handle;
    parm.cddl_cert_len       = MAX_CERTIFICATE;
    parm.cddl_cert_ptr       = certificate;
    parm.cddl_pk_len         = MAX_PRIVATE_KEY;
    parm.cddl_pk_ptr         = privateKey;
    parm.cddl_label_len      = sizeof(label);
    parm.cddl_label_ptr      = label;
    parm.cddl_racf_userid[0] = 8;   /* maximum user ID length */
    parm.cddl_sdn_len        = MAX_SUBJECTS_DN;
    parm.cddl_sdn_ptr        = subjectsDn;
    parm.cddl_recid_len      = MAX_SUBJECTS_DN;
    parm.cddl_recid_ptr      = recordId;
    *(uint32_t *)parm.cddl_status = 0;

    IRRSDL00(workArea, &alet1, &safRc, &alet2, &racfRc, &alet3, &racfReason,
             &fc, (uint32_t *)(void *)&attributes, racfUser, ringName, &parmVer, &parm);

    if (safRc == 0) {
        ncerts++;
        printCert(&parm);
        fc = CDDL_DATA_GET_NEXT;
        while (safRc == 0) {
            parm.cddl_cert_len       = MAX_CERTIFICATE;
            parm.cddl_pk_len         = MAX_PRIVATE_KEY;
            parm.cddl_label_len      = sizeof(label);
            parm.cddl_racf_userid[0] = 8;   /* maximum user ID length */
            parm.cddl_sdn_len        = MAX_SUBJECTS_DN;
            parm.cddl_recid_len      = MAX_SUBJECTS_DN;
            *(uint32_t *)parm.cddl_status = 0;

            IRRSDL00(workArea, &alet1, &safRc, &alet2, &racfRc, &alet3, &racfReason,
                     &fc, (uint32_t *)(void *)&attributes, racfUser, ringName, &parmVer, &parm);

            if (safRc == 0) {
                ncerts++;
                printCert(&parm);
            }
            else {
                if (safRc != 8 || racfRc != 8 || racfReason != 44) {    /* not "no more certs" */
                    printf("IRRSDL00 DataGetNext safrc=%d, racfrc=%d, racfreason=%d\n",
                           safRc, racfRc, racfReason);
                }
            }
       }
    }
    else {
        if (safRc != 8 || racfRc != 8 || racfReason != 44) {    /* not "no more certs" */
            printf("IRRSDL00 DataGetFirst safrc=%d, racfrc=%d, racfreason=%d\n",
                   safRc, racfRc, racfReason);
        }
    }

    /* abort query in all circumstances */
    fc = CDDL_DATA_ABORT;
    parmVer = 0;
    IRRSDL00(workArea, &alet1, &safRc, &alet2, &racfRc, &alet3, &racfReason,
             &fc, (uint32_t *)(void *)&attributes, racfUser, ringName, &parmVer, &parm);
    if (safRc != 0) {
        printf("IRRSDL00 DataAbort safrc=%d, racfrc=%d, racfreason=%d\n",
               safRc, racfRc, racfReason);
    }

    free(certificate);
    free(privateKey);
    free(subjectsDn);
    free(recordId);

    return ncerts;
}

bool getNextUser(char *user, void **pParmBlock, uint32_t *pParmBlockLength)
{
    uint8_t                 workArea[1024] __attribute__((__aligned__(8)));
    uint32_t                alet1, alet2, alet3;
    uint32_t                safRc, racfRc, racfReason;
    uint8_t                 fc, sp;
    struct admn_prof_map    *pprof;
    uint32_t                pblen;
    void                    *parmBlock, *outbuf;
    bool                    result;

    parmBlock = *pParmBlock;

    alet1 = alet2 = alet3 = 0;  /* all parameters in primary address space */
    fc = ADMN_XTR_NEXT_USER;
    sp = ADMIN_STORAGE_POOL;

    /* if parameter block has not been allocated, allocate and initialise it */
    if (parmBlock == NULL) {
        pblen = sizeof(struct admn_prof_map) + strlen(user);
        parmBlock = get31SpK(sp, ADMIN_USER_KEY, pblen);
        memset(parmBlock, 0x00, pblen);
        pprof = (struct admn_prof_map *)parmBlock;
        memcpy(pprof->admn_prof_eye, ADMN_PROF_PXTR, 4);
        memcpy(pprof->admn_prof_clsname, "USER    ", 8);
        pprof->admn_prof_namelen = strlen(user);
        memcpy(pprof->admn_prof_profname, user, strlen(user));
        pprof->admn_prof_nameonly = 1;
    }
    else {
        pblen = *pParmBlockLength;
    }

    result = false;
    IRRSEQ00(workArea, &alet1, &safRc, &alet2, &racfRc, &alet3, &racfReason,
             &fc, parmBlock, NULL, NULL, &sp, &outbuf);

    if (safRc == 0) {
        pprof = (struct admn_prof_map *)outbuf;
        memcpy(user, pprof->admn_prof_profname, pprof->admn_prof_namelen);
        user[pprof->admn_prof_namelen] = '\0';
        free31SpK(sp, ADMIN_USER_KEY, pblen, parmBlock);
        *pParmBlock = outbuf;
        *pParmBlockLength = pprof->admn_prof_outlen;
        result = true;
    }
    else {
        free31SpK(sp, ADMIN_USER_KEY, pblen, parmBlock);
        if (safRc != 4 || racfRc != 4 || racfReason != 4) {     /* something other than "no more users (= 4/4/4)" */
            printf("IRRSEQ00 safrc=%d, racfrc=%d, racfreason=%d\n",
                   safRc, racfRc, racfReason);
        }
    }

    return result;
}

void printCert(struct cddl_get_cert *pparm)
{
    uint32_t            status = *(uint32_t *)pparm->cddl_status;
    gsk_status          rc, rc1, rc2;
    gsk_buffer          buffer, name;
    x509_certificate    cert;
    x509_name           xname;
    struct tm           *pStart, *pEnd;
    char                serialNumber[44], *subject, *issuer;

    buffer.length = pparm->cddl_cert_len;
    buffer.data   = pparm->cddl_cert_ptr;
    rc = gsk_decode_certificate(&buffer, &cert);
    if (rc == 0) {
        pStart = &cert.tbsCertificate.validity.notBefore;
        pEnd   = &cert.tbsCertificate.validity.notAfter;
        for (int i = 0; i < cert.tbsCertificate.serialNumber.length; i++) {
            sprintf(serialNumber + 2 * i, "%02X", ((uint8_t *)cert.tbsCertificate.serialNumber.data)[i]);
        }
        serialNumber[2 * cert.tbsCertificate.serialNumber.length] = '\0';
        name.length = pparm->cddl_sdn_len;
        name.data = pparm->cddl_sdn_ptr;
        subject = "";
        issuer = "";
        rc = gsk_decode_name(&name, &xname);
        if (rc == 0) {
            rc1 = gsk_name_to_dn(&xname, &subject);
            if (rc1 != 0) {
                fprintf(stderr, "gsk_name_to_dn for subject's DN, status=%08X %s\n", rc1, gsk_strerror(rc1));
            }
            gsk_free_name(&xname);
            rc2 = gsk_name_to_dn(&cert.tbsCertificate.issuer, &issuer);
            if (rc1 != 0) {
                fprintf(stderr, "gsk_name_to_dn for issuer's DN, status=%08X %s\n", rc2, gsk_strerror(rc2));
                subject = "";
            }
        }
        else {
            fprintf(stderr, "gsk_decode_name, status=%08X %s\n", rc, gsk_strerror(rc));
            subject = "";
        }

        printf("%.*s,\"%.*s\",%s,%04u-%02u-%02u %02u:%02u:%02u,%04u-%02u-%02u %02u:%02u:%02u,\"%s\",\"%s\",\"%s\"\n",
               (int)pparm->cddl_racf_userid[0], pparm->cddl_racf_userid + 1,
               pparm->cddl_label_len, pparm->cddl_label_ptr,
               status == 0x80000000 ? "TRUST" : (status == 0x40000000 ? "HIGHTRUST" : status == 0x20000000 ? "NOTRUST" : "?unknown?"),
               pStart->tm_year+1900, pStart->tm_mon+1, pStart->tm_mday, pStart->tm_hour, pStart->tm_min, pStart->tm_sec,
               pEnd->tm_year+1900, pEnd->tm_mon+1, pEnd->tm_mday, pEnd->tm_hour, pEnd->tm_min, pEnd->tm_sec,
               serialNumber, subject, issuer);
        gsk_free_certificate(&cert);
        if (rc == 0 && rc1 == 0) gsk_free_string(subject);
        if (rc == 0 && rc2 == 0) gsk_free_string(issuer);
    }
    else {
        fprintf(stderr, "gsk_decode_certificate status=%08X \n", rc, gsk_strerror(rc));
    }
}
