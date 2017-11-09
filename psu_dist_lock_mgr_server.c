#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <glib.h>
#include "utility.h"
#include "psu_dist_lock_mgr_msg.h"

static GArray *nodes = NULL;
static char *local_ip = NULL;

// ricart & agrawala algorithm variables
typedef struct _LockVar {
  int lock_number;
  int myseqno;
  int highestseqno;
  bool requesting_cs;
  GArray *deffered;
} LockVar;

GArray *lockvar_list = NULL;

LockVar *find_lockvar(int lock_number, bool create)
{
  // search in the list to find the lock variables
  LockVar *lockvar = NULL;
  for(int i = 0; i < lockvar_list->len; ++i)
  {
    if(g_array_index(lockvar_list, LockVar *, i)->lock_number == lock_number)
      lockvar = g_array_index(lockvar_list, LockVar *, i);
  }
  if(lockvar == NULL && create)
  {
    lockvar = (LockVar *)malloc(sizeof(LockVar));
    lockvar->lock_number = lock_number;
    lockvar->highestseqno = 0;
    lockvar->myseqno = 0;
    lockvar->requesting_cs = false;
    lockvar->deffered = g_array_new(FALSE, FALSE, sizeof(pthread_mutex_t *));
    g_array_append_val(lockvar_list, lockvar);
  }
  return lockvar;
}


bool_t init_lock_mgr_1_svc(char **node_str, void *result, struct svc_req *req)
{
  // initialize the global nodes list variable
  nodes = g_array_new(FALSE, FALSE, sizeof(char *));

  // get the local ip address
  GArray *local_ip_addresses = g_array_new(FALSE, FALSE, sizeof(char *));
  get_local_ip_addresses(local_ip_addresses);

  assert(local_ip_addresses->len != 0);

  int node_index = 0;
  for(char *pch = strtok(*node_str,","); pch != NULL; pch = strtok(NULL, ","))
  {
    // ignore the local ip address
    if(local_ip == NULL)
    {
      for(int i = 0; i < local_ip_addresses->len; ++i)
      {
        if(strcmp(pch, g_array_index(local_ip_addresses, char *, i)) == 0)
        {
          local_ip = g_array_index(local_ip_addresses, char *, i);
        }
      }
    }
    char *buf = (char *)malloc((strlen(pch) + 1) * sizeof(char));
    strncpy(buf, pch, strlen(pch) + 1);
    g_array_append_val(nodes, buf);
    ++node_index;
  }

  // must have found the local ip in the nodes information
  assert(local_ip != NULL);

  // remove the rest of ip addresses
  for(int i = 0; i < local_ip_addresses->len; ++i)
  {
    if(g_array_index(local_ip_addresses, char *, i) != local_ip)
      free(g_array_index(local_ip_addresses, char *, i));
  }

  g_array_free(local_ip_addresses, TRUE);

  // initialize global lock_var_list
  lockvar_list = g_array_new(FALSE, FALSE, sizeof(LockVar *));
  printf("Lock manager initialized, nodes information lists below:\n");
  for(int i = 0; i < local_ip_addresses->len; ++i)
    printf("node[%d] = %s\n", i, g_array_index(nodes, char *, i));

  return true;
}

bool_t acquire_lock_1_svc(int* number, void *result, struct svc_req *req)
{
  assert(nodes != NULL);
  printf("Acquiring lock number %d.\n", *number);

  LockVar *lockvar = find_lockvar(*number, true);
  lockvar->requesting_cs = true;
  lockvar->myseqno = lockvar->highestseqno + 1;
  for(int i = 0; i < nodes->len; ++i)
  {
    CLIENT *client = clnt_create(g_array_index(nodes, char *, i), PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, "udp");
    RequestPack *pack = (RequestPack *)malloc(sizeof(RequestPack));
    pack->lock_number = *number;
    // TODO: solve the id part
    pack->nodeid = 0;
    pack->seqno = lockvar->myseqno;
    void *result = NULL;
    printf("Send request to %s\n", g_array_index(nodes, char *, i));
    request_1(pack, &result, client);
  }
  printf("Lock %d Acquired.\n", *number);
  return true;
}

bool_t release_lock_1_svc(int* number, void *result, struct svc_req *req)
{
  printf("Releasing Lock %d.\n", *number);
  LockVar * lockvar = find_lockvar(*number, false);
  assert(lockvar != NULL);
  for(int i = 0; i < lockvar->deffered->len; ++i)
  {
    pthread_mutex_unlock(g_array_index(lockvar->deffered, pthread_mutex_t *, i));
    pthread_mutex_destroy(g_array_index(lockvar->deffered, pthread_mutex_t *, i));
  }

  g_array_free(lockvar->deffered, TRUE);
  lockvar->deffered = g_array_new(FALSE, FALSE, sizeof(pthread_mutex_t *));
  printf("Lock %d Released.\n", *number);
  return true;
}

bool_t request_1_svc(RequestPack *pack, void *result, struct svc_req *req)
{
  printf("Getting request for lock number %d, seqno is %d.\n", pack->lock_number, pack->seqno);
  LockVar *lockvar = find_lockvar(pack->lock_number, true);
  lockvar->highestseqno = MAX(pack->seqno, lockvar->highestseqno);

  if(lockvar->requesting_cs &&
    (pack->seqno > lockvar->myseqno || (pack->seqno == lockvar->myseqno && compare_ip(pack->nodeip, local_ip))))
  {
    pthread_mutex_t *mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);
    printf("Defer the reply for lock number %d.\n", pack->lock_number);

    g_array_append_val(lockvar->deffered, mutex);

    // lock the request
    pthread_mutex_lock(mutex);
    pthread_mutex_lock(mutex);
  }
  return true;
}

int psu_dist_lock_mgr_1_freeresult(SVCXPRT *transp,xdrproc_t xdr_result, caddr_t result)
{
  xdr_free(xdr_result,result);
  return 1;
}