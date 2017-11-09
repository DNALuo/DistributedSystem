#pragma once
#include <rpc/rpc.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif


struct RequestPack {
	int lock_number;
	int seqno;
	int nodeid;
};
typedef struct RequestPack RequestPack;

#define PSU_DIST_LOCK_MGR 0x2ffffff
#define PSU_DIST_LOCK_MGR_V1 1

#if defined(__STDC__) || defined(__cplusplus)
#define INIT_LOCK_MGR 1
extern  enum clnt_stat init_lock_mgr_1(char **, void *, CLIENT *);
extern  bool_t init_lock_mgr_1_svc(char **, void *, struct svc_req *);
#define ACQUIRE_LOCK 2
extern  enum clnt_stat acquire_lock_1(int *, void *, CLIENT *);
extern  bool_t acquire_lock_1_svc(int *, void *, struct svc_req *);
#define RELEASE_LOCK 3
extern  enum clnt_stat release_lock_1(int *, void *, CLIENT *);
extern  bool_t release_lock_1_svc(int *, void *, struct svc_req *);
#define REQUEST 4
extern  enum clnt_stat request_1(RequestPack *, void *, CLIENT *);
extern  bool_t request_1_svc(RequestPack *, void *, struct svc_req *);
extern int psu_dist_lock_mgr_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define INIT_LOCK_MGR 1
extern  enum clnt_stat init_lock_mgr_1();
extern  bool_t init_lock_mgr_1_svc();
#define ACQUIRE_LOCK 2
extern  enum clnt_stat acquire_lock_1();
extern  bool_t acquire_lock_1_svc();
#define RELEASE_LOCK 3
extern  enum clnt_stat release_lock_1();
extern  bool_t release_lock_1_svc();
#define REQUEST 4
extern  enum clnt_stat request_1();
extern  bool_t request_1_svc();
extern int psu_dist_lock_mgr_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_RequestPack (XDR *, RequestPack*);

#else /* K&R C */
extern bool_t xdr_RequestPack ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

