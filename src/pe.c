#include "config.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>

static int getTextSectionOffset(PIMAGE_SECTION_HEADER pSectionHeader , int NumberOfSections)
{
    while(NumberOfSections > 0)
    {
        if( !strcmp((char*)pSectionHeader->Name , ".text"))
        {
            return pSectionHeader->PointerToRawData;
        }
    }
    /* we did not find .text section */
    return 0;
}