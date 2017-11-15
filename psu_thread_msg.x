struct rpc__sigset_t
{
  unsigned hyper __val[16];
};

struct rpc_libc_fpxreg
{
  unsigned int significand[4];
  unsigned int exponent;
  unsigned int padding[3];
};

struct rpc_libc_xmmreg
{
  unsigned int element[4];
};

struct rpc_libc_fpstate
{
  /* 64-bit FXSAVE format.  */
  unsigned int		cwd;
  unsigned int		swd;
  unsigned int		ftw;
  unsigned int		fop;
  unsigned hyper		rip;
  unsigned hyper		rdp;
  unsigned int		mxcsr;
  unsigned int		mxcr_mask;
  struct rpc_libc_fpxreg	_st[8];
  struct rpc_libc_xmmreg	_xmm[16];
  unsigned int		padding[24];
};

typedef hyper rpc_greg_t;

typedef rpc_greg_t rpc_gregset_t[23];

struct rpc_mcontext
{
  rpc_gregset_t gregs;
  /* Note that fpregs is a pointer.  */
  struct rpc_libc_fpstate fpregs;
  unsigned hyper __reserved1 [8];
};

struct rpc_stack_t
{
  int ss_flag;
  opaque ss_sp<>;
};


struct rpc_ucontext
{
  unsigned long uc_flags;
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

