#include "psu_dist_lock_mgr_msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <memory.h>
#include <rpc/svc.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

const char *procedures[] = {"NULLPROC", "INIT_LOCK_MGR", "ACQUIRE_LOCK", "RELEASE_LOCK", "REQUEST"};
extern void initialize_global_variable();

struct thread_data
{
  struct svc_req *rqstp;
  SVCXPRT *transp;
  caddr_t argument;
  xdrproc_t xdr_argument, xdr_result;
  bool_t (*local)(char *, void *, struct svc_req *);
};

static void parse_thread_data(struct svc_req *rqstp, register SVCXPRT *transp, struct thread_data *thread_data)
{
  union argument_union{
    char *init_lock_mgr_1_arg;
    int acquire_lock_1_arg;
    int release_lock_1_arg;
    RequestPack request_1_arg;
  } *argument = (union argument_union *)malloc(sizeof(union argument_union));

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
  memset ((char *)argument, 0, sizeof (*argument));
  if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) argument)) {
    printf("Error getting arguments.\n");
    svcerr_decode (transp);
  }

  thread_data->local = local;
  thread_data->rqstp = rqstp;
  thread_data->transp = transp;
  thread_data->argument = (caddr_t) argument;
  thread_data->xdr_result = _xdr_result;
  thread_data->xdr_argument = _xdr_argument;
}

static void *psu_dist_lock_mgr_1_run(void *data)
{
  // unpack the thread data
  struct thread_data *ptr_data = (struct thread_data *)data;
  struct svc_req *rqstp = ptr_data->rqstp;
  SVCXPRT *transp = ptr_data->transp;
  caddr_t argument = ptr_data->argument;
  xdrproc_t _xdr_argument = ptr_data->xdr_argument, _xdr_result = ptr_data->xdr_result;
  bool_t (*local)(char *, void *, struct svc_req *) = ptr_data->local;

  bool_t retval;
  union {
  } result;

	retval = (bool_t) (*local)((char *)argument, (void *)&result, rqstp);
	if (retval > 0 && !svc_sendreply(transp, (xdrproc_t) _xdr_result, (char *)&result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	if (!psu_dist_lock_mgr_1_freeresult (transp, _xdr_result, (caddr_t) &result))
		fprintf (stderr, "%s", "unable to free results");

  printf("\033[33;1mProcedure %s finishes.\033[0m\n", procedures[rqstp->rq_proc]);
	return NULL;
}

static void psu_dist_lock_mgr_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
  // if it's nullproc don't create a thread
  if(rqstp->rq_proc == NULLPROC) {
    (void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
    return;
  }

  struct thread_data *data_ptr=(struct thread_data *)malloc(sizeof(struct thread_data));

  parse_thread_data(rqstp, transp, data_ptr);

  printf("\033[32;1mProcedure calls %s\033[0m\n", procedures[rqstp->rq_proc]);

  pthread_t *thread= (pthread_t *)malloc(sizeof(pthread_t));
  pthread_create(thread, NULL, psu_dist_lock_mgr_1_run, (void *)data_ptr);
  pthread_detach(*thread);

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
	if (!svc_register(transp, PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, psu_dist_lock_mgr_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, psu_dist_lock_mgr_1, IPPROTO_TCP)) {
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
