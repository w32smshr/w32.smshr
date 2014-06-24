#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#ifdef _SMSHR_DEBUG
void printHex(const void *lpvbits, const unsigned int n) {
    char* data = (char*) lpvbits;
    unsigned int i = 0;
    char line[17] = {};
    printf("%.8X | ", (unsigned char*)data);
    while ( i < n ) {
        line[i%16] = *(data+i);
        if ((line[i%16] < 32) || (line[i%16] > 126)) {
            line[i%16] = '.';
        }
        printf("%.2X", (unsigned char)*(data+i));
        i++;
        if (i%4 == 0) {
            if (i%16 == 0) {
                if (i < n-1)
                    printf(" | %s\n%.8X | ", &line, data+i);
            } else {
                printf(" ");
            }
        }
    }
    while (i%16 > 0) {
        (i%4 == 0)?printf("   "):printf("  ");
        line[i%16] = ' ';
        i++;
    }
    printf(" | %s\n", &line);
}
#endif //_SMSHR_DEBUG
