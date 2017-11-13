#include "psu_thread.h"
#include <ucontext.h>
#include <signal.h>

int psu_thread_create(psu_thread_info_t* t_info, void* (*start_routine)(void*), void* args)
{
  ucontext_t a;

}

int psu_thread_migrate(char* node)
{

}