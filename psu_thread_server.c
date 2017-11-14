#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <glib.h>
#include <unistd.h>
#include "utility.h"
#include "psu_thread_msg.h"

bool_t migrate_1_svc(rpc_ucontext *context, void *res, struct svc_req *req)
{
  printf("Server received.\n");
  return true;
}


int psu_thread_1_freeresult(SVCXPRT *transp,xdrproc_t xdr_result, caddr_t result)
{
  return 1;
}