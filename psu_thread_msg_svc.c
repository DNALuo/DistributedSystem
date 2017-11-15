#include "./psu_thread_msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include "utility.h"

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

static void parse_thread_data(struct svc_req *rqstp, register SVCXPRT *transp, struct thread_data *ptr)
{
  union argument_union {
    rpc_ucontext migrate_1_arg;
  } *argument = (union argument_union *)malloc(sizeof(union argument_union));
  union result_union{
  } *result = (union result_union *)malloc(sizeof(union result_union));
  bool_t retval;
  xdrproc_t _xdr_argument, _xdr_result;
  bool_t (*local)(char *, void *, struct svc_req *);

  switch (rqstp->rq_proc) {
    case NULLPROC:
      (void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
      return;

    case MIGRATE:
      _xdr_argument = (xdrproc_t) xdr_rpc_ucontext;
      _xdr_result = (xdrproc_t) xdr_void;
      local = (bool_t (*) (char *, void *,  struct svc_req *))migrate_1_svc;
      break;

    default:
      svcerr_noproc (transp);
      return;
  }
  memset ((char *)argument, 0, sizeof (*argument));
  if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) argument)) {
    fprintf(stderr, "Error parsing arguments.\n");
    svcerr_decode (transp);
    exit(1);
  }

  PACK_THREAD_DATA(ptr);
}

static void run(struct svc_req *rqstp, register SVCXPRT *transp)
{
  dispatcher(rqstp, transp, parse_thread_data, psu_thread_1_freeresult);
}

int main (int argc, char **argv)
{
  register SVCXPRT *transp;

  pmap_unset (PSU_THREAD, PSU_THREAD_V1);

  transp = svcudp_create(RPC_ANYSOCK);
  if (transp == NULL) {
    fprintf (stderr, "%s", "cannot create udp service.");
    exit(1);
  }
  if (!svc_register(transp, PSU_THREAD, PSU_THREAD_V1, run, IPPROTO_UDP)) {
    fprintf (stderr, "%s", "unable to register (PSU_THREAD, PSU_THREAD_V1, udp).");
    exit(1);
  }

  transp = svctcp_create(RPC_ANYSOCK, 0, 0);
  if (transp == NULL) {
    fprintf (stderr, "%s", "cannot create tcp service.");
    exit(1);
  }
  if (!svc_register(transp, PSU_THREAD, PSU_THREAD_V1, run, IPPROTO_TCP)) {
    fprintf (stderr, "%s", "unable to register (PSU_THREAD, PSU_THREAD_V1, tcp).");
    exit(1);
  }
  printf("Server up and running.\n");
  svc_run ();
  fprintf (stderr, "%s", "svc_run returned");
  exit (1);
  /* NOTREACHED */
}