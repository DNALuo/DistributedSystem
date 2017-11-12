#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <glib.h>
#include <unistd.h>
#include "utility.h"
#include "psu_dist_lock_mgr_msg.h"

static GArray *clients = NULL;
static u_quad_t mac = 0;
static bool has_initialized = false;

// ricart & agrawala algorithm variables
typedef struct _LockVar {
  pthread_mutex_t lock; // for local threads acquiring the same lock
  int lock_number;
  int myseqno;
  int highestseqno;
  bool requesting_cs;
  GArray *deffered;
} LockVar;

GArray *lockvar_list = NULL;
pthread_mutex_t lockvar_list_lock;

#define WAIT_FOR_TRUE(cond) \
{ \
  while(!(cond)) \
    usleep(50000); \
}

LockVar *find_lockvar(int lock_number, bool create)
{
  // search in the list to find the lock variables
  LockVar *lockvar = NULL;
  pthread_mutex_lock(&lockvar_list_lock);
  for(int i = 0; i < lockvar_list->len; ++i)
  {
    if(g_array_index(lockvar_list, LockVar *, i)->lock_number == lock_number)
    {
      lockvar = g_array_index(lockvar_list, LockVar *, i);
      pthread_mutex_unlock(&lockvar_list_lock);
      return lockvar;
    }
  }
  // if not found
  if(create)
  {
    lockvar = (LockVar *)malloc(sizeof(LockVar));
    lockvar->lock_number = lock_number;
    lockvar->highestseqno = 0;
    lockvar->myseqno = 0;
    lockvar->requesting_cs = false;
    lockvar->deffered = g_array_new(FALSE, FALSE, sizeof(pthread_mutex_t *));
    pthread_mutex_init(&(lockvar->lock), NULL);
    g_array_append_val(lockvar_list, lockvar);
  }
  pthread_mutex_unlock(&lockvar_list_lock);
  return lockvar;
}

void initialize_global_variable()
{
  clients = g_array_new(FALSE, FALSE, sizeof(CLIENT *));
  lockvar_list = g_array_new(FALSE, FALSE, sizeof(LockVar *));
  mac = get_mac_address();
  pthread_mutex_init(&lockvar_list_lock, NULL);
  assert(mac != 0);
  assert(clients != NULL);
  assert(lockvar_list != NULL);
}

bool_t init_lock_mgr_1_svc(char **node_str, void *res, struct svc_req *req)
{
  // get the local ip address
  GArray *local_ip_addresses = g_array_new(FALSE, FALSE, sizeof(char *));
  get_local_ip_addresses(local_ip_addresses);

  assert(local_ip_addresses->len != 0);

  // copy the node_str since strtok will modify the string
  char *node_str_copy = (char *)malloc(sizeof(char) * (strlen(*node_str) + 1));
  strncpy(node_str_copy, *node_str, strlen(*node_str) + 1);

  bool has_found;
  for(char *pch = strtok(node_str_copy, ","); pch != NULL; pch = strtok(NULL, ","))
  {
    // ignore the local ip address
    has_found = false;
    for(int i = 0; i < local_ip_addresses->len; ++i)
    {
      if(strcmp(pch, g_array_index(local_ip_addresses, char *, i)) == 0)
      {
        has_found = true;
        break;
      }
    }
    if(has_found)
      continue;

    CLIENT *client = create_client(pch, PSU_DIST_LOCK_MGR, PSU_DIST_LOCK_MGR_V1, "udp");
    g_array_append_val(clients, client);
  }

  // remove the local ip addresses
  for(int i = 0; i < local_ip_addresses->len; ++i)
    free(g_array_index(local_ip_addresses, char *, i));

  g_array_free(local_ip_addresses, TRUE);

  /*
  printf("Lock manager initialized, nodes information lists below:\n");
  for(int i = 0; i < nodes->len; ++i)
    printf("node[%d] = %s\n", i, g_array_index(nodes, char *, i));
  */
  has_initialized = true;

  return true;
}

bool_t acquire_lock_1_svc(int* number, void *res, struct svc_req *req)
{
  // busy waiting for the initialization to finish
  WAIT_FOR_TRUE(has_initialized);

  printf("Acquiring lock number %d.\n", *number);

  LockVar *lockvar = find_lockvar(*number, true);

  // lock the lockvar for local 2 threads acquiring the same lock
  pthread_mutex_lock(&(lockvar->lock));

  lockvar->requesting_cs = true;
  lockvar->myseqno = lockvar->highestseqno + 1;
  for(int i = 0; i < clients->len; ++i)
  {
    RequestPack *pack = (RequestPack *)malloc(sizeof(RequestPack));
    pack->lock_number = *number;
    pack->mac = mac;
    pack->seqno = lockvar->myseqno;
    void *result = NULL;
    printf("Send request\n");
    request_1(pack, &result, g_array_index(clients, CLIENT *, i));
  }
  printf("Lock %d Acquired.\n", *number);
  return true;
}

bool_t release_lock_1_svc(int* number, void *res, struct svc_req *req)
{
  // busy waiting for the initialization to finish
  WAIT_FOR_TRUE(has_initialized);

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

  pthread_mutex_unlock(&(lockvar->lock));
  
  printf("Lock %d Released.\n", *number);
  return true;
}

// this function may be called before the initialization from other servers
bool_t request_1_svc(RequestPack *pack, void *res, struct svc_req *req)
{
  printf("Getting request for lock number %d, seqno is %d.\n", pack->lock_number, pack->seqno);
  LockVar *lockvar = find_lockvar(pack->lock_number, true);
  lockvar->highestseqno = MAX(pack->seqno, lockvar->highestseqno);

  if(lockvar->requesting_cs &&
    (pack->seqno > lockvar->myseqno ||
    (pack->seqno == lockvar->myseqno && pack->mac > mac)))
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
  return 1;
}