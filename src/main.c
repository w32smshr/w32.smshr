#include <stdio.h>
#include <stdlib.h>

#include "irc.h"

int main(int argc, char * argv[]) {
  if (prepareBot("188.126.73.62", 6667) != 0) {
    getchar();
    exit(1);
  }

  printf("connected\n");
  botHello("w32smshr", "w32smshr", "w32smshr");
  botLoop();

  getchar();
  return 0;
}
