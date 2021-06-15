Vectorized operations for Yorick
================================

The `VOPS` plug-in for [Yorick](http://yorick.github.com/) provides a number of
optimized functions to perform basic linear algebra operations on arrays (as if
they are vectors).  These functions can serve as fast building blocks for more
advanced algorithms (e.g., the linear conjugate gradient method).

- `vops_norm1(x)` yields the L1-norm of the real-valued array `x`, defined as
  `sum(abs(x))`.

- `vops_norm2(x)` yields the L2-norm (Euclidean norm) of the real-valued array
  `x`, defined as `sqrt(sum(x*x))`.

- `vops_norminf(x)` yields the infinite-norm of the real-valued array
  `x`, defined as `max(abs(x))`.

- `vops_inner(x,y)` yields the inner product of the real-valued arrays `x`, and `y`,
  defined as `sum(x*y)`.

- `vops_inner(w,x,y)` yields the "triple" inner product of the real-valued
  arrays `w`, `x`, and `y`, defined as `sum(w*x*y)`.

- `vops_scale(alpha,x)` or `vops_scale(x,alpha)` yields the real-valued array
  `x` scaled by the factor `alpha`.  If `alpha = 0`, the result is filled by
  zeros whatever the values in `x` (hence `x` may contain NaN's in that case).

- `vops_scale, x, alpha;` scale the real-valued array `x` scaled by the factor
  `alpha` in-place (i.e., overwriting the contents of `x`).

The following results have been obtained for double precision floating points
(`double`):

| Description    | Code                |       Power |
|:---------------|:--------------------|------------:|
| L1-norm        | `sum(abs(x))`       |  1.7 Gflops |
|                | `vops_norm1(x)`     | 20.5 Gflops |
| L2-norm        | `sqrt(sum(x*x))`    |  1.9 Gflops |
|                | `vops_norm2(x)`     | 24.8 Gflops |
| Inf-norm       | `max(abs(x))`       |  2.7 Gflops |
|                | `vops_norminf(x)`   | 20.5 Gflops |
| Inner product  | `sum(x*y)`          |  1.9 Gflops |
|                | `vops_inner(x,y)`   | 15.6 Gflops |
| Triple product | `sum(w*x*y)`        |  2.1 Gflops |
|                | `vops_inner(x,x,y)` | 16.0 Gflops |


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
