/* macho.h */

/* parameter types */
typedef long            *MACHO_INTEGER;
typedef unsigned char   *MACHO_STRING;
typedef unsigned char   **MACHO_POINTER;

/* function prototypes */
long PCKMO(
        MACHO_INTEGER   GR0,           /* GR0 */
        MACHO_STRING    GR1,           /* GR1 */
        MACHO_INTEGER   PcRc,          /* PC routine return code */
        MACHO_INTEGER   ticks);        /* elapsed clock ticks */

