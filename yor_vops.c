/*
 * yor_vops.c --
 *
 * Implements Vectorized operations for Yorick.
 *
 *-----------------------------------------------------------------------------
 *
 * This file is part of VOPS for Yorick (https://github.com/emmt/yor-vops)
 * released under the MIT "Expat" license.
 *
 * Copyright (C) 2021: Éric Thiébaut <eric.thiebaut@univ-lyon1.fr>
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>

#include <pstdlib.h>
#include <play.h>
#include <yapi.h>

/* Define some macros to get rid of some GNU extensions when not compiling
   with GCC. */
#if ! (defined(__GNUC__) && __GNUC__ > 1)
#   define __attribute__(x)
#   define __inline__
#   define __FUNCTION__        ""
#   define __PRETTY_FUNCTION__ ""
#endif

PLUG_API void y_error(const char *) __attribute__ ((noreturn));

//static inline void push_string(const char* str)
//{
//  ypush_q(NULL)[0] = p_strcpy(str);
//}

static int promote_type(int a, int b)
{
    if (a == b) {
        return a;
    }
    if ((unsigned)a <= Y_COMPLEX && (unsigned)b <= Y_COMPLEX) {
        return (a < b ? b : a);
    }
    return -1;
}

typedef struct array {
    long dims[Y_DIMSIZE];
    long ntot;
    int type;
    void* data;
} array;

static inline array* get_array(int iarg, array* arr)
{
    arr->data = ygeta_any(iarg, &arr->ntot, arr->dims, &arr->type);
    return arr;
}

static inline void coerce(int iarg, array* arr, int type)
{
    if (arr->type != type) {
        arr->data = ygeta_coerce(iarg, arr->data, arr->ntot, arr->dims,
                                 arr->type, type);
        arr->type = type;
    }
}

static inline array* get_real_array(int iarg, array* arr, bool inplace)
{
    long index;
    if (inplace) {
        index = yget_ref(iarg);
        if (index < 0) {
            y_error("argument must not be an expression");
        }
    } else {
        index = -1;
    }
    get_array(iarg, arr);
    if (arr->type != Y_DOUBLE && arr->type != Y_FLOAT) {
        if ((unsigned)arr->type > Y_DOUBLE) {
            y_error("argument is not real-valued");
        }
        coerce(iarg, arr, Y_DOUBLE);
        if (inplace) {
            // Redefine caller's variable.
            yput_global(index, iarg);
        }
    }
    return arr;
}

