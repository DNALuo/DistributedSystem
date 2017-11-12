#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "psu_dist_lock_mgr.h"

struct param {
  int thread_id;
  int lock_num;
};

void *thread_func(void *p)
{
  struct param *args = (struct param *)p;
  psu_acquire_lock(args->lock_num);
  printf("[Thread%d-Lock%d]Doing Task.\n", args->thread_id, args->lock_num);
  sleep(5);
  printf("[Thread%d-Lock%d]Task Finished.\n", args->thread_id, args->lock_num);
  psu_release_lock(args->lock_num);
  free(args);
  return NULL;
}

int main(int argc, char **argv)
{
  char **nodes = &argv[1];
  psu_init_lock_mgr(nodes, argc - 1);

  printf("Spawn 3 threads.\n");

  pthread_t *threads[3];

  int lock_num[] = {1, 1, 2};
  for(int i = 0; i < 3; ++i)
  {
    pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));
    struct param *p = (struct param *)malloc(sizeof(struct param));
    p->lock_num = lock_num[i];
    p->thread_id = i;
    pthread_create(thread, NULL, thread_func, (void *)p);
    threads[i] = thread;
  }

  for(int i = 0; i < 3; ++i)
    pthread_join(*threads[i], NULL);

  printf("Exit tests.\n");
}