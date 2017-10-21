#include "msg.h"

void* print_name_1_svc(char** txt, struct svc_req *req)
{
  printf("Hello world: %s\n", *txt);
}
