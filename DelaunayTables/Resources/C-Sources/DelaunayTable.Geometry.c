
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

static int calculate_divisionRatioMatrix(
    const size_t nDim,
    const double* const* const polygon,  // double[nDim+1][nDim]
          double*        const matrix    // double[nDim, nDim]
) {
    /*
     * # Matrix size constants
     * Since the dimensions of the output matrix are limited to [nDim, nDim],
     * n, m, lda and lwork all have the same value.
     */

    int status = SUCCESS;

    int*    ipiv = NULL;  // pivot indices (result discarded)
    double* work = NULL;  // work address for dgetri (result discarded)

    if (!(ipiv = (int*) MALLOC(nDim * sizeof(int)))) {
        status = FAILURE; goto finally;
    }
    if (!(work = (double*) MALLOC(nDim * sizeof(double)))) {
        status = FAILURE; goto finally;
    }

    for (size_t jRow = 0 ; jRow < nDim ; jRow++)
    for (size_t iCol = 0 ; iCol < nDim ; iCol++) {
        matrix[nDim*jRow+iCol] = polygon[jRow+1][iCol] - polygon[0][iCol];
    }

    LAPACK__dgetrf(
        nDim,     // m
        nDim,     // n
        matrix,   // a
        nDim,     // lda
        ipiv,     // ipiv
        &status   // info
    );
    if (status) goto finally;

    LAPACK__dgetri(
        nDim,     // n
        matrix,   // a
        nDim,     // lda
        ipiv,     // ipiv
        work,     // work
        nDim,     // lwork
        &status   // info
    );
    if (status) goto finally;

finally:

    if (ipiv) FREE(ipiv);
    if (work) FREE(work);

    return status;
}

int divisionRatioFromPolygonVertices(
    const size_t nDim,
    const double* const* const polygon,  // double[nDim+1][nDim]
    const double*        const point,    // double[nDim]
          double*        const ratio     // double[nDim+1]
) {
    int status = SUCCESS;

    double* matrix = NULL;  // double[nDim, nDim]
    double* rel_p  = NULL;  // double[nDim]

    if (!(matrix = (double*) MALLOC(nDim * nDim * sizeof(double)))) {
        status = FAILURE; goto finally;
    }
    if (!(rel_p = (double*) MALLOC(nDim * sizeof(double)))) {
        status = FAILURE; goto finally;
    }

    status = calculate_divisionRatioMatrix(nDim, polygon, matrix);
    if (status) {goto finally;}

    for (size_t i = 0 ; i < nDim ; i++) {
        rel_p[i] = point[i] - polygon[0][i];
    }

    LAPACK__dgemv(
        'N',        // trans
        nDim,       // m
        nDim,       // n
        1.0,        // alpha
        matrix,     // a
        nDim,       // lda
        rel_p,      // x
        1,          // incx
        0.0,        // beta
        &ratio[1],  // y
        1           // incy
    );
    if (status) {goto finally;}

    ratio[0] = 1.0;
    for(size_t i = 1 ; i < (nDim+1) ; i++) {
        ratio[0] -= ratio[i];
    }

finally:

    if (matrix) FREE(matrix);
    if (rel_p)  FREE(rel_p);

    return status;
}
