/* machopc.h */
/* definitions shared between PC routine, loader and stub */

#define PC_TOKEN_NAME   "MACHO.PCROUTINE "
#define PC_WORK_AREA    65536

/* IDs for verbs provided by PC routine */
#define MACVERB_PCKMO   101

/* error codes */
#define MACERR_SUCCESS  0
#define MACERR_NOTIMPL  8
#define MACERR_NOTAUTH  12
#define MACERR_NOTREG   16

/* common parameter block */
typedef struct {
    unsigned int    verb;
    unsigned int    return_code;
} BLOCK_COMMON, *PBLOCK_COMMON;

/* parameter blocks for each verb */
typedef struct {
    BLOCK_COMMON        common;
    unsigned long long  gr0;
    unsigned long long  gr1;
} BLOCK_PCKMO, *PBLOCK_PCKMO;
