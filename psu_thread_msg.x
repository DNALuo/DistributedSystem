struct rpc__sigset_t
{
  unsigned long int __val[16];
};

struct rpc_libc_fpxreg
{
  unsigned short int significand[4];
  unsigned short int exponent;
  unsigned short int padding[3];
};

struct rpc_libc_xmmreg
{
  __uint32_t	element[4];
};

struct rpc_libc_fpstate
{
  /* 64-bit FXSAVE format.  */
  __uint16_t		cwd;
  __uint16_t		swd;
  __uint16_t		ftw;
  __uint16_t		fop;
  __uint64_t		rip;
  __uint64_t		rdp;
  __uint32_t		mxcsr;
  __uint32_t		mxcr_mask;
  struct rpc_libc_fpxreg	_st[8];
  struct rpc_libc_xmmreg	_xmm[16];
  __uint32_t		padding[24];
};

typedef long int rpc_greg_t;

typedef rpc_greg_t rpc_gregset_t[19];

struct rpc_mcontext
{
  rpc_gregset_t gregs;
  /* Due to Linux's history we have to use a pointer here.  The SysV/i386
     ABI requires a struct with the values.  */
  struct rpc_libc_fpstate fpregs;
  unsigned long int oldmask;
  unsigned long int cr2;
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

