/*
 * vops.i --
 *
 * Vectorized operations for Yorick.
 *
 *-----------------------------------------------------------------------------
 *
 * This file is part of VOPS for Yorick (https://github.com/emmt/yor-vops)
 * released under the MIT "Expat" license.
 *
 * Copyright (C) 2021: Éric Thiébaut <eric.thiebaut@univ-lyon1.fr>
 */

if (is_func(plug_in)) plug_in, "yor_vops";

local vops;
/* DOCUMENT Vectorized operations for Yorick

   Plug-in "vops.i" provides a number of optimized functions to perform basic
   linear algebra operations on arrays (as if they are vectors).  Available
   functions and subroutines are:

   - `vops_norm1(x)` yields the L1-norm of the real-valued array `x`, defined
     as `sum(abs(x))`.

   - `vops_norm2(x)` yields the L2-norm (Euclidean norm) of the real-valued
     array `x`, defined as `sqrt(sum(x*x))`.

   - `vops_norminf(x)` yields the infinite-norm of the real-valued array `x`,
     defined as `max(abs(x))`.

   - `vops_inner(x,y)` yields the inner product of the real-valued arrays `x`,
     and `y`, defined as `sum(x*y)`.

   - `vops_inner(w,x,y)` yields the "triple" inner product of the real-valued
     arrays `w`, `x`, and `y`, defined as `sum(w*x*y)`.

   - `vops_scale(alpha,x)` or `vops_scale(x,alpha)` yields the real-valued
     array `x` scaled by the factor `alpha`.  If `alpha = 0`, the result is
     filled by zeros whatever the values in `x` (hence `x` may contain NaN's in
     that case).

   - `vops_scale, x, alpha;` scales the real-valued array `x` scaled by the
     factor `alpha` in-place (i.e., overwriting the contents of `x`).

   - `vops_update, y, alpha, x;` computes `y += alpha*x` for arrays `x` and `y`
     and scalar factor `alpha` efficiently, overwriting the contents `y`.


   SEE ALSO: vops_norm1, vops_norm2, vops_norminf, vops_inner, vops_scale,
             vops_update.
 */

extern vops_norm1;
/* DOCUMENT nrm = vops_norm1(x);

      Compute the L1-norm of the real-valued array `x`, defined as:

          nrm = sum(abs(x));

   SEE ALSO: vops, vops_norm2, vops_norminf.
 */

extern vops_norm2;
/* DOCUMENT nrm = vops_norm2(x);

      Compute the L2-norm (Euclidean norm) of the real-valued array `x`,
      defined as:

          nrm = sqrt(sum(x*x));

   SEE ALSO: vops, vops_inner, vops_norm1, vops_norminf.
 */

extern vops_norminf;
/* DOCUMENT nrm = vops_norminf(x);

      Compute the infinite-norm of the real-valued array `x`, defined as:

          nrm = max(abs(x));

   SEE ALSO: vops, vops_norm1, vops_norm2.
 */

extern vops_inner;
/* DOCUMENT res = vops_inner([w,] x, y);

      Compute the inner product of the real-valued arrays `w`, `x`, and `y`,
      defined as:

          res = sum(w*x*y);   (if `w` specified)
          res = sum(x*y);     (else)

      except that arguments must all have the same dimensions.

   SEE ALSO: vops, vops_norm2.
 */

extern vops_scale;
/* DOCUMENT res = vops_scale(x, alpha);
         or res = vops_scale(alpha, x);
         or vops_scale, x, alpha;

      Scale the real-valued array `x` by the factor `alpha`.  The operation is
      done in-place if `vops_scale` is called as a sub-routine.

      If `alpha = 0`, the result is filled by zeros whatever the values in `x`
      (hence `x` may contain NaN's in that case).

   SEE ALSO: vops, vops_update.
 */

extern vops_update;
/* DOCUMENT vops_update, y, alpha, x;

      Compute `y += alpha*x` for arrays `x` and `y` and scalar factor `alpha`
      efficiently, overwriting the contents of `y`.  Arrays `x` and `y` must
      have the same dimensions.  The result is always of floating-point type
      (`float` if both `x` and `y` are of type `float`, `double` otherwise`).
      If `y` must be promoted to the result type, it must not be an expression.
      The result `y` is returned if called as a function.

   SEE ALSO: vops, vops_scale.
 */
