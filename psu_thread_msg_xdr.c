#include "psu_thread_msg.h"

bool_t xdr_rpc__sigset_t (XDR *xdrs, rpc__sigset_t *objp)
{
  register int32_t *buf;

  int i;

  if (xdrs->x_op == XDR_ENCODE) {
    buf = XDR_INLINE (xdrs, ( 16 ) * BYTES_PER_XDR_UNIT);
    if (buf == NULL) {
      if (!xdr_vector (xdrs, (char *)objp->__val, 16,
                       sizeof (u_long), (xdrproc_t) xdr_u_long))
        return FALSE;
    } else {
      {
        register u_long *genp;

        for (i = 0, genp = objp->__val;
             i < 16; ++i) {
          IXDR_PUT_U_LONG(buf, *genp++);
        }
      }
    }
    return TRUE;
  } else if (xdrs->x_op == XDR_DECODE) {
    buf = XDR_INLINE (xdrs, ( 16 ) * BYTES_PER_XDR_UNIT);
    if (buf == NULL) {
      if (!xdr_vector (xdrs, (char *)objp->__val, 16,
                       sizeof (u_long), (xdrproc_t) xdr_u_long))
        return FALSE;
    } else {
      {
        register u_long *genp;

        for (i = 0, genp = objp->__val;
             i < 16; ++i) {
          *genp++ = IXDR_GET_U_LONG(buf);
        }
      }
    }
    return TRUE;
  }

  if (!xdr_vector (xdrs, (char *)objp->__val, 16,
                   sizeof (u_long), (xdrproc_t) xdr_u_long))
    return FALSE;
  return TRUE;
}

bool_t xdr_rpc_libc_fpxreg (XDR *xdrs, rpc_libc_fpxreg *objp)
{
  register int32_t *buf;

  int i;

  if (xdrs->x_op == XDR_ENCODE) {
    buf = XDR_INLINE (xdrs, (1 +  4  + 3 )* BYTES_PER_XDR_UNIT);
    if (buf == NULL) {
      if (!xdr_vector (xdrs, (char *)objp->significand, 4,
                       sizeof (u_short), (xdrproc_t) xdr_u_short))
        return FALSE;
      if (!xdr_u_short (xdrs, &objp->exponent))
        return FALSE;
      if (!xdr_vector (xdrs, (char *)objp->padding, 3,
                       sizeof (u_short), (xdrproc_t) xdr_u_short))
        return FALSE;
    } else {
      {
        register u_short *genp;

        for (i = 0, genp = objp->significand;
             i < 4; ++i) {
          IXDR_PUT_U_SHORT(buf, *genp++);
        }
      }
      IXDR_PUT_U_SHORT(buf, objp->exponent);
      {
        register u_short *genp;

        for (i = 0, genp = objp->padding;
             i < 3; ++i) {
          IXDR_PUT_U_SHORT(buf, *genp++);
        }
      }
    }
    return TRUE;
  } else if (xdrs->x_op == XDR_DECODE) {
    buf = XDR_INLINE (xdrs, (1 +  4  + 3 )* BYTES_PER_XDR_UNIT);
    if (buf == NULL) {
      if (!xdr_vector (xdrs, (char *)objp->significand, 4,
                       sizeof (u_short), (xdrproc_t) xdr_u_short))
        return FALSE;
      if (!xdr_u_short (xdrs, &objp->exponent))
        return FALSE;
      if (!xdr_vector (xdrs, (char *)objp->padding, 3,
                       sizeof (u_short), (xdrproc_t) xdr_u_short))
        return FALSE;
    } else {
      {
        register u_short *genp;

        for (i = 0, genp = objp->significand;
             i < 4; ++i) {
          *genp++ = IXDR_GET_U_SHORT(buf);
        }
      }
      objp->exponent = IXDR_GET_U_SHORT(buf);
      {
        register u_short *genp;

        for (i = 0, genp = objp->padding;
             i < 3; ++i) {
          *genp++ = IXDR_GET_U_SHORT(buf);
        }
      }
    }
    return TRUE;
  }

  if (!xdr_vector (xdrs, (char *)objp->significand, 4,
                   sizeof (u_short), (xdrproc_t) xdr_u_short))
    return FALSE;
  if (!xdr_u_short (xdrs, &objp->exponent))
    return FALSE;
  if (!xdr_vector (xdrs, (char *)objp->padding, 3,
                   sizeof (u_short), (xdrproc_t) xdr_u_short))
    return FALSE;
  return TRUE;
}

bool_t xdr_rpc_libc_xmmreg (XDR *xdrs, rpc_libc_xmmreg *objp)
{
  register int32_t *buf;

  int i;
  if (!xdr_vector (xdrs, (char *)objp->element, 4,
                   sizeof (u_int32_t), (xdrproc_t) xdr_u_int32_t))
    return FALSE;
  return TRUE;
}

bool_t xdr_rpc_libc_fpstate (XDR *xdrs, rpc_libc_fpstate *objp)
{
  register int32_t *buf;

  int i;
  if (!xdr_u_int16_t (xdrs, &objp->cwd))
    return FALSE;
  if (!xdr_u_int16_t (xdrs, &objp->swd))
    return FALSE;
  if (!xdr_u_int16_t (xdrs, &objp->ftw))
    return FALSE;
  if (!xdr_u_int16_t (xdrs, &objp->fop))
    return FALSE;
  if (!xdr_u_int64_t (xdrs, &objp->rip))
    return FALSE;
  if (!xdr_u_int64_t (xdrs, &objp->rdp))
    return FALSE;
  if (!xdr_u_int32_t (xdrs, &objp->mxcsr))
    return FALSE;
  if (!xdr_u_int32_t (xdrs, &objp->mxcr_mask))
    return FALSE;
  if (!xdr_vector (xdrs, (char *)objp->_st, 8,
                   sizeof (rpc_libc_fpxreg), (xdrproc_t) xdr_rpc_libc_fpxreg))
    return FALSE;
  if (!xdr_vector (xdrs, (char *)objp->_xmm, 16,
                   sizeof (rpc_libc_xmmreg), (xdrproc_t) xdr_rpc_libc_xmmreg))
    return FALSE;
  if (!xdr_vector (xdrs, (char *)objp->padding, 24,
                   sizeof (u_int32_t), (xdrproc_t) xdr_u_int32_t))
    return FALSE;
  return TRUE;
}

bool_t xdr_rpc_greg_t (XDR *xdrs, rpc_greg_t *objp)
{
  register int32_t *buf;

  if (!xdr_long (xdrs, objp))
    return FALSE;
  return TRUE;
}

bool_t xdr_rpc_gregset_t (XDR *xdrs, rpc_gregset_t objp)
{
  register int32_t *buf;

  if (!xdr_vector (xdrs, (char *)objp, 23,
                   sizeof (rpc_greg_t), (xdrproc_t) xdr_rpc_greg_t))
    return FALSE;
  return TRUE;
}

bool_t xdr_rpc_mcontext (XDR *xdrs, rpc_mcontext *objp)
{
  register int32_t *buf;

  int i;

  if (xdrs->x_op == XDR_ENCODE) {
    if (!xdr_rpc_gregset_t (xdrs, objp->gregs))
      return FALSE;
    if (!xdr_rpc_libc_fpstate (xdrs, &objp->fpregs))
      return FALSE;
    buf = XDR_INLINE (xdrs, ( 8 ) * BYTES_PER_XDR_UNIT);
    if (buf == NULL) {
      if (!xdr_vector (xdrs, (char *)objp->__reserved1, 8,
                       sizeof (u_long), (xdrproc_t) xdr_u_long))
        return FALSE;
    } else {
      {
        register u_long *genp;

        for (i = 0, genp = objp->__reserved1;
             i < 8; ++i) {
          IXDR_PUT_U_LONG(buf, *genp++);
        }
      }
    }
    return TRUE;
  } else if (xdrs->x_op == XDR_DECODE) {
    if (!xdr_rpc_gregset_t (xdrs, objp->gregs))
      return FALSE;
    if (!xdr_rpc_libc_fpstate (xdrs, &objp->fpregs))
      return FALSE;
    buf = XDR_INLINE (xdrs, ( 8 ) * BYTES_PER_XDR_UNIT);
    if (buf == NULL) {
      if (!xdr_vector (xdrs, (char *)objp->__reserved1, 8,
                       sizeof (u_long), (xdrproc_t) xdr_u_long))
        return FALSE;
    } else {
      {
        register u_long *genp;

        for (i = 0, genp = objp->__reserved1;
             i < 8; ++i) {
          *genp++ = IXDR_GET_U_LONG(buf);
        }
      }
    }
    return TRUE;
  }

  if (!xdr_rpc_gregset_t (xdrs, objp->gregs))
    return FALSE;
  if (!xdr_rpc_libc_fpstate (xdrs, &objp->fpregs))
    return FALSE;
  if (!xdr_vector (xdrs, (char *)objp->__reserved1, 8,
                   sizeof (u_long), (xdrproc_t) xdr_u_long))
    return FALSE;
  return TRUE;
}

bool_t xdr_rpc_stack_t (XDR *xdrs, rpc_stack_t *objp)
{
  register int32_t *buf;

  if (!xdr_int (xdrs, &objp->ss_flag))
    return FALSE;
  if (!xdr_bytes (xdrs, (char **)&objp->ss_sp.ss_sp_val, (u_int *) &objp->ss_sp.ss_sp_len, ~0))
    return FALSE;
  return TRUE;
}

bool_t xdr_rpc_ucontext (XDR *xdrs, rpc_ucontext *objp)
{
  register int32_t *buf;

  if (!xdr_u_long (xdrs, &objp->uc_flags))
    return FALSE;
  if (!xdr_rpc_stack_t (xdrs, &objp->uc_stack))
    return FALSE;
  if (!xdr_rpc_mcontext (xdrs, &objp->uc_mcontext))
    return FALSE;
  if (!xdr_rpc__sigset_t (xdrs, &objp->uc_sigmask))
    return FALSE;
  if (!xdr_rpc_libc_fpstate (xdrs, &objp->__fpregs_mem))
    return FALSE;
  return TRUE;
}