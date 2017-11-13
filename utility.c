#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <glib.h>
#include <string.h>
#include <pthread.h>
#include "utility.h"

// multi-thread safe version of clnt_create with timeout set to 1hr
CLIENT *create_client(char *host, unsigned int prog, unsigned int vers, char *protocol)
{
  static pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&client_mutex);
  CLIENT *client = clnt_create(host, prog, vers, protocol);

  struct timeval TIMEOUT = { 60 * 60, 0 };
  if(client == NULL)
  {
    clnt_pcreateerror(host);
    exit(1);
  };
  if(strcmp(protocol, "udp") == 0)
    clnt_control(client, CLSET_RETRY_TIMEOUT, (char *)&TIMEOUT);

  clnt_control(client, CLSET_TIMEOUT, (char *)&TIMEOUT);
  pthread_mutex_unlock(&client_mutex);
  return client;
}

void get_local_ip_addresses(GArray *buf)
{
  struct ifaddrs *addrs = (struct ifaddrs *)malloc(sizeof(struct ifaddrs));
  getifaddrs(&addrs);

  for(struct ifaddrs *tmp = addrs; tmp != NULL; tmp = tmp->ifa_next)
  {
    if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
    {
      struct sockaddr_in *p_addr = (struct sockaddr_in *)tmp->ifa_addr;
      char *ip_address = (char *)malloc(sizeof(char) * 17);
      inet_ntop(AF_INET, &(p_addr->sin_addr), ip_address, INET_ADDRSTRLEN);
      g_array_append_val(buf, ip_address);
    }
  }
  freeifaddrs(addrs);
}


#ifdef AF_LINK
#include <net/if_dl.h>
uint8_t *get_ptr(struct ifaddrs *ifaptr)
{
  if (ifaptr->ifa_addr->sa_family == AF_LINK)
    return (unsigned char *)LLADDR((struct sockaddr_dl *)(ifaptr)->ifa_addr);
  else
    return NULL;
}
#endif
#ifdef AF_PACKET
#include <netpacket/packet.h>
uint8_t *get_ptr(struct ifaddrs *ifaptr)
{
    if (ifaptr->ifa_addr->sa_family == AF_PACKET)
        return (unsigned char *)((struct sockaddr_ll*)ifaptr->ifa_addr)->sll_addr;
    else
      return NULL;
}
#endif

u_quad_t get_mac_address()
{
  struct ifaddrs *ifap;
  u_quad_t result = 0;
  uint8_t *ptr = NULL;

  if (getifaddrs(&ifap) == 0)
  {
    for (struct ifaddrs *ifaptr = ifap; ifaptr != NULL; ifaptr = ifaptr->ifa_next)
    {
      ptr = get_ptr(ifaptr);

      if(ptr != NULL)
      {
        for (int i = 5; i >= 0; --i)
          result |= (u_quad_t) *ptr++ << (CHAR_BIT * i);
        if(result != 0)
          break;
      }
    }
    freeifaddrs(ifap);
  }
  return result;
}

struct thread_data
{
  struct svc_req *rqstp;
  SVCXPRT *transp;
  caddr_t argument;
  xdrproc_t _xdr_argument, _xdr_result;
  bool_t (*local)(char *, void *, struct svc_req *);
};

#define PACK_THREAD_DATA(ptr) \
(ptr)->local = local; \
(ptr)->rqstp = rqstp; \
(ptr)->transp = transp; \
(ptr)->argument = (caddr_t) argument; \
(ptr)->_xdr_result = _xdr_result; \
(ptr)->_xdr_argument = _xdr_argument;

#define UNPACK_THREAD_DATA(ptr) \
struct thread_data *ptr_data = (struct thread_data *)data; \
struct svc_req *rqstp = ptr_data->rqstp; \
SVCXPRT *transp = ptr_data->transp; \
caddr_t argument = ptr_data->argument; \
xdrproc_t _xdr_argument = ptr_data->xdr_argument, _xdr_result = ptr_data->xdr_result; \
bool_t (*local)(char *, void *, struct svc_req *) = ptr_data->local;


#define FREE_THREAD_DATA(ptr) \
{ \
  free((ptr)->argument); \
  free((ptr)); \
}

static struct thread_wrapper_data {
  void *(*run)(void *);
  void *thread_data;
  const char *procedure_name;
};

void *run_wrapper(void *wrapper_data)
{
  struct thread_wrapper_data *ptr = (struct thread_wrapper_data *)wrapper_data;
  (*ptr->run)(ptr->thread_data);
  printf("\033[33;1mProcedure %s finishes.\033[0m\n", ptr->procedure_name);
  free(ptr);
}

void dispatcher_mt(
  struct svc_req *rqstp,
  SVCXPRT *transp,
  const char **procedure_names,
  void (*parse_thread_data)(struct svc_req *rqstp, register SVCXPRT *transp, struct thread_data *thread_data),
  void *(*run)(void *))
{
  // if it's nullproc don't create a thread
  if(rqstp->rq_proc == NULLPROC)
  {
    (void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
    return;
  }

  struct thread_data *data_ptr=(struct thread_data *)malloc(sizeof(struct thread_data));

  parse_thread_data(rqstp, transp, data_ptr);

  printf("\033[32;1mProcedure calls %s\033[0m\n", procedure_names[rqstp->rq_proc]);

  struct thread_wrapper_data *wrapper = (struct thread_wrapper_data *)malloc(sizeof(struct thread_wrapper_data));
  wrapper->thread_data = data_ptr;
  wrapper->run = run;
  wrapper->procedure_name = procedure_names[rqstp->rq_proc];
  pthread_t *thread= (pthread_t *)malloc(sizeof(pthread_t));
  pthread_create(thread, NULL, &run_wrapper, (void *)data_ptr);
  pthread_detach(*thread);
}
