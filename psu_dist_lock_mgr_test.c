#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "psu_dist_lock_mgr.h"

pthread_t thread1;
pthread_t thread2;

void *thread1_func(void *param)
{
  psu_acquire_lock(1);
  printf("[Thread1]Doing Task.\n");
  sleep(5);
  psu_release_lock(1);
}

void *thread2_func(void *param)
{
  psu_acquire_lock(2);
  printf("[Thread2]Doing Task.\n");
  sleep(5);
  psu_release_lock(2);
}

int main(int argc, char **argv)
{
  char **nodes = &argv[1];
  psu_init_lock_mgr(nodes, argc - 1);

  printf("Spawn 2 thread.\n");
  pthread_create(&thread1, NULL, thread1_func, NULL);
  pthread_create(&thread2, NULL, thread2_func, NULL);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  printf("Exit tests.\n");
}