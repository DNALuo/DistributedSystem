#include "psu_dist_lock_mgr_msg.h"

bool_t xdr_RequestPack (XDR *xdrs, RequestPack *objp)
{
  register int32_t *buf;

  if (!xdr_int (xdrs, &objp->lock_number))
    return FALSE;
  if (!xdr_int (xdrs, &objp->seqno))
    return FALSE;
  if (!xdr_string (xdrs, &objp->nodeip, 17))
    return FALSE;
  return TRUE;
}
