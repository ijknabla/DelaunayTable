
#include "DelaunayTable.Geometry.h"


static void LAPACK__dgetrf(
          LAPACK_INTEGER           m,
          LAPACK_INTEGER           n,
          LAPACK_DOUBLE_PRECISION* a,     /// a[lda, n]
          LAPACK_INTEGER           lda,
          LAPACK_INTEGER*          ipiv,  /// ipiv[(min(m, n))]
          LAPACK_INTEGER*          info
) {
    dgetrf_(
        &m,
        &n,
        a,
        &lda,
        ipiv,
        info
    );
}

static void LAPACK__dgetri(
          LAPACK_INTEGER           n,
          LAPACK_DOUBLE_PRECISION* a,     /// a[lda, n]
          LAPACK_INTEGER           lda,
    const LAPACK_INTEGER*          ipiv,  /// ipiv[n]
          LAPACK_DOUBLE_PRECISION* work,  /// work[max(1, lwork)]
          LAPACK_INTEGER           lwork,
          LAPACK_INTEGER*          info
) {
    dgetri_(
        &n,
        a,
        &lda,
        (LAPACK_INTEGER*) ipiv,
        work,
        &lwork,
        info
    );
}

static void LAPACK__dgemv(
          LAPACK_CHARACTER         trans,  /// one of 'N', 'T', 'C'
          LAPACK_INTEGER           m,
          LAPACK_INTEGER           n,
          LAPACK_DOUBLE_PRECISION  alpha,
    const LAPACK_DOUBLE_PRECISION* a,      /// a[lda, n]
          LAPACK_INTEGER           lda,
    const LAPACK_DOUBLE_PRECISION* x,      /// x[...]
          LAPACK_INTEGER           incx,
          LAPACK_DOUBLE_PRECISION  beta,
          LAPACK_DOUBLE_PRECISION* y,
          LAPACK_INTEGER           incy
) {
    dgemv_(
        &trans,
        &m,
        &n,
        &alpha,
        (LAPACK_DOUBLE_PRECISION*) a,
        &lda,
        (LAPACK_DOUBLE_PRECISION*) x,
        &incx,
        &beta,
        y,
        &incy
    );
}
