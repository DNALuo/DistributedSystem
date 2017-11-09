#include <glib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "psu_dist_lock_mgr_msg.h"
#include "psu_dist_lock_mgr.h"

static bool has_initialized = false;

void psu_init_lock_mgr(char** nodes, int num_nodes)
{
  char *node_str = (char *)malloc(sizeof(char) * 1024);
  int str_size = sizeof(char) * 1024;
  node_str[0] = '\0';

  // transform the nodes into one single string
  for(int i = 0; i < num_nodes; ++i)
  {
    // check if there's space available
    while(strlen(node_str) + strlen(nodes[i]) + 2 > str_size)
    {
      node_str = (char *)realloc(node_str, str_size + sizeof(char) * 1024);
      str_size += sizeof(char) * 1024;
    }
    strcat(node_str, nodes[i]);
    if(i != num_nodes - 1)
      strcat(node_str, ",");
  }
  CLIENT *client = clnt_create("127.0.0.1", PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, "udp");
  printf("Calling init_lock_mgr with %s.\n", node_str);
  void *result = NULL;
  init_lock_mgr_1(&node_str, &result, client);
  clnt_destroy(client);
  has_initialized = true;
  printf("Call init_lock_mgr finished.\n");
}

void psu_acquire_lock(int lock_number)
{
  if(!has_initialized)
  {
    printf("Error!The lock manager hasn't been initialized.!\n");
    return;
  }
  CLIENT *client = clnt_create("127.0.0.1", PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, "udp");
  printf("Calling acquire_lock with number %d.\n", lock_number);
  void *result = NULL;
  acquire_lock_1(&lock_number, &result, client);
  clnt_destroy(client);
  printf("Lock %d Acquired.\n", lock_number);
}

void psu_release_lock(int lock_number)
{
  if(!has_initialized)
  {
    printf("Error!The lock manager hasn't been initialized.!\n");
    return;
  }
  CLIENT *client = clnt_create("127.0.0.1", PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, "udp");
  printf("Calling release_lock with number %d.\n", lock_number);
  void *result = NULL;
  release_lock_1(&lock_number, &result, client);
  clnt_destroy(client);
  printf("Lock %d Released.\n", lock_number);
}