static inline bool same_dims(
    const long* a,
    const long* b)
{
    long z[1] = {0};
    if (a == NULL) {
        a = z;
    }
    if (b == NULL) {
        b = z;
    }
    if (a == b) {
        return true;
    }
    long ndims;
    if ((ndims = a[0]) != b[0]) {
        return false;
    }
    for (long i = 1; i <= ndims; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

/* DOCUMENT res = vmult(x, y [,z]);
         or vmult, x, y, z;
 */

/* DOCUMENT res = vcombine();
         or vcombine, res, [alpha,] x, [beta,] y;
 */

//#define ENCODE_(func, T) \
//    static inline T func(T a, T b) { return (a < b ? a : b); }
//ENCODE_(min_flt, float);
//ENCODE_(min_dbl, double);
//#undef ENCODE_

#define ENCODE_(func, T) \
    static inline T func(T a, T b) { return (a < b ? b : a); }
ENCODE_(max_flt, float);
ENCODE_(max_dbl, double);
#undef ENCODE_

//-----------------------------------------------------------------------------
// VOPS_NORM1

#define ENCODE_(func, T, abs)               \
    static T func(                          \
        const T* x,                         \
        long     n)                         \
    {                                       \
        T s = 0;                            \
        for (long i = 0; i < n; ++i) {      \
            s += abs(x[i]);                 \
        }                                   \
        return s;                           \
    }
ENCODE_(vops_norm1_flt, float, fabsf);
ENCODE_(vops_norm1_dbl, double, fabs);
#undef ENCODE_

void Y_vops_norm1(int argc)
{
    if (argc != 1) {
        y_error("usage: vops_norm1(x)");
    }
    array x;
    get_real_array(0, &x, false);
    double nrm;
    if (x.type == Y_DOUBLE) {
        nrm = vops_norm1_dbl(x.data, x.ntot);
    } else {
        nrm = vops_norm1_flt(x.data, x.ntot);
    }
    ypush_double(nrm);
}

//-----------------------------------------------------------------------------
// VOPS_NORM2

#define ENCODE_(func, T, abs, sqrt)             \
    static T func(                              \
        const T* x,                             \
        long     n)                             \
    {                                           \
        if (n == 1) {                           \
            return abs(x[0]);                   \
        } else {                                \
            T s = 0;                            \
            for (long i = 0; i < n; ++i) {      \
                s += x[i]*x[i];                 \
            }                                   \
            return sqrt(s);                     \
        }                                       \
    }
ENCODE_(vops_norm2_flt, float, fabsf, sqrtf);
ENCODE_(vops_norm2_dbl, double, fabs, sqrt);
#undef ENCODE_

void Y_vops_norm2(int argc)
{
    if (argc != 1) {
        y_error("usage: vops_norm2(x)");
    }
    array x;
    get_real_array(0, &x, false);
    double nrm;
    if (x.type == Y_DOUBLE) {
        nrm = vops_norm2_dbl(x.data, x.ntot);
    } else {
        nrm = vops_norm2_flt(x.data, x.ntot);
    }
    ypush_double(nrm);
}

//-----------------------------------------------------------------------------
// VOPS_NORMINF

#define ENCODE_(func, T, abs, max)              \
    static T func(                              \
        const T* x,                             \
        long     n)                             \
    {                                           \
        if (n == 1) {                           \
            return abs(x[0]);                   \
        } else {                                \
            T s = 0;                            \
            for (long i = 0; i < n; ++i) {      \
                s = max(s, abs(x[i]));          \
            }                                   \
            return s;                           \
        }                                       \
    }
ENCODE_(vops_norminf_flt, float, fabsf, max_flt);
ENCODE_(vops_norminf_dbl, double, fabs, max_dbl);
#undef ENCODE_

void Y_vops_norminf(int argc)
{
    if (argc != 1) {
        y_error("usage: vops_norminf(x)");
    }
    array x;
    get_real_array(0, &x, false);
    double nrm;
    if (x.type == Y_DOUBLE) {
        nrm = vops_norminf_dbl(x.data, x.ntot);
    } else {
        nrm = vops_norminf_flt(x.data, x.ntot);
    }
    ypush_double(nrm);
}

//-----------------------------------------------------------------------------
// VOPS_INNER

#define ENCODE_(func, T)                        \
    static T func(                              \
        const T* restrict x,                    \
        const T* restrict y,                    \
        long n)                                 \
    {                                           \
        T s = 0;                                \
        for (long i = 0; i < n; ++i) {          \
            s += x[i]*y[i];                     \
        }                                       \
        return s;                               \
    }
ENCODE_(vops_inner2_flt, float);
ENCODE_(vops_inner2_dbl, double);
#undef ENCODE_

#define ENCODE_(func, T)                        \
    static T func(                              \
        const T* restrict w,                    \
        const T* restrict x,                    \
        const T* restrict y,                    \
        long n)                                 \
    {                                           \
        T s = 0;                                \
        for (long i = 0; i < n; ++i) {          \
            s += w[i]*x[i]*y[i];                \
        }                                       \
        return s;                               \
    }
ENCODE_(vops_inner3_flt, float);
ENCODE_(vops_inner3_dbl, double);
#undef ENCODE_

void Y_vops_inner(int argc)
{
    int w_iarg, x_iarg, y_iarg;
    if (argc == 2) {
        w_iarg = -1;
        x_iarg = argc - 1;
        y_iarg = argc - 2;
    } else if (argc == 3) {
        w_iarg = argc - 1;
        x_iarg = argc - 2;
        y_iarg = argc - 3;
    } else {
        y_error("usage: vops_inner([w,] x, y)");
    }
    array w, x, y;
    if (argc > 2) {
        get_array(w_iarg, &w);
        if ((unsigned)w.type > Y_DOUBLE) {
            y_error("argument `w` is not real-valued");
        }
    }
    get_array(x_iarg, &x);
    if ((unsigned)x.type > Y_DOUBLE) {
        y_error("argument `x` is not real-valued");
    }
    get_array(y_iarg, &y);
    if ((unsigned)y.type > Y_DOUBLE) {
        y_error("argument `y` is not real-valued");
    }
    if (!same_dims(x.dims, y.dims) ||
        (argc > 2 && !same_dims(x.dims, w.dims))) {
        y_error("arguments must have the same dimensions");
    }
    int T = promote_type(x.type, y.type);
    if (argc > 2) {
        T = promote_type(w.type, T);
    }
    if (T < 0) {
        y_error("arguments have unsupported types");
    }
    if (T != Y_FLOAT) {
        T = Y_DOUBLE;
    }
    double res;
    coerce(x_iarg, &x, T);
    coerce(y_iarg, &y, T);
    if (argc > 2) {
        coerce(w_iarg, &y, T);
        if (T == Y_DOUBLE) {
            res = vops_inner3_dbl(w.data, x.data, y.data, x.ntot);
        } else {
            res = vops_inner3_flt(w.data, x.data, y.data, x.ntot);
        }
    } else {
        if (T == Y_DOUBLE) {
            res = vops_inner2_dbl(x.data, y.data, x.ntot);
        } else {
            res = vops_inner2_flt(x.data, y.data, x.ntot);
        }
    }
    ypush_double(res);
}

//-----------------------------------------------------------------------------
// VOPS_SCALE

#define ENCODE_(func, T)                        \
    static void func(                           \
        T*       dst,                           \
        T        alpha,                         \
        const T* src,                           \
        long              n)                    \
    {                                           \
        if (alpha == 0) {                       \
            memset(dst, 0, n*sizeof(T));        \
        } else if (alpha == 1) {                \
            if (dst != src) {                   \
                memcpy(dst, src, n*sizeof(T));  \
            }                                   \
        } else if (alpha == -1) {               \
            for (long i = 0; i < n; ++i) {      \
                dst[i] = -src[i];               \
            }                                   \
        } else if (dst != src) {                \
           for (long i = 0; i < n; ++i) {       \
               dst[i] = alpha*src[i];           \
           }                                    \
       } else {                                 \
           for (long i = 0; i < n; ++i) {       \
               dst[i] *= alpha;                 \
           }                                    \
       }                                        \
    }
ENCODE_(vops_scale_flt, float);
ENCODE_(vops_scale_dbl, double);
#undef ENCODE_

void Y_vops_scale(int argc)
{
    if (argc != 2) {
        y_error("usage: vops_scale(x, alpha)");
    }
    bool inplace = yarg_subroutine();
    int x_iarg = argc - 1;
    int a_iarg = argc - 2;
    if (!inplace && yarg_rank(a_iarg) > 0) {
        // Assume order of arguments have been swapped.
        int tmp = a_iarg;
        a_iarg = x_iarg;
        x_iarg = tmp;
    }
    array x;
    get_real_array(x_iarg, &x, inplace);
    double alpha = ygets_d(a_iarg);
    bool single = (x.type == Y_FLOAT);
    void* dst;
    if (inplace) {
        dst = x.data;
    } else if (single) {
        dst = ypush_f(x.dims);
    } else {
        dst = ypush_d(x.dims);
    }
    if (single) {
        vops_scale_flt(dst, alpha, x.data, x.ntot);
    } else {
        vops_scale_dbl(dst, alpha, x.data, x.ntot);
    }
}

//-----------------------------------------------------------------------------
// VOPS_UPDATE

#define ENCODE_(func, T)                        \
    static void func(                           \
        T*       y,                             \
        T        alpha,                         \
        const T* x,                             \
        long     n)                             \
    {                                           \
        if (alpha == 1) {                       \
            for (long i = 0; i < n; ++i) {      \
                y[i] += x[i];                   \
            }                                   \
        } else if (alpha == -1) {               \
            for (long i = 0; i < n; ++i) {      \
                y[i] -= x[i];                   \
            }                                   \
        } else if (alpha != 0) {                \
           for (long i = 0; i < n; ++i) {       \
               y[i] += alpha*x[i];              \
           }                                    \
       }                                        \
    }
ENCODE_(vops_update_flt, float);
ENCODE_(vops_update_dbl, double);
#undef ENCODE_

void Y_vops_update(int argc)
{
    if (argc != 3) {
        y_error("usage: vops_update, y, alpha, x;");
    }
    int y_iarg = argc - 1;
    int a_iarg = argc - 2;
    int x_iarg = argc - 3;
    long y_index = yget_ref(y_iarg);
    array y;
    get_array(y_iarg, &y);
    if ((unsigned)y.type > Y_DOUBLE) {
        y_error("argument `y` is not real-valued");
    }
    double alpha = ygets_d(a_iarg);
    array x;
    get_array(x_iarg, &x);
    if ((unsigned)x.type > Y_DOUBLE) {
        y_error("argument `x` is not real-valued");
    }
    if (!same_dims(x.dims, y.dims)) {
        y_error("arguments `x` and `y` must have the same dimensions");
    }
    int T = promote_type(x.type, y.type);
    if (T < 0) {
        y_error("arguments `x` and `y` have unsupported types");
    }
    if (T != Y_FLOAT) {
        T = Y_DOUBLE;
    }
    if (y.type != T) {
        if (y_index < 0) {
            y_error("argument `y` must not be an expression or must have "
                    "correct element type (`float` if `x` and `y` both "
                    "have `float` elements, or `double` otherwise)");
        }
        coerce(y_iarg, &y, T);
        yput_global(y_index, y_iarg);
    }
    coerce(x_iarg, &x, T);
    if (T == Y_FLOAT) {
        vops_update_flt(y.data, alpha, x.data, x.ntot);
    } else {
        vops_update_dbl(y.data, alpha, x.data, x.ntot);
    }
    yarg_drop(argc - 1);
}

//-----------------------------------------------------------------------------
// VOPS_COMBINE

#define ENCODE_(func, T, scale)                                 \
    static void func(                                           \
        T*       dst,                                           \
        T        alpha,                                         \
        const T* x,                                             \
        T        beta,                                          \
        const T* y,                                             \
        long     n)                                             \
    {                                                           \
        /* May swap operands to reduce alternatives. */         \
        if (alpha != beta &&                                    \
            (beta == 0 ||                                       \
             (beta == 1 && alpha != 0) ||                       \
             (beta == -1 && alpha != 0 && alpha != 1))) {       \
            const T* ptr = x; x = y; y = ptr;                   \
            T val = alpha; alpha = beta; beta = val;            \
        }                                                       \
        if (alpha == 0) {                                       \
            scale(dst, beta, y, n);                             \
        } else if (alpha == 1) {                                \
            /* beta is not 0 */                                 \
            if (beta == 1) {                                    \
                for (long i = 0; i < n; ++i) {                  \
                    dst[i] = x[i] + y[i];                       \
                }                                               \
            } else if (beta == -1) {                            \
                for (long i = 0; i < n; ++i) {                  \
                    dst[i] = x[i] - y[i];                       \
                }                                               \
            } else {                                            \
                for (long i = 0; i < n; ++i) {                  \
                    dst[i] = x[i] + beta*y[i];                  \
                }                                               \
            }                                                   \
        } else if (alpha == -1) {                               \
            /* beta is neither 0, nor 1 */                      \
            if (beta == -1) {                                   \
                for (long i = 0; i < n; ++i) {                  \
                    dst[i] = -x[i] - y[i];                      \
                }                                               \
            } else {                                            \
                for (long i = 0; i < n; ++i) {                  \
                    dst[i] = beta*y[i] - x[i];                  \
                }                                               \
            }                                                   \
        } else {                                                \
            /* alpha and beta are neither 0, nor ±1 */          \
            for (long i = 0; i < n; ++i) {                      \
                dst[i] = alpha*x[i] + beta*y[i];                \
            }                                                   \
        }                                                       \
    }
ENCODE_(vops_combine_flt, float,  vops_scale_flt);
ENCODE_(vops_combine_dbl, double, vops_scale_dbl);
#undef ENCODE_

void Y_vops_combine(int argc)
{
    int d_iarg, a_iarg, x_iarg, b_iarg, y_iarg;
    long d_index;
    if (argc == 5) {
        d_iarg = argc - 1;
        a_iarg = argc - 2;
        x_iarg = argc - 3;
        b_iarg = argc - 4;
        y_iarg = argc - 5;
        d_index = yget_ref(d_iarg); // before any other operations
    } else if (argc == 4 && !yarg_subroutine()) {
        d_iarg = -1;
        a_iarg = argc - 1;
        x_iarg = argc - 2;
        b_iarg = argc - 3;
        y_iarg = argc - 4;
        d_index = -1;
    } else {
        y_error(yarg_subroutine() ?
                "usage: vops_combine, dst, alpha, x, beta, y;":
                "usage: vops_combine([dst,] alpha, x, beta, y)");
        return;
    }

    // Get input arguments.
    double alpha = ygets_d(a_iarg);
    array x;
    get_array(x_iarg, &x);
    if ((unsigned)x.type > Y_DOUBLE) {
        y_error("argument `x` is not real-valued");
    }
    double beta = ygets_d(b_iarg);
    array y;
    get_array(y_iarg, &y);
    if ((unsigned)y.type > Y_DOUBLE) {
        y_error("argument `y` is not real-valued");
    }
    if (!same_dims(x.dims, y.dims)) {
        y_error("arguments `x` and `y` must have the same dimensions");
    }
    int T = promote_type(x.type, y.type);
    if (T < 0) {
        y_error("arguments `x` and `y` have unsupported types");
    }
    if (T != Y_FLOAT) {
        T = Y_DOUBLE;
    }

    // Get/create output array.
    int drop = 0;
    void* dst = NULL;
    if (d_iarg >= 0) {
        int d_type = yarg_typeid(d_iarg);
        if (d_type == T && yarg_rank(d_iarg) == x.dims[0]) {
            array d;
            get_array(d_iarg, &d);
            if (same_dims(x.dims, d.dims)) {
                // Re-use the destination.
                dst = d.data;
                drop = d_iarg;
            }
        }
        if (dst == NULL) {
            if (d_index < 0) {
                y_error("destination must have the correct size and type "
                        "or must be a simple variable");
            }
            if (d_type != Y_VOID) {
                // Free memory that may be used by the destination variable.
                // Replace stack item by nil, then redefine variable.
                ypush_nil();
                yarg_swap(0, d_iarg + 1); // +1 because of push
                yarg_drop(1);
                yput_global(d_index, d_iarg);
            }
        }
    }
    if (dst == NULL) {
        // Allocate output array.
        if (T == Y_FLOAT) {
            dst = ypush_f(x.dims);
        } else {
            dst = ypush_d(x.dims);
        }
        if (d_index >= 0) {
            yput_global(d_index, 0);
        }
    }

    // Convert input arrays and call function.
    coerce(x_iarg, &x, T);
    coerce(y_iarg, &y, T);
    if (T == Y_FLOAT) {
        vops_combine_flt(dst, alpha, x.data, beta, y.data, x.ntot);
    } else {
        vops_combine_dbl(dst, alpha, x.data, beta, y.data, x.ntot);
    }
    if (drop > 0) {
        // Leave result on top of the stack.
        yarg_drop(d_iarg);
    }
}
