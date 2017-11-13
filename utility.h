#pragma once
#include <rpc/rpc.h>

typedef struct _GArray GArray;
void get_local_ip_addresses(GArray *buf);
u_quad_t get_mac_address();
CLIENT *create_client(char *host, unsigned int prog, unsigned int vers, char *protocol);

struct thread_data
{
  struct svc_req *rqstp;
  SVCXPRT *transp;
  caddr_t argument;
  caddr_t result;
  xdrproc_t _xdr_argument, _xdr_result;
  bool_t (*local)(char *, void *, struct svc_req *);
  int (*freeresult)(SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result);
};

#define PACK_THREAD_DATA(ptr) \
(ptr)->local = local; \
(ptr)->rqstp = rqstp; \
(ptr)->transp = transp; \
(ptr)->argument = (caddr_t) argument; \
(ptr)->result = (caddr_t) result; \
(ptr)->_xdr_result = _xdr_result; \
(ptr)->_xdr_argument = _xdr_argument;

void dispatcher_mt(
  struct svc_req *rqstp,
  SVCXPRT *transp,
  const char **procedure_names,
  void (*parse_thread_data)(struct svc_req *rqstp, register SVCXPRT *transp, struct thread_data *thread_data),
  int (*freeresult)(SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result));
