struct rpc__sigset_t
{
  u_int64_t __val[16];
};

struct rpc_libc_fpxreg
{
  u_int16_t significand[4];
  u_int16_t exponent;
  u_int16_t padding[3];
};

struct rpc_libc_xmmreg
{
  unsigned int element[4];
};

struct rpc_libc_fpstate
{
  /* 64-bit FXSAVE format.  */
  u_int16_t		cwd;
  u_int16_t		swd;
  u_int16_t		ftw;
  u_int16_t		fop;
  u_int64_t		rip;
  u_int64_t		rdp;
  u_int32_t		mxcsr;
  u_int32_t		mxcr_mask;
  struct rpc_libc_fpxreg	_st[8];
  struct rpc_libc_xmmreg	_xmm[16];
  u_int32_t		padding[24];
};

typedef u_int64_t rpc_greg_t;

typedef rpc_greg_t rpc_gregset_t[23];

struct rpc_mcontext
{
  rpc_gregset_t gregs;
  /* Note that fpregs is a pointer.  */
  struct rpc_libc_fpstate fpregs;
  u_int64_t __reserved1 [8];
};

struct rpc_stack_t
{
  int ss_flag;
  opaque ss_sp<>;
};


struct rpc_ucontext
{
  u_int64_t uc_flags;
  rpc_stack_t uc_stack;
  rpc_mcontext uc_mcontext;
  rpc__sigset_t uc_sigmask;
  struct rpc_libc_fpstate __fpregs_mem;
};

program PSU_THREAD {
  version PSU_THREAD_V1 {
    void MIGRATE(rpc_ucontext) = 1;
  } = 1;
} = 0x3ffffff;

