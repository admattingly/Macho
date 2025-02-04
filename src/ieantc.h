/* C header for IEANTxx name/token pair functions */

/*
 * Name/Token Level Constants
 */
#define IEANT_TASK_LEVEL        1
#define IEANT_HOME_LEVEL        2
#define IEANT_PRIMARY_LEVEL     3
#define IEANT_SYSTEM_LEVEL      4
#define IEANT_TASKAUTH_LEVEL    11
#define IEANT_HOMEAUTH_LEVEL    12
#define IEANT_PRIMARYAUTH_LEVEL 13
/*
 * Name/Token Persistence Constants
 */
#define IEANT_NOPERSIST         0
#define IEANT_PERSIST           1
#define IEANT_NOCHECKPOINT      0
#define IEANT_CHECKPOINTOK      2
/*
 * Name/Token Return Code Constants
 */
#define IEANT_OK                0
#define IEANT_DUP_NAME          4
#define IEANT_NOT_FOUND         4
#define IEANT_24BITMODE         8
#define IEANT_NOT_AUTH          16
#define IEANT_SRB_MODE          20
#define IEANT_LOCK_HELD         24
#define IEANT_LEVEL_INVALID     28
#define IEANT_NAME_INVALID      32
#define IEANT_PERSIST_INVALID   36
#define IEANT_AR_INVALID        40
#define IEANT_UNEXPECTED_ERR    64

/* name/token pair functions */
void IEANTCR(int *plevel, void *name, void *token, int *persist, int *return_code);
void IEANTDL(int *plevel, void *name, int *return_code);
void IEANTRT(int *plevel, void *name, void *token, int *return_code);
#pragma linkage(IEAN4CR, OS)
#pragma linkage(IEAN4DL, OS)
#pragma linkage(IEAN4RT, OS)
void IEAN4CR(int *plevel, void *name, void *token, int *persist, int *return_code);
void IEAN4DL(int *plevel, void *name, int *return_code);
void IEAN4RT(int *plevel, void *name, void *token, int *return_code);
