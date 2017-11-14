typedef struct
{
  unsigned long int __val[16];
} __sigset_t;

struct _libc_fpxreg
{
  unsigned short int significand[4];
  unsigned short int exponent;
  unsigned short int padding[3];
};

struct _libc_xmmreg
{
  __uint32_t	element[4];
};

struct _libc_fpstate
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
  struct _libc_fpxreg	_st[8];
  struct _libc_xmmreg	_xmm[16];
  __uint32_t		padding[24];
};

typedef long int greg_t;

typedef greg_t gregset_t[19];

struct MContext
{
  gregset_t gregs;
  /* Due to Linux's history we have to use a pointer here.  The SysV/i386
     ABI requires a struct with the values.  */
  struct _libc_fpstate fpregs;
  unsigned long int oldmask;
  unsigned long int cr2;
}

struct Stack
{
  int ss_flag;
  opaque ss_sp<>;
}


struct Context
{
  unsigned long uc_flags;
  Stack uc_stack;
  MContext uc_mcontext;
  __sigset_t uc_sigmask;
  int us_sigmask;
  struct _libc_fpstate __fpregs_mem;
};

program PSU_THREAD {
  version PSU_THREAD_V1 {
    void MIGRATE(Context) = 1;
  } = 1;
} = 0x3ffffff;

