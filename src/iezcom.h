#pragma pack(packed)

#ifndef __com__
#define __com__

struct com {
  void * __ptr32 comecbpt;
                 /* -            PTR TO ECB FOR STOP OR MODIFY COMMAND */
  void * __ptr32 comcibpt;
                 /* -            PTR TO COMMAND INPUT BUFFER (CIB)     */
  unsigned char  comtoken[4];
                 /* -          31 BIT RIGHT JUSTIFIED TOKEN            */
  };

/* Values for field "comtoken" */
#define comtokhr 0x80 /* -        BIT = 1 INDICATES A TOKEN PRESENT */

#endif

#pragma pack(reset)
