local tic, toc, tic_toc_timer;
/* DOCUMENT tic;
         or toc;
         or toc, n;
         or toc();
         or toc(n);

     The `tic` subroutine set a timer for the next call to `toc` which, when
     called as a subroutine, prints the elapsed time since previous `tic` or,
     when called as a function, returns the elapsed time (see `timer` for the
     format and meaning of the elapsed time values).

     If optional argument N is provided, elapsed time is divided by this
     number.

     External variable `tic_toc_timer` may be declared local before calling
     `tic` to restrict the measurment of the elapsed time to the current
     context.

     Example:

         local tic_toc_timer;
         tic;
         .....; // some code to benchmark
         toc;


   SEE ALSO: timer, benchmark.
*/

func tic
{
  extern tic_toc_timer;
  timer, (tic_toc_timer = array(double, 3));
}

func toc(n)
{
  extern tic_toc_timer;
  timer, (t = array(double, 3));
  t -= tic_toc_timer;
  if (! is_void(n)) t /= n;
  if (! am_subroutine()) return t;
  write, format="Elapsed time: cpu = %g s / sys = %g s / wall = %g s\n",
    t(1), t(2), t(3);
}

func flops(nops)
{
  extern tic_toc_timer;
  timer, (t = array(double, 3));
  f = nops/(t - tic_toc_timer)(1);
  if (! am_subroutine()) return ;
  write, format="Computational power: %7.3f Gflops\n", f*1e-9;
}

dims = 10000;
w = random(dims);
x = random(dims);
y = random(dims);
warmup = [2,1000,1000];
repeat = 100000;

write, format="Test %s:\n", "empty";
k = repeat; random_n, warmup; tic; while (--k >= 0) r0 = 0.0; toc, repeat;
write, "";

nops = 2*numberof(x);
write, format="%30s: ", "sum(x*y)";
k = repeat; random_n, warmup; tic; while (--k >= 0) r1 = sum(x*y); flops, nops*repeat;
write, format="%30s: ", "vops_inner(x, y)";
k = repeat; random_n, warmup; tic; while (--k >= 0) r2 = vops_inner(x, y); flops, nops*repeat;
write, format="Results: %g / %g\n\n", r2, r1;

nops = 3*numberof(x);
write, format="%30s: ", "sum(w*x*y)";
k = repeat; random_n, warmup; tic; while (--k >= 0) r1 = sum(x*x*y); flops, nops*repeat;
write, format="%30s: ", "vops_inner(w, x, y)";
k = repeat; random_n, warmup; tic; while (--k >= 0) r2 = vops_inner(x, x, y); flops, nops*repeat;
write, format="Results:  %g / %g\n\n", r2, r1;

nops = 2*numberof(x);
write, format="%30s: ", "sum(abs(x))";
k = repeat; random_n, warmup; tic; while (--k >= 0) r1 = sum(abs(x)); flops, nops*repeat;
write, format="%30s: ", "vops_norm1(x)";
k = repeat; random_n, warmup; tic; while (--k >= 0) r2 = vops_norm1(x); flops, nops*repeat;
write, format="Results: %g / %g\n\n", r2, r1;

nops = 2*numberof(x);
write, format="%30s: ", "sqrt(sum(x*x))";
k = repeat; random_n, warmup; tic; while (--k >= 0) r1 = sqrt(sum(x*x)); flops, nops*repeat;
write, format="%30s: ", "vops_norm2(x)";
k = repeat; random_n, warmup; tic; while (--k >= 0) r2 = vops_norm2(x); flops, nops*repeat;
write, format="Results: %g / %g\n\n", r2, r1;

nops = 2*numberof(x);
write, format="%30s: ", "max(abs(x))";
k = repeat; random_n, warmup; tic; while (--k >= 0) r1 = max(abs(x)); flops, nops*repeat;
write, format="%30s: ", "vops_norminf(x)";
k = repeat; random_n, warmup; tic; while (--k >= 0) r2 = vops_norminf(x); flops, nops*repeat;
write, format="Results: %g / %g\n\n", r2, r1;

alpha = random();
nops = numberof(x);
write, format="%30s: ", "alpha*x";
k = repeat; random_n, warmup; tic; while (--k >= 0) r1 = alpha*x; flops, nops*repeat;
write, format="%30s: ", "vops_scale(x, alpha)";
k = repeat; random_n, warmup; tic; while (--k >= 0) r2 = vops_scale(x, alpha); flops, nops*repeat;
write, format="Results: max(abs(dif)) = %g\n\n", max(abs(r2 - r1));

alpha = random();
nops = 2*numberof(x);
y1 = y;
write, format="%30s: ", "y += alpha*x";
k = repeat; random_n, warmup; tic; while (--k >= 0) y1 += alpha*x; flops, nops*repeat;
y2 = y;
write, format="%30s: ", "vops_update, y, alpha, x";
k = repeat; random_n, warmup; tic; while (--k >= 0) vops_update, y2, alpha, x; flops, nops*repeat;
y1 = y; y1 += alpha*x;
y2 = y; vops_update, y2, alpha, x;
write, format="Results: max(abs(dif)) = %g\n\n", max(abs(y2 - y1));
