#include <unistd.h>
#include "psu_dist_lock_mgr.h"

int main(int argc, char **argv)
{
  char **nodes = &argv[1];
  psu_init_lock_mgr(nodes, argc - 1);
  psu_acquire_lock(1);
  sleep(1);
  psu_release_lock(1);
}