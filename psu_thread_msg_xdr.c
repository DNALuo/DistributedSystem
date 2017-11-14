#include "./psu_thread_msg.h"

bool_t xdr_Context (XDR *xdrs, Context *objp)
{
  register int32_t *buf;


  if (xdrs->x_op == XDR_ENCODE) {
    buf = XDR_INLINE (xdrs, 3 * BYTES_PER_XDR_UNIT);
    if (buf == NULL) {
      if (!xdr_u_int (xdrs, &objp->uc_mcsize))
        return FALSE;
      if (!xdr_int (xdrs, &objp->uc_onstack))
        return FALSE;
      if (!xdr_int (xdrs, &objp->us_sigmask))
        return FALSE;

    } else {
      IXDR_PUT_U_LONG(buf, objp->uc_mcsize);
      IXDR_PUT_LONG(buf, objp->uc_onstack);
      IXDR_PUT_LONG(buf, objp->us_sigmask);
    }
    if (!xdr_bytes (xdrs, (char **)&objp->uc_mcontext.uc_mcontext_val, (u_int *) &objp->uc_mcontext.uc_mcontext_len, ~0))
      return FALSE;
    if (!xdr_int (xdrs, &objp->ss_flag))
      return FALSE;
    if (!xdr_u_int (xdrs, &objp->ss_size))
      return FALSE;
    if (!xdr_bytes (xdrs, (char **)&objp->ss_sp.ss_sp_val, (u_int *) &objp->ss_sp.ss_sp_len, ~0))
      return FALSE;
    return TRUE;
  } else if (xdrs->x_op == XDR_DECODE) {
    buf = XDR_INLINE (xdrs, 3 * BYTES_PER_XDR_UNIT);
    if (buf == NULL) {
      if (!xdr_u_int (xdrs, &objp->uc_mcsize))
        return FALSE;
      if (!xdr_int (xdrs, &objp->uc_onstack))
        return FALSE;
      if (!xdr_int (xdrs, &objp->us_sigmask))
        return FALSE;

    } else {
      objp->uc_mcsize = IXDR_GET_U_LONG(buf);
      objp->uc_onstack = IXDR_GET_LONG(buf);
      objp->us_sigmask = IXDR_GET_LONG(buf);
    }
    if (!xdr_bytes (xdrs, (char **)&objp->uc_mcontext.uc_mcontext_val, (u_int *) &objp->uc_mcontext.uc_mcontext_len, ~0))
      return FALSE;
    if (!xdr_int (xdrs, &objp->ss_flag))
      return FALSE;
    if (!xdr_u_int (xdrs, &objp->ss_size))
      return FALSE;
    if (!xdr_bytes (xdrs, (char **)&objp->ss_sp.ss_sp_val, (u_int *) &objp->ss_sp.ss_sp_len, ~0))
      return FALSE;
    return TRUE;
  }

  if (!xdr_u_int (xdrs, &objp->uc_mcsize))
    return FALSE;
  if (!xdr_int (xdrs, &objp->uc_onstack))
    return FALSE;
  if (!xdr_int (xdrs, &objp->us_sigmask))
    return FALSE;
  if (!xdr_bytes (xdrs, (char **)&objp->uc_mcontext.uc_mcontext_val, (u_int *) &objp->uc_mcontext.uc_mcontext_len, ~0))
    return FALSE;
  if (!xdr_int (xdrs, &objp->ss_flag))
    return FALSE;
  if (!xdr_u_int (xdrs, &objp->ss_size))
    return FALSE;
  if (!xdr_bytes (xdrs, (char **)&objp->ss_sp.ss_sp_val, (u_int *) &objp->ss_sp.ss_sp_len, ~0))
    return FALSE;
  return TRUE;
}