#pragma once

#include <rpc/rpc.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif


struct rpc__sigset_t {
  u_quad_t __val[16];
};
typedef struct rpc__sigset_t rpc__sigset_t;

struct rpc_libc_fpxreg {
  u_int significand[4];
  u_int exponent;
  u_int padding[3];
};
typedef struct rpc_libc_fpxreg rpc_libc_fpxreg;

struct rpc_libc_xmmreg {
  u_int element[4];
};
typedef struct rpc_libc_xmmreg rpc_libc_xmmreg;

struct rpc_libc_fpstate {
  u_int cwd;
  u_int swd;
  u_int ftw;
  u_int fop;
  u_quad_t rip;
  u_quad_t rdp;
  u_int mxcsr;
  u_int mxcr_mask;
  struct rpc_libc_fpxreg _st[8];
  struct rpc_libc_xmmreg _xmm[16];
  u_int padding[24];
};
typedef struct rpc_libc_fpstate rpc_libc_fpstate;

typedef quad_t rpc_greg_t;

typedef rpc_greg_t rpc_gregset_t[23];

struct rpc_mcontext {
  rpc_gregset_t gregs;
  struct rpc_libc_fpstate fpregs;
  u_quad_t __reserved1[8];
};
typedef struct rpc_mcontext rpc_mcontext;

struct rpc_stack_t {
  int ss_flag;
  struct {
    u_int ss_sp_len;
    char *ss_sp_val;
  } ss_sp;
};
typedef struct rpc_stack_t rpc_stack_t;

struct rpc_ucontext {
  u_long uc_flags;
  rpc_stack_t uc_stack;
  rpc_mcontext uc_mcontext;
  rpc__sigset_t uc_sigmask;
  struct rpc_libc_fpstate __fpregs_mem;
};
typedef struct rpc_ucontext rpc_ucontext;

#define PSU_THREAD 0x3ffffff
#define PSU_THREAD_V1 1

#if defined(__STDC__) || defined(__cplusplus)
#define MIGRATE 1
extern  enum clnt_stat migrate_1(rpc_ucontext *, void *, CLIENT *);
extern  bool_t migrate_1_svc(rpc_ucontext *, void *, struct svc_req *);
extern int psu_thread_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define MIGRATE 1
extern  enum clnt_stat migrate_1();
extern  bool_t migrate_1_svc();
extern int psu_thread_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_rpc__sigset_t (XDR *, rpc__sigset_t*);
extern  bool_t xdr_rpc_libc_fpxreg (XDR *, rpc_libc_fpxreg*);
extern  bool_t xdr_rpc_libc_xmmreg (XDR *, rpc_libc_xmmreg*);
extern  bool_t xdr_rpc_libc_fpstate (XDR *, rpc_libc_fpstate*);
extern  bool_t xdr_rpc_greg_t (XDR *, rpc_greg_t*);
extern  bool_t xdr_rpc_gregset_t (XDR *, rpc_gregset_t);
extern  bool_t xdr_rpc_mcontext (XDR *, rpc_mcontext*);
extern  bool_t xdr_rpc_stack_t (XDR *, rpc_stack_t*);
extern  bool_t xdr_rpc_ucontext (XDR *, rpc_ucontext*);

#else /* K&R C */
extern bool_t xdr_rpc__sigset_t ();
extern bool_t xdr_rpc_libc_fpxreg ();
extern bool_t xdr_rpc_libc_xmmreg ();
extern bool_t xdr_rpc_libc_fpstate ();
extern bool_t xdr_rpc_greg_t ();
extern bool_t xdr_rpc_gregset_t ();
extern bool_t xdr_rpc_mcontext ();
extern bool_t xdr_rpc_stack_t ();
extern bool_t xdr_rpc_ucontext ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif
