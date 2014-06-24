#define ERR_OK 0
#define ERR_FAIL 1
#define ERR_OMEM 2

#define MAGIC_COOKIE 0x0DD28491
#define __NO_MINGW_LFS 1
#define _POSIX_C_SOURCE 1
#define _POSIX_SOURCE 1

#define _TRAILER_STRUCT_T \
    typedef struct __attribute__ ((__packed__)) _trailer \
    { \
        unsigned int magicNr; \
        unsigned int *myOffset; \
        unsigned int mySize; \
        unsigned int *rlEntryPt; \
    } trailer_t;
