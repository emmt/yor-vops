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

static inline array *coerce(int iarg, array* arr, int type)
{
    if (arr->type != type) {
        arr->data = ygeta_coerce(iarg, arr->data, arr->ntot, arr->dims,
                                 arr->type, type);
        arr->type = type;
    }
    return arr;
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

static inline int same_dims(
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
        return 1;
    }
    long ndims;
    if ((ndims = a[0]) != b[0]) {
        return 0;
    }
    for (long i = 1; i <= ndims; ++i) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
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
        y_error("arguments have unsupported type");
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
    int inplace = yarg_subroutine();
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
    int single = (x.type == Y_FLOAT);
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
// VOPS_SCALE

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
        y_error("usage: vops_update(y, alpha, x)");
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
        y_error("arguments have `x` and `y` incompatible types");
    }
    int single = (T == Y_FLOAT);
    if (!single) {
        T = Y_DOUBLE;
    }
    if (y.type != T) {
        if (y_index < 0) {
            y_error("argument `y` must not be an expression "
                    "or already have correct element type");
        }
        coerce(y_iarg, &y, T);
        yput_global(y_index, y_iarg);
    }
    if (x.type != T) {
        coerce(x_iarg, &x, T);
    }
    if (single) {
        vops_update_flt(y.data, alpha, x.data, x.ntot);
    } else {
        vops_update_dbl(y.data, alpha, x.data, x.ntot);
    }
    yarg_drop(argc - 1);
}
