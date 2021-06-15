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

extern vops_norm1;
/* DOCUMENT nrm = vops_norm1(x);

      Compute the L1-norm of the real-valued array `x`, defined as:

          nrm = sum(abs(x));

   SEE ALSO: vops_norm2, vops_norminf, vops_inner.
 */

extern vops_norm2;
/* DOCUMENT nrm = vops_norm2(x);

      Compute the L2-norm (Euclidean norm) of the real-valued array `x`,
      defined as:

          nrm = sqrt(sum(x*x));

   SEE ALSO: vops_norm1, vops_norminf, vops_inner.
 */

extern vops_norminf;
/* DOCUMENT nrm = vops_norminf(x);

      Compute the infinite-norm of the real-valued array `x`, defined as:

          nrm = max(abs(x));

   SEE ALSO: vops_norm1, vops_norm2, vops_inner.
 */

extern vops_inner;
/* DOCUMENT res = vops_inner([w,] x, y);

      Compute the inner product of the real-valued arrays `w`, `x`, and `y`,
      defined as:

          res = sum(w*x*y);   (if `w` specified)
          res = sum(x*y);     (else)

      except that arguments must all have the same dimensions.

   SEE ALSO: vops_norm1, vops_norm2, vops_norminf.
 */

extern vops_scale;
/* DOCUMENT res = vops_scale(x, alpha);
         or res = vops_scale(alpha, x);
         or vops_scale, x, alpha;

      Scale the real-valued array `x` by the factor `alpha`.  The operation is
      done in-place if `vops_scale` is called as a sub-routine.

      If `alpha = 0`, the result is filled by zeros whatever the values in `x`
      (hence `x` may contain NaN's in that case).

   SEE ALSO: vops_norm1, vops_norm2, vops_norminf.
 */
