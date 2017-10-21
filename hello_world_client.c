#include <stdio.h>
#include <stdlib.h>
#include <rpc/rpc.h>
#include "msg.h"

int main(int argc, char* argv[]) {
  if(argc < 3) {
    printf("Usage: ./client <ip-addr> <message>\n");
    exit(1);
  }

  CLIENT *c1;

  c1 = clnt_create(argv[1], HELLOWORLD, HELLO_V1, "tcp");
  if(c1 == NULL) {
    printf("pcreate error\n");
    exit(1);
  }

  printf("Client: Calling host: %s with message: %s\n", argv[1], argv[2]);
  print_name_1(&argv[2], c1);
  printf("Client: Call ended\n");

  return 0;
}
