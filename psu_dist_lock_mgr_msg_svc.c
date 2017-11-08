#include "psu_dist_lock_mgr_msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

static void *psu_dist_lock_mgr_1_run(void *data)
{
  // unpack the thread data
  struct thread_data
  {
    struct svc_req *rqstp;
    SVCXPRT *transp;
  } *ptr_data;

  ptr_data = (struct thread_data  *)data;
  struct svc_req *rqstp = ptr_data-> rqstp;
  register SVCXPRT *transp = ptr_data-> transp;

	union {
		char *init_lock_mgr_1_arg;
		int acquire_lock_1_arg;
		int release_lock_1_arg;
		RequestPack request_1_arg;
	} argument;
	union {
	} result;
	bool_t retval;
	xdrproc_t _xdr_argument, _xdr_result;
	bool_t (*local)(char *, void *, struct svc_req *);

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
		svcerr_noproc (transp);
		return NULL;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return NULL;
	}
	retval = (bool_t) (*local)((char *)&argument, (void *)&result, rqstp);
	if (retval > 0 && !svc_sendreply(transp, (xdrproc_t) _xdr_result, (char *)&result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	if (!psu_dist_lock_mgr_1_freeresult (transp, _xdr_result, (caddr_t) &result))
		fprintf (stderr, "%s", "unable to free results");

	return NULL;
}

static void psu_dist_lock_mgr_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
  // if it's nullproc don't create a thread
  if(rqstp->rq_proc == NULLPROC) {
    (void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
    return;
  }

  char *procedures[] = {"NULLPROC", "INIT_LOCK_MGR", "ACQUIRE_LOCK", "RELEASE_LOCK", "REQUEST"};

  printf("Procedure calls %s\n", procedures[rqstp->rq_proc]);

  struct thread_data
  {
    struct svc_req *rqstp;
    SVCXPRT *transp;
  } *data_ptr=(struct thread_data*)malloc(sizeof(struct thread_data));
  data_ptr-> rqstp = rqstp;
  data_ptr-> transp = transp;
  pthread_t thread;
  pthread_attr_t attr;
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&thread, &attr, &psu_dist_lock_mgr_1_run, (void *)data_ptr);

  printf("Thread created to run.\n");
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
	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
