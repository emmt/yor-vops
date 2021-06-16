dims = 10000;
w = random(dims);
x = random(dims);
y = random(dims);
warmup = [2,1000,1000];
repeat = 100000;

func vops_warmup
/* DOCUMENT vops_warmup;

     Warm-up the CPU by performing a large number of operations and start the
     timer.

   SEE ALSO: vops_tic.
 */
{
    random_n, 1000, 1000;
    vops_tic;
}

write, format="Test %s:\n", "empty";
k = repeat; vops_warmup; while (--k >= 0) r0 = 0.0; vops_toc, repeat;
write, "";

// L1-norm.
nops = 2*numberof(x);

write, format="%30s: ", "sum(abs(x))";
k = repeat; vops_warmup; while (--k >= 0) r1 = sum(abs(x));
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="%30s: ", "vops_norm1(x)";
k = repeat; vops_warmup; while (--k >= 0) r2 = vops_norm1(x);
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="Results: %g / %g\n\n", r2, r1;

// L2-norm.
nops = 2*numberof(x);

write, format="%30s: ", "sqrt(sum(x*x))";
k = repeat; vops_warmup; while (--k >= 0) r1 = sqrt(sum(x*x));
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="%30s: ", "vops_norm2(x)";
k = repeat; vops_warmup; while (--k >= 0) r2 = vops_norm2(x);
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="Results: %g / %g\n\n", r2, r1;

// Infinite-norm.
nops = 2*numberof(x);

write, format="%30s: ", "max(abs(x))";
k = repeat; vops_warmup; while (--k >= 0) r1 = max(abs(x));
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="%30s: ", "vops_norminf(x)";
k = repeat; vops_warmup; while (--k >= 0) r2 = vops_norminf(x);
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="Results: %g / %g\n\n", r2, r1;

// Inner product.
nops = 2*numberof(x);

write, format="%30s: ", "sum(x*y)";
k = repeat; vops_warmup; while (--k >= 0) r1 = sum(x*y);
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="%30s: ", "vops_inner(x, y)";
k = repeat; vops_warmup; while (--k >= 0) r2 = vops_inner(x, y);
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="Results: %g / %g\n\n", r2, r1;

// Triple inner product.
nops = 3*numberof(x);

write, format="%30s: ", "sum(w*x*y)";
k = repeat; vops_warmup; while (--k >= 0) r1 = sum(x*x*y);
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="%30s: ", "vops_inner(w, x, y)";
k = repeat; vops_warmup; while (--k >= 0) r2 = vops_inner(x, x, y);
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="Results:  %g / %g\n\n", r2, r1;

// Scale.
nops = numberof(x);
alpha = random();

write, format="%30s: ", "alpha*x";
k = repeat; vops_warmup; while (--k >= 0) r1 = alpha*x;
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="%30s: ", "vops_scale(x, alpha)";
k = repeat; vops_warmup; while (--k >= 0) r2 = vops_scale(x, alpha);
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="Results: max(abs(dif)) = %g\n\n", max(abs(r2 - r1));

// Update.
nops = 2*numberof(x);
alpha = random();

y1 = y;
write, format="%30s: ", "y += alpha*x";
k = repeat; vops_warmup; while (--k >= 0) y1 += alpha*x;
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

y2 = y;
write, format="%30s: ", "vops_update, y, alpha, x";
k = repeat; vops_warmup; while (--k >= 0) vops_update, y2, alpha, x;
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

y1 = y; y1 += alpha*x;
y2 = y; vops_update, y2, alpha, x;
write, format="Results: max(abs(dif)) = %g\n\n", max(abs(y2 - y1));
