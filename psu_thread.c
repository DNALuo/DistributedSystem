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
  cont->


  void *result = NULL;
  migrate_1(cont, &result, client);
  clnt_destroy(client);
}