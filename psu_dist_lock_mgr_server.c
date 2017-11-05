#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <glib.h>
#include "psu_dist_lock_mgr_msg.h"

static char **nodes = NULL;
static unsigned int num_nodes = 0;

// ricart & agrawala algorithm variables
typedef struct _LockVar {
  int lock_number;
  int myseqno;
  int highestseqno;
  bool requesting_cs;
} LockVar;

GArray *lockvar_list = NULL;

LockVar *find_lockvar(int lock_number)
{
  // search in the list to find the lock variables
  LockVar *lockvar = NULL;
  for(int i = 0; i < lockvar_list->len; ++i)
  {
    if(g_array_index(lockvar_list, LockVar *, i)->lock_number == lock_number)
      lockvar = g_array_index(lockvar_list, LockVar *, i);
  }
  if(lockvar == NULL)
  {
    lockvar = (LockVar *)malloc(sizeof(LockVar));
    lockvar->lock_number = lock_number;
    lockvar->highestseqno = 0;
    lockvar->myseqno = 0;
    lockvar->requesting_cs = false;
    g_array_append_val(lockvar_list, lockvar);
  }
  return lockvar;
}


void* init_lock_mgr_1_svc(char **node_str, struct svc_req *req)
{
  char *str = *node_str;
  for(num_nodes = 0; *str != '\0'; ++str)
  {
    if(*str == ',')
      ++num_nodes;
  }
  ++num_nodes;

  // initialize the global nodes list variable
  nodes = (char **)malloc(sizeof(char *) * num_nodes);

  char *pch = strtok(*node_str,",");
  int node_index = 0;
  while (pch != NULL)
  {
    nodes[node_index] = pch;
    ++node_index;
    pch = strtok(NULL, ",");
  }

  // initialize global lock_var_list
  lockvar_list = g_array_new(FALSE, FALSE, sizeof(LockVar *));
  return NULL;
}

void* acquire_lock_1_svc(int* number, struct svc_req *req)
{
  assert(nodes != NULL);
  //fprintf(fp, "Acquring lock number %d.\n", *number);

  LockVar *lockvar = find_lockvar(*number);
  lockvar->requesting_cs = true;
  lockvar->myseqno = lockvar->highestseqno + 1;
  for(int i = 0; i < num_nodes; ++i)
  {
    CLIENT *client = clnt_create(nodes[i], PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, "tcp");
    RequestPack *pack = (RequestPack *)malloc(sizeof(RequestPack));
    pack->lock_number = *number;
    // TODO: solve the id part
    pack->nodeid = 0;
    pack->seqno = lockvar->myseqno;
    request_1(pack, client);
  }
  return NULL;
}

void* release_lock_1_svc(int* number, struct svc_req *req)
{
  assert(nodes != NULL);
  return NULL;
}

void* request_1_svc(RequestPack *pack, struct svc_req *req)
{
  LockVar *lockvar = find_lockvar(pack->lock_number);
  lockvar->highestseqno = MAX(pack->seqno, lockvar->highestseqno);
  // TODO: solve the id part
  if(lockvar->requesting_cs &&
    (pack->seqno > lockvar->myseqno || (pack->seqno == lockvar->myseqno && pack->nodeid > 0)))
  {
    pthread_mutex_t *mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    //pthread_mutex_init(mutex, );
  } else {
    return NULL;
  }
}
