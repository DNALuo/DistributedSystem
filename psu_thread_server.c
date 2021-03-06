#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <glib.h>
#include "utility.h"
#include "psu_thread_msg.h"

#include <ucontext.h>
#include <sys/ucontext.h>
#include <unistd.h>

bool_t migrate_1_svc(rpc_ucontext *context, void *res, struct svc_req *req)
{
  bool has_set_context = false;
  printf("Server received.\n");
  ucontext_t cont;

  // initialize the context
  getcontext(&cont);

  cont.uc_flags = context->uc_flags;

  // ucontext.stack_t
  cont.uc_stack.ss_flags = context->uc_stack.ss_flags;
  cont.uc_stack.ss_sp = context->uc_stack.ss_sp.ss_sp_val;
  cont.uc_stack.ss_size = context->uc_stack.ss_sp.ss_sp_len;

  // ucontext.mcontext_t
  memcpy(cont.uc_mcontext.__reserved1, context->uc_mcontext.__reserved1, sizeof(cont.uc_mcontext.__reserved1));
  cont.uc_mcontext.fpregs = (struct _libc_fpstate *)malloc(sizeof(struct _libc_fpstate));
  memcpy(cont.uc_mcontext.fpregs, &context->uc_mcontext.fpregs, sizeof(struct _libc_fpstate));
  memcpy(cont.uc_mcontext.gregs, context->uc_mcontext.gregs, sizeof(gregset_t));

  memcpy(&cont.uc_sigmask, &context->uc_sigmask, sizeof(__sigset_t));

  memcpy(&cont.__fpregs_mem, &context->__fpregs_mem, sizeof(struct _libc_fpstate));

    cont.uc_mcontext.gregs[REG_RSP] = cont.uc_stack.ss_sp;



  ucontext_t *my_context = (ucontext_t *)malloc(sizeof(ucontext_t));
  getcontext(my_context);
  if(!has_set_context)
  {
    cont.uc_link = my_context;
    has_set_context = true;
    pid_t pid = 0;
    pid = fork();
    if(pid == 0)
    {
      printf("Setting the context.\n");
      setcontext(&cont);
    }
  }

  return true;
}


int psu_thread_1_freeresult(SVCXPRT *transp,xdrproc_t xdr_result, caddr_t result)
{
  return 1;
}