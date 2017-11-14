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
#include <stdbool.h>
#include <assert.h>

struct thread_map
{
  pthread_t *thread;
  char *stack;
  size_t stack_size;
};

static GArray *thread_list = NULL;
static pthread_mutex_t thread_list_lock = PTHREAD_MUTEX_INITIALIZER;

int psu_thread_create(psu_thread_info_t* t_info, void* (*start_routine)(void*), void* args)
{
  size_t stack_size = 8 * 1024 * 1024;
  char *stack = (char *)malloc(sizeof(char) * stack_size);
  pthread_attr_t attr;
  pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));
  pthread_attr_init(&attr);
  pthread_attr_setstack(&attr, stack, stack_size);

  // put into the thread_list
  pthread_mutex_lock(&thread_list_lock);
  if(thread_list == NULL)
    thread_list = g_array_new(FALSE, FALSE, sizeof(struct thread_map *));

  struct thread_map *map = (struct thread_map *)malloc(sizeof(struct thread_map));
  map->stack = stack;
  map->thread = thread;
  map->stack_size = stack_size;
  g_array_append_val(thread_list, map);
  pthread_mutex_unlock(&thread_list_lock);

  pthread_create(thread, &attr, start_routine, args);
  pthread_join(*thread, NULL);
}

int psu_thread_migrate(char* node)
{
  ucontext_t context;
  getcontext(&context);
  CLIENT *client = create_client(node, PSU_THREAD, PSU_THREAD_V1, "tcp");
  Context *cont = (Context *)malloc(sizeof(Context));

  cont->uc_onstack = context.uc_onstack;
  cont->us_sigmask = context.uc_sigmask;
  cont->uc_mcsize = (u_int)context.uc_mcsize;
  cont->uc_mcontext.uc_mcontext_val = (char *)malloc(sizeof(*context.uc_mcontext));
  memcpy(cont->uc_mcontext.uc_mcontext_val, context.uc_mcontext, sizeof(*context.uc_mcontext));
  cont->uc_mcontext.uc_mcontext_len = (u_int)context.uc_mcsize;

  cont->ss_flag = context.uc_stack.ss_flags;
  cont->ss_size = (u_int)context.uc_stack.ss_size;

  pthread_mutex_lock(&thread_list_lock);
  bool has_found = false;
  for(int i = 0; i < thread_list->len; ++i)
  {
    if(pthread_equal(*(g_array_index(thread_list, struct thread_map *, i)->thread), pthread_self()))
    {
      struct thread_map *map = g_array_index(thread_list, struct thread_map *, i);
      cont->ss_sp.ss_sp_len = (u_int)map->stack_size;
      cont->ss_size = (u_int)map->stack_size;
      cont->ss_sp.ss_sp_val = (char *)malloc(sizeof(char) * map->stack_size);
      memcpy(cont->ss_sp.ss_sp_val, map->stack, sizeof(char) * map->stack_size);
      has_found = true;
      break;
    }
  }

  // must have found the thread or something has gone wrong
  assert(has_found);

  pthread_mutex_unlock(&thread_list_lock);

  void *result = NULL;
  migrate_1(cont, &result, client);
  clnt_destroy(client);
}