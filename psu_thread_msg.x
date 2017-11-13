struct Context
{
  unsiged int uc_mcsize;
  int uc_onstack;
  int us_sigmask;
  opaque uc_mcontext<>;

  int ss_flag;
  unsiged int ss_size;
  opaque ss_sp<>;
};

program PSU_THREAD {
  version PSU_THREAD_V1 {
    void MIGRATE(Context) = 1;
  } = 1;
} = 0x3ffffff;

