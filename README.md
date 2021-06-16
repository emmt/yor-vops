Vectorized operations for Yorick
================================

The `VOPS` plug-in for [Yorick](http://yorick.github.com/) provides a number of
optimized functions to perform basic linear algebra operations on arrays (as if
they are vectors).  These functions can serve as fast building blocks for more
advanced algorithms (e.g., the linear conjugate gradient method).


Usage
-----

Functions and subroutines provided by `VOPS` are:

    vops_norm1(x)                          -->  sum(abs(x))

yields the L1-norm of the real-valued array `x`;

    vops_norm2(x)                          -->  sqrt(sum(x*x))

yields the L2-norm (Euclidean norm) of the real-valued array `x`;

    vops_norminf(x)                        -->  max(abs(x))

yields the infinite-norm of the real-valued array `x`;

    vops_inner(x,y)                        -->  sum(x*y)
    vops_inner(w,x,y)                      -->  sum(w*x*y)

yields the inner product or "triple" innner product of the real-valued arrays
`w`, `x`, and `y`;

    vops_scale(alpha, x)                   -->  alpha*x
    vops_scale(x, alpha)                   -->  alpha*x
    vops_scale, x, alpha;                  -->  x *= alpha

yields the real-valued array `x` scaled by the factor `alpha` (if `alpha` is
zero, the result is filled by zeros whatever the values in `x`, hence `x` may
contain NaN's in that case); if called as a subroutines, `vops_scale`
overwrites the contents of `x`;

    vops_update, y, alpha, x;              -->  y += alpha*x

computes `y += alpha*x` for arrays `x` and `y` and scalar factor `alpha`,
overwriting the contents `y`;

    vops_combine(alpha, x, beta, y)        -->  alpha*x + beta*y
    vops_combine, dst, alpha, x, beta, y;  -->  dst = alpha*x + beta*y

computes `alpha*x + beta*y` efficiently for arrays `x` and `y` and scalar
factors `alpha` and `beta` efficiently; if called with 5 arguments,
`vops_combine` automatically redefine or re-use the contents of `dst`.


Benefits
--------

One of the benefits of using `VOPS` is the acceleration of computations (see
[Performances](#performances)).  Another advantage is that single-precision
computations can be performed if arrays are of type `float`.


Performances
------------

The following timings have been obtained for double precision floating-point
(`double`) variables:

| Description    | Code                               |       Power | Complexity |
|:---------------|:-----------------------------------|------------:|-----------:|
| L1-norm        | `sum(abs(x))`                      |  1.7 Gflops |      `2⋅n` |
|                | `vops_norm1(x)`                    | 21.5 Gflops |            |
| L2-norm        | `sqrt(sum(x*x))`                   |  1.9 Gflops |      `2⋅n` |
|                | `vops_norm2(x)`                    | 24.8 Gflops |            |
| Inf-norm       | `max(abs(x))`                      |  2.7 Gflops |      `2⋅n` |
|                | `vops_norminf(x)`                  | 20.5 Gflops |            |
| Inner product  | `sum(x*y)`                         |  1.9 Gflops |      `2⋅n` |
|                | `vops_inner(x,y)`                  | 15.6 Gflops |            |
| Triple product | `sum(w*x*y)`                       |  2.1 Gflops |      `3⋅n` |
|                | `vops_inner(x,x,y)`                | 16.0 Gflops |            |
| Scale          | `x *= alpha`                       |  1.6 Gflops |        `n` |
|                | `vops_scale,x,alpha`               |  3.4 Gflops |            |
| Update         | `y += alpha*x`                     |  2.1 Gflops |      `2⋅n` |
|                | `vops_update,y,alpha,x`            | 10.6 Gflops |            |
| Combine        | `z = alpha*x + beta*y`             |  1.9 Gflops |      `3⋅n` |
|                | `z = vops_combine(alpha,x,beta,y)` |  8.4 Gflops |            |
|                | `vops_combine,z,alpha,x,beta,y`    | 13.5 Gflops |            |

The "Power" is the number of floating-point operations per second which is
computed by dividing the CPU time by the number of floating-point operations
(given in column "Complexity" with `n` the number of elements).  The code for
benchmarking is in file [`vops-tests.i`](./vops-tests.i).

Speed-up may be up to a factor 13 thanks to
[SIMD](https://en.wikipedia.org/wiki/SIMD) instructions.  These results have
been obtained on a **single core** of an AMD Ryzen Threadripper 2950X 16-core
processor at 4.1 GHz with a plug-in compiled by Clang (version 10.0) with [loop
vectorization](https://en.wikipedia.org/wiki/Automatic_vectorization).
Compiler and optimization flags have been configured by (see
["Installation"](#installation)):

```sh
./configure cc=clang copt='-O3 -mavx2 -mfma -ffast-math'
```


Installation
------------

In short, building and installing the plug-in can be as quick as:
```sh
cd $BUILD_DIR
$SRC_DIR/configure
make
make install
```
where `$BUILD_DIR` is the build directory (at your convenience) and `$SRC_DIR`
is the source directory of the plug-in code.  The build and source directories
can be the same in which case, call `./configure` to configure for building.

If the plug-in has been properly installed, it is sufficient to use any of its
functions to automatically load the plug-in.  You may force the loading of the
plug-in by something like:
```cpp
#include "vops.i"
```
or
```cpp
require, "vops.i";
```
in your code.

More detailled installation explanations are given below.

1. You must have Yorick installed on your machine.

2. Unpack the plug-in code somewhere.

3. Configure for compilation.  There are two possibilities:

   For an **in-place build**, go to the source directory of the plug-in code
   and run the configuration script:
   ```sh
   cd SRC_DIR
   ./configure
   ```
   To see the configuration options, type:
   ```sh
   ./configure --help
   ```

   To compile in a **different build directory**, say `$BUILD_DIR`, create the
   build directory, go to the build directory, and run the configuration
   script:
   ```sh
   mkdir -p $BUILD_DIR
   cd $BUILD_DIR
   $SRC_DIR/configure
   ```
   where `$SRC_DIR` is the path to the source directory of the plug-in code.
   To see the configuration options, type:
   ```sh
   $SRC_DIR/configure --help
   ```

4. Compile the code:
   ```sh
   make clean
   make
   ```

5. Install the plug-in in Yorick directories:
   ```sh
   make install
   ```
