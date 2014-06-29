#ifndef PE_H_INCLUDED
#define PE_H_INCLUDED

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct PEHdr
{
    char *mapBuf;
    DWORD SectionOffset; /* .text section offset*/
    IMAGE_DOS_HEADER *pDosHeader;
    IMAGE_NT_HEADERS *pNtHeader;
    IMAGE_SECTION_HEADER *pSecHeader;
} PEHdr_t;

int readPE(const char *path, struct PEHdr *hdr, bool writeAccess);

#endif // PE_H_INCLUDED
