/* macho.h */

/* parameter types */
typedef long                *MACHO_INTEGER;
typedef unsigned long long  *MACHO_DWORD;
typedef unsigned char       *MACHO_STRING;
typedef unsigned char       **MACHO_POINTER;

/* function prototypes */
long ALLOC(
        MACHO_POINTER   pbuffer,       /* (output)       buffer pointer */
        MACHO_INTEGER   pbufferLength, /* (input)        buffer length required */
        MACHO_INTEGER   preturnCode,   /* (output)       return code from STORAGE OBTAIN macro */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long ALLOC4K(
        MACHO_POINTER   pbuffer,       /* (output)       buffer pointer */
        MACHO_INTEGER   pbufferLength, /* (input)        buffer length required */
        MACHO_INTEGER   preturnCode,   /* (output)       return code from STORAGE OBTAIN macro */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long CMPSC(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long CMPSC3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_STRING    R1static,      /* (output)       R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long COPYIN(
        MACHO_STRING    string,        /* (input)        string to be copied */
        MACHO_INTEGER   pstringLength, /* (input)        string length */
        MACHO_POINTER   pbuffer,       /* (input)        buffer to receive string */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long COPYOUT(
        MACHO_POINTER   pbuffer,       /* (input)        buffer containing string to be copied */
        MACHO_INTEGER   pbufferLength, /* (input)        string length */
        MACHO_STRING    string,        /* (output)       string */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long DFLTCC(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_STRING    R3,            /* (input/output) R3   */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long DFLTCC3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_STRING    R1static,      /* (output)       R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_STRING    R3,            /* (input/output) R3   */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long EXTRACT(
        MACHO_STRING    answerarea,    /* (output)       12-fullword (48-byte) area to receive output */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long FLOATIN(
        MACHO_STRING    text,          /* (input)        string */
        MACHO_INTEGER   ptextLength,   /* (input)        string length */
        MACHO_STRING    binary,        /* (output)       4-byte binary IEEE 754 single-length floating-point value */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long FLOATOUT(
        MACHO_STRING    binary,        /* (input)        4-byte binary IEEE 754 single-length floating-point value */
        MACHO_STRING    text,          /* (output)       string */
        MACHO_INTEGER   ptextLength,   /* (input/output) (maximum on input, actual on ouput) string length */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long FREE(
        MACHO_POINTER   pbuffer,       /* (input)        buffer to be freed */
        MACHO_INTEGER   pbufferLength, /* (input)        buffer length */
        MACHO_INTEGER   preturnCode,   /* (output)       return code from STORAGE RELEASE macro */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KDSA(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_POINTER   pR1,           /* (ignored)      R1   */
        MACHO_INTEGER   pR1plus1,      /* (ignored)      R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KDSA3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_STRING    R1static,      /* (ignored)      R1   */
        MACHO_INTEGER   pR1plus1,      /* (ignored)      R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KIMD(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_POINTER   pR1,           /* (ignored)      R1   */
        MACHO_INTEGER   pR1plus1,      /* (ignored)      R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KIMD3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_STRING    R1static,      /* (ignored)      R1   */
        MACHO_INTEGER   pR1plus1,      /* (ignored)      R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KLMD(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KLMD3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_STRING    R1static,      /* (output)       R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KM(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KM3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_STRING    R1static,      /* (output)       R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KMA(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_POINTER   pR3,           /* (input/output) R3   */
        MACHO_INTEGER   pR3plus1,      /* (input/output) R3+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KMA3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_STRING    R1static,      /* (input/output) R1   */
        MACHO_STRING    R3static,      /* (input/output) R3   */
        MACHO_INTEGER   pR3plus1,      /* (input/output) R3+1 */
        MACHO_STRING    R2static,      /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KMAC(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_POINTER   pR1,           /* (ignored)      R1   */
        MACHO_INTEGER   pR1plus1,      /* (ignored)      R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KMAC3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_STRING    R1static,      /* (ignored)      R1   */
        MACHO_INTEGER   pR1plus1,      /* (ignored)      R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KMC(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KMC3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_STRING    R1static,      /* (output)       R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KMCTR(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_POINTER   pR3,           /* (input/output) R3   */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KMCTR3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_STRING    R1static,      /* (input/output) R1   */
        MACHO_STRING    R3static,      /* (input/output) R3   */
        MACHO_STRING    R2static,      /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KMF(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KMF3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_STRING    R1static,      /* (output)       R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KMO(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long KMO3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_STRING    R1static,      /* (output)       R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long NNPA(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long NNPA3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long PCC(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long PCC3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long PCKMO(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input/output) GR1 */
        MACHO_INTEGER   pPcRc,         /* (output)       PC routine return code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long PFPO(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_INTEGER   pGR1,          /* (output)       GR1 */
        MACHO_STRING    FPR0,          /* (input/output) 8-byte (64-bit) buffer */
        MACHO_STRING    FPR2,          /* (input/output) 8-byte (64-bit) buffer */
        MACHO_STRING    FPR4,          /* (input/output) 8-byte (64-bit) buffer */
        MACHO_STRING    FPR6,          /* (input/output) 8-byte (64-bit) buffer */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long PRNO(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long PRNO3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_STRING    R1static,      /* (output)       R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long SORTL(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_POINTER   pR1,           /* (input/output) R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_POINTER   pR2,           /* (input/output) R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long SORTL3(
        MACHO_INTEGER   pGR0,          /* (input)        GR0 */
        MACHO_STRING    GR1,           /* (input)        GR1 */
        MACHO_STRING    R1static,      /* (output)       R1   */
        MACHO_INTEGER   pR1plus1,      /* (input/output) R1+1 */
        MACHO_STRING    R2static,      /* (input)        R2   */
        MACHO_INTEGER   pR2plus1,      /* (input/output) R2+1 */
        MACHO_INTEGER   pCC,           /* (output)       condition code */
        MACHO_INTEGER   pinvokes,      /* (output)       number of times instruction was driven before CC=3 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long STCK(
        MACHO_STRING    D2,            /* (output)       8-byte (64-bit) buffer */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long STCKE(
        MACHO_STRING    D2,            /* (output)       16-byte (128-bit) buffer */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long STCKF(
        MACHO_STRING    D2,            /* (output)       8-byte (64-bit) buffer */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long VCFN(
        MACHO_STRING    V1,            /* (output)       V1 */
        MACHO_STRING    V2,            /* (input)        V2 */
        MACHO_INTEGER   pM3,           /* (input)        M3 */
        MACHO_INTEGER   pM4,           /* (input)        M4 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long VCLFNH(
        MACHO_STRING    V1,            /* (output)       V1 */
        MACHO_STRING    V2,            /* (input)        V2 */
        MACHO_INTEGER   pM3,           /* (input)        M3 */
        MACHO_INTEGER   pM4,           /* (input)        M4 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long VCLFNL(
        MACHO_STRING    V1,            /* (output)       V1 */
        MACHO_STRING    V2,            /* (input)        V2 */
        MACHO_INTEGER   pM3,           /* (input)        M3 */
        MACHO_INTEGER   pM4,           /* (input)        M4 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long VCNF(
        MACHO_STRING    V1,            /* (output)       V1 */
        MACHO_STRING    V2,            /* (input)        V2 */
        MACHO_INTEGER   pM3,           /* (input)        M3 */
        MACHO_INTEGER   pM4,           /* (input)        M4 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */
long VCRNF(
        MACHO_STRING    V1,            /* (output)       V1 */
        MACHO_STRING    V2,            /* (input)        V2 */
        MACHO_STRING    V3,            /* (input)        V3 */
        MACHO_INTEGER   pM4,           /* (input)        M4 */
        MACHO_INTEGER   pM5,           /* (input)        M5 */
        MACHO_INTEGER   pticks);       /* (output)       elapsed clock ticks */

