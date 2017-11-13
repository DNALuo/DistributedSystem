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

static void *run(void *data)
{
  struct thread_data *ptr_data = (struct thread_data *)data;
  struct svc_req *rqstp = ptr_data->rqstp;
  SVCXPRT *transp = ptr_data->transp;
  caddr_t argument = ptr_data->argument;
  caddr_t result = ptr_data->result;
  xdrproc_t _xdr_argument = ptr_data->_xdr_argument, _xdr_result = ptr_data->_xdr_result;
  bool_t (*local)(char *, void *, struct svc_req *) = ptr_data->local;
  int (*freeresult)(SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result) = ptr_data->freeresult;
  bool_t retval;

  retval = (bool_t) (*local)((char *)argument, (void *)result, rqstp);
  if (retval > 0 && !svc_sendreply(transp, (xdrproc_t) _xdr_result, (char *)&result)) {
    svcerr_systemerr (transp);
  }
  if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) argument)) {
    fprintf (stderr, "%s", "unable to free arguments");
    exit (1);
  }
  if (!freeresult (transp, _xdr_result, (caddr_t) result))
    fprintf (stderr, "%s", "unable to free results");

  printf("\033[33;1mProcedure %d finishes.\033[0m\n", rqstp->rq_proc);
  free(ptr_data->argument);
  free(ptr_data->result);
  free(ptr_data->rqstp);
  free(ptr_data);
  return NULL;
}

void dispatcher(
  struct svc_req *rqstp,
  SVCXPRT *transp,
  void (*parse_thread_data)(struct svc_req *rqstp, register SVCXPRT *transp, struct thread_data *thread_data),
  int (*freeresult)(SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result))
{
  // if it's nullproc don't create a thread
  if(rqstp->rq_proc == NULLPROC)
  {
    (void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
    return;
  }

  struct thread_data *data_ptr=(struct thread_data *)malloc(sizeof(struct thread_data));

  // do copy the rqstp object since it may change after the thread is invoked
  data_ptr->transp = transp;
  data_ptr->rqstp = (struct svc_req *)malloc(sizeof(struct svc_req));
  memcpy(data_ptr->rqstp, rqstp, sizeof(struct svc_req));
  data_ptr->freeresult = freeresult;

  // parse the argument and result
  parse_thread_data(rqstp, transp, data_ptr);

  printf("\033[32;1mProcedure calls %d\033[0m\n", rqstp->rq_proc);

  pthread_t *thread= (pthread_t *)malloc(sizeof(pthread_t));
  pthread_create(thread, NULL, &run, (void *)data_ptr);
  pthread_detach(*thread);
}
