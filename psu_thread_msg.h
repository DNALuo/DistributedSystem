#pragma once
#include <rpc/rpc.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif


struct Context {
  u_int uc_mcsize;
  int uc_onstack;
  int us_sigmask;
  struct {
    u_int uc_mcontext_len;
    char *uc_mcontext_val;
  } uc_mcontext;
  int ss_flag;
  u_int ss_size;
  struct {
    u_int ss_sp_len;
    char *ss_sp_val;
  } ss_sp;
};
typedef struct Context Context;

#define PSU_THREAD 0x3ffffff
#define PSU_THREAD_V1 1

#if defined(__STDC__) || defined(__cplusplus)
#define MIGRATE 1
extern  enum clnt_stat migrate_1(Context *, void *, CLIENT *);
extern  bool_t migrate_1_svc(Context *, void *, struct svc_req *);
extern int psu_thread_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define MIGRATE 1
extern  enum clnt_stat migrate_1();
extern  bool_t migrate_1_svc();
extern int psu_thread_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_Context (XDR *, Context*);

#else /* K&R C */
extern bool_t xdr_Context ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

