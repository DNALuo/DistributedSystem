#include "psu_dist_lock_mgr_msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <memory.h>
#include "utility.h"
#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

extern void initialize_global_variable();

static void parse_thread_data(struct svc_req *rqstp, register SVCXPRT *transp, struct thread_data *thread_data)
{
  // allocate argument and result on the heap
  union argument_union{
    char *init_lock_mgr_1_arg;
    int acquire_lock_1_arg;
    int release_lock_1_arg;
    RequestPack request_1_arg;
  } *argument = (union argument_union *)malloc(sizeof(union argument_union));

  union result_union {
  } *result = (union result_union *)malloc(sizeof(union result_union));

  xdrproc_t _xdr_argument = NULL, _xdr_result = NULL;
  bool_t (*local)(char *, void *, struct svc_req *) = NULL;
  switch (rqstp->rq_proc) {
    case INIT_LOCK_MGR:
      _xdr_argument = (xdrproc_t) xdr_wrapstring;
      _xdr_result = (xdrproc_t) xdr_void;
      local = (bool_t (*) (char *, void *,  struct svc_req *))init_lock_mgr_1_svc;
      break;

    case ACQUIRE_LOCK:
      _xdr_argument = (xdrproc_t) xdr_int;
      _xdr_result = (xdrproc_t) xdr_void;
      local = (bool_t (*) (char *, void *,  struct svc_req *))acquire_lock_1_svc;
      break;

    case RELEASE_LOCK:
      _xdr_argument = (xdrproc_t) xdr_int;
      _xdr_result = (xdrproc_t) xdr_void;
      local = (bool_t (*) (char *, void *,  struct svc_req *))release_lock_1_svc;
      break;

    case REQUEST:
      _xdr_argument = (xdrproc_t) xdr_RequestPack;
      _xdr_result = (xdrproc_t) xdr_void;
      local = (bool_t (*) (char *, void *,  struct svc_req *))request_1_svc;
      break;

    default:
      printf("No procedure Found!\n");
      svcerr_noproc (transp);
  }

  // memset and call the argument by pointer
  memset ((char *)argument, 0, sizeof (*argument));
  if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) argument)) {
    printf("Error getting arguments.\n");
    svcerr_decode (transp);
  }

  PACK_THREAD_DATA(thread_data);
}

static void run(struct svc_req *rqstp, register SVCXPRT *transp)
{
  dispatcher(rqstp, transp, parse_thread_data, psu_dist_lock_mgr_1_freeresult);
}

int main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, run, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, run, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, tcp).");
		exit(1);
	}
  printf("Server up and running...\n");
  // initialize the global variables
  initialize_global_variable();
	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
