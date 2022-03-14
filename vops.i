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

         vops_norm1(x)                          -->  sum(abs(x))

     yields the L1-norm of the real-valued array `x`;

         vops_norm2(x)                          -->  sqrt(sum(x*x))

     yields the L2-norm (Euclidean norm) of the real-valued array `x`;

         vops_norminf(x)                        -->  max(abs(x))

     yields the infinite-norm of the real-valued array `x`;

         vops_inner(x,y)                        -->  sum(x*y)
         vops_inner(w,x,y)                      -->  sum(w*x*y)

     yields the inner product or "triple" innner product of the real-valued
     arrays `w`, `x`, and `y`;

         vops_scale(alpha, x)                   -->  alpha*x
         vops_scale(x, alpha)                   -->  alpha*x
         vops_scale, x, alpha;                  -->  x *= alpha

     yields the real-valued array `x` scaled by the factor `alpha` (if `alpha`
     is zero, the result is filled by zeros whatever the values in `x`, hence
     `x` may contain NaN's in that case); if called as a subroutine,
     `vops_scale` overwrites the contents of `x`;

         vops_update, y, alpha, x;              -->  y += alpha*x

     computes `y += alpha*x` for arrays `x` and `y` and scalar factor `alpha`,
     overwriting the contents of `y`;

         vops_combine(alpha, x, beta, y)        -->  alpha*x + beta*y
         vops_combine, dst, alpha, x, beta, y;  -->  dst = alpha*x + beta*y

     computes `alpha*x + beta*y` efficiently for arrays `x` and `y` and scalar
     factors `alpha` and `beta` efficiently; if called with 5 arguments,
     `vops_combine` automatically redefines or re-uses the contents of `dst`.


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
      done in-place if `vops_scale` is called as a subroutine.

      If `alpha = 0`, the result is filled by zeros whatever the values in `x`
      (hence `x` may contain NaN's in that case).

   SEE ALSO: vops, vops_combine, vops_update.
 */

extern vops_update;
/* DOCUMENT vops_update, y, alpha, x;
         or y = vops_update(y, alpha, x);

      Compute `y += alpha*x` efficiently for arrays `x` and `y`, and scalar
      factor `alpha`, overwriting the contents of `y`.  Arrays `x` and `y` must
      have the same dimensions.  The result is always of floating-point type
      (`float` if `x` and `y` are both of type `float`, `double` otherwise).
      If `y` must be promoted to the result type, it must not be an expression
      (i.e., `y` must be a simple variable for the caller).

      The updated result `y` is returned if called as a function.

   SEE ALSO: vops, vops_combine, vops_scale.
 */

extern vops_combine;
/* DOCUMENT vops_combine, dst, alpha, x, beta, y;
         or dst = vops_combine(alpha, x, beta, y);

      Compute `dst = alpha*x + beta*y` efficiently for arrays `x` and `y`, and
      scalar factors `alpha` and `beta`.  Arrays `x` and `y` must have the same
      dimensions.  The result `dst` is always of floating-point type (`float`
      if `x` and `y` are both of type `float`, `double` otherwise).

      When called with 5 arguments, `dst` is overwritten by the result.  If the
      contents of `dst` has the correct dimensions and type, the memory
      allocated for `dst` is re-used; otherwise, `dst` is automatically
      re-allocated but must not be an expression (i.e., `dst` must be a simple
      variable for the caller).

   SEE ALSO: vops, vops_scale, vops_update.
 */

local vops_tic, vops_toc, vops_flops, vops_time;
/* DOCUMENT vops_tic;
         or vops_toc;
         or vops_toc, n;
         or vops_toc();
         or vops_toc(n);
         or vops_flops, nops;
         or vops_flops(nops);

     The `vops_tic` subroutine set a timer for the next call to `vops_toc`
     which, when called as a subroutine, prints the elapsed time since previous
     `vops_tic` or, when called as a function, returns the elapsed time (see
     `timer` for the format and meaning of the elapsed time values).

     If optional argument `n` is provided, elapsed time is divided by this
     number.

     `vops_flops` is similar to `vops_toc` except that it computes the number
     of floating-point operations per second given `nops` the total number of
     floating-point operations and the consumed CPU time since the last call to
     `vops_tic`.

     External variable `vops_time` may be declared local before calling
     `vops_tic` to restrict the measurment of the elapsed time to the current
     context.

     Example:

         local vops_time;
         vops_tic;
         .....; // some code to benchmark
         vops_toc;


   SEE ALSO: timer, benchmark.
*/

func vops_tic
{
  extern vops_time;
  timer, (vops_time = array(double, 3));
}

func vops_toc(n)
{
  extern vops_time;
  timer, (t = array(double, 3));
  t -= vops_time;
  if (! is_void(n)) t /= n;
  if (! am_subroutine()) return t;
  write, format="Elapsed time: cpu = %g s / sys = %g s / wall = %g s\n",
    t(1), t(2), t(3);
}

func vops_flops(nops)
{
  extern vops_time;
  timer, (t = array(double, 3));
  flops = nops/(t - vops_time)(1); // CPU time
  if (! am_subroutine()) return flops;
  write, format="Computational power: %7.3f Gflops\n", format*1e-9;
}
