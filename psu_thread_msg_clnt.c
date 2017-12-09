#include <memory.h> /* for memset */
#include "psu_thread_msg.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

enum clnt_stat
migrate_1(rpc_ucontext *argp, void *clnt_res, CLIENT *clnt)
{
  return (clnt_call(clnt, MIGRATE,
                    (xdrproc_t) xdr_rpc_ucontext, (caddr_t) argp,
                    (xdrproc_t) xdr_void, (caddr_t) clnt_res,
                    TIMEOUT));
}