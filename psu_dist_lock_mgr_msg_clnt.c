#include <memory.h> /* for memset */
#include "psu_dist_lock_mgr_msg.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

enum clnt_stat init_lock_mgr_1(char **argp, void *clnt_res, CLIENT *clnt)
{
  return (clnt_call(clnt, INIT_LOCK_MGR,
                    (xdrproc_t) xdr_wrapstring, (caddr_t) argp,
                    (xdrproc_t) xdr_void, (caddr_t) clnt_res,
                    TIMEOUT));
}

enum clnt_stat acquire_lock_1(int *argp, void *clnt_res, CLIENT *clnt)
{
  return (clnt_call(clnt, ACQUIRE_LOCK,
                    (xdrproc_t) xdr_int, (caddr_t) argp,
                    (xdrproc_t) xdr_void, (caddr_t) clnt_res,
                    TIMEOUT));
}

enum clnt_stat release_lock_1(int *argp, void *clnt_res, CLIENT *clnt)
{
  return (clnt_call(clnt, RELEASE_LOCK,
                    (xdrproc_t) xdr_int, (caddr_t) argp,
                    (xdrproc_t) xdr_void, (caddr_t) clnt_res,
                    TIMEOUT));
}

enum clnt_stat request_1(RequestPack *argp, void *clnt_res, CLIENT *clnt)
{
  return (clnt_call(clnt, REQUEST,
                    (xdrproc_t) xdr_RequestPack, (caddr_t) argp,
                    (xdrproc_t) xdr_void, (caddr_t) clnt_res,
                    TIMEOUT));
}