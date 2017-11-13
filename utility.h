#pragma once
#include <rpc/rpc.h>

void get_local_ip_addresses(GArray *buf);
u_quad_t get_mac_address();
CLIENT *create_client(char *host, unsigned int prog, unsigned int vers, char *protocol);

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
xdrproc_t _xdr_argument = ptr_data->_xdr_argument, _xdr_result = ptr_data->_xdr_result; \
bool_t (*local)(char *, void *, struct svc_req *) = ptr_data->local;

void dispatcher_mt(
  struct svc_req *rqstp,
  SVCXPRT *transp,
  const char **procedure_names,
  void (*parse_thread_data)(struct svc_req *rqstp, register SVCXPRT *transp, struct thread_data *thread_data),
  void *(*run)(void *));
