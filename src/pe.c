#include "config.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "log.h"
#include "pe.h"

static int getTextSectionOffset(PIMAGE_SECTION_HEADER pSectionHeader , int NumberOfSections)
{
    int n = NumberOfSections;
    while(n > 0)
    {
        if( !strcmp((char*)pSectionHeader->Name , ".text") || !strcmp((char*)pSectionHeader->Name , ".code") || !strcmp((char*)pSectionHeader->Name , "CODE") || !strcmp((char*)pSectionHeader->Name , "TEXT"))
        {
            return pSectionHeader->PointerToRawData;
        }
        n--;
    }
    /* we did not find .text section */
    return 0;
}

int readPE(const char *path, struct PEHdr *hdr, bool writeAccess)
{
    HANDLE hFile;
    HANDLE hMap;
    char *MappedFile = 0;
    DWORD FileSize; /* file size */

    D("[PE]: Mapping file: %s", path);
    hFile = CreateFile(path ,
                       (writeAccess ? (GENERIC_WRITE | GENERIC_READ) : GENERIC_READ) ,
                       0 ,
                       0 ,
                       OPEN_EXISTING ,
                       FILE_ATTRIBUTE_NORMAL ,
                       0);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        E("[PE]: Can't open File! Error code : %lu" , GetLastError());
        return (-1);
    }
    /* get file size */
    FileSize = GetFileSize(hFile , 0 );
    D("[PE]: File Size: %lu", FileSize);
    /* mapping file */
    hMap = CreateFileMapping(hFile ,
                             0 ,
                             (writeAccess ? PAGE_READWRITE : PAGE_READONLY),
                             0 ,
                             FileSize ,
                             0);
    if(hMap == INVALID_HANDLE_VALUE)
    {
        E("[PE]: Can't map file! Error code: %lu" , GetLastError());
        CloseHandle(hFile);
        return (-1);
    }
    D2("[PE]: file mapped");
    MappedFile = (char*)MapViewOfFile(hMap , (writeAccess ? (FILE_MAP_READ | FILE_MAP_WRITE) : FILE_MAP_READ) , 0 , 0 , FileSize);
    if(MappedFile == NULL)
    {
        E("[PE]: Can't map file! Error code %lu", GetLastError());
        CloseHandle(hFile);
        CloseHandle(hMap);
        UnmapViewOfFile(MappedFile);
        return (-1);
    }
    hdr->mapBuf = MappedFile;
    hdr->pDosHeader = (IMAGE_DOS_HEADER*)MappedFile;
    hdr->pNtHeader = (IMAGE_NT_HEADERS*)((DWORD)MappedFile + hdr->pDosHeader->e_lfanew);
    hdr->pSecHeader = IMAGE_FIRST_SECTION(hdr->pNtHeader);
    /* get .text section PointerToRawData*/
    hdr->SectionOffset = getTextSectionOffset(hdr->pSecHeader , hdr->pNtHeader->FileHeader.NumberOfSections);
    if (hdr->SectionOffset == 0) {
        E2("[PE] No useful section found ..");
    }
    _PE_DBG_INFO(hdr)
    return (0);
}
