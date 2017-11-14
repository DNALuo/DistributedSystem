#include "psu_thread.h"
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <memory.h>
#include <glib.h>
#include "utility.h"
#include "psu_thread_msg.h"

#define _XOPEN_SOURCE
#include <ucontext.h>



int psu_thread_create(psu_thread_info_t* t_info, void* (*start_routine)(void*), void* args)
{
  pthread_t thread;
  pthread_create(&thread, NULL, start_routine, args);
  pthread_join(thread, NULL);
}

int psu_thread_migrate(char* node)
{
  ucontext_t context;
  getcontext(&context);
  CLIENT *client = create_client(node, PSU_THREAD, PSU_THREAD_V1, "tcp");
  rpc_ucontext *cont = (rpc_ucontext *)malloc(sizeof(rpc_ucontext));
  cont->uc_flags = context.uc_flags;

  // ucontext.stack_t
  cont->uc_stack.ss_flag = context.uc_stack.ss_flags;
  cont->uc_stack.ss_sp.ss_sp_val = (char *)malloc(sizeof(char) * context.uc_stack.ss_size);
  memcpy(cont->uc_stack.ss_sp.ss_sp_val, context.uc_stack.ss_sp, sizeof(char) * context.uc_stack.ss_size);
  cont->uc_stack.ss_sp.ss_sp_len = (u_int)context.uc_stack.ss_size;

  // ucontext.mcontext_t
  memcpy(cont->uc_mcontext.__reserved1, context.uc_mcontext.__reserved1, sizeof(context.uc_mcontext.__reserved1));
  memcpy(&(cont->uc_mcontext.fpregs), context.uc_mcontext.fpregs, sizeof(rpc_libc_fpstate));
  memcpy(cont->uc_mcontext.gregs, context.uc_mcontext.gregs, sizeof(rpc_gregset_t));

  memcpy(&cont->uc_sigmask, &context.uc_sigmask, sizeof(rpc__sigset_t));

  memcpy(&cont->__fpregs_mem, &context.__fpregs_mem, sizeof(rpc_libc_fpstate));

  void *result = NULL;
  migrate_1(cont, &result, client);
  clnt_destroy(client);
}