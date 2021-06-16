dims = 10000;
w = random(dims);
x = random(dims);
y = random(dims);
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

r3 = [];
multipliers = [0,1,-1,2];
for (i = 1; i <= numberof(multipliers); ++i) {
    alpha = multipliers(i);
    for (j = 1; j <= numberof(multipliers); ++j) {
        beta = multipliers(j);
        r1 = alpha*x + beta*y;
        r2 = vops_combine(alpha, x, beta, y);
        vops_combine, r3, alpha, x, beta, y;
        write, format="vops_combine(%2d, x, %2d, y): max(|dif|) = %.1e / %.1e\n",
            alpha, beta, max(abs(r2 - r1)), max(abs(r3 - r1));
    }
}

r4 = r3; r5 = []; eq_nocopy, r5, r4;
vops_combine, r5, alpha, x, beta, y;
if (&r5 != &r4) error, "unexpected re-define";

r4 = array(float, dims); r5 = []; eq_nocopy, r5, r4;
vops_combine, r5, alpha, x, beta, y;
if (&r5 == &r4) error, "unexpected re-use";

r4 = array(int, dims); r5 = []; eq_nocopy, r5, r4;
vops_combine, r5, alpha, x, beta, y;
if (&r5 == &r4) error, "unexpected re-use";

r4 = 1.0; r5 = []; eq_nocopy, r5, r4;
vops_combine, r5, alpha, x, beta, y;
if (&r5 == &r4) error, "unexpected re-use";


write, format="Test %s:\n", "empty";
k = repeat; vops_warmup; while (--k >= 0) r0 = 0.0; vops_toc, repeat;
write, "";

// L1-norm.
nops = 2*numberof(x);

write, format="%35s ->", "sum(abs(x))";
k = repeat; vops_warmup; while (--k >= 0) r1 = sum(abs(x));
write, format="%14.6g / %7.3f Gflops\n", r1, vops_flops(nops*repeat)/1e9;

write, format="%35s ->", "vops_norm1(x)";
k = repeat; vops_warmup; while (--k >= 0) r2 = vops_norm1(x);
write, format="%14.6g / %7.3f Gflops\n\n", r2, vops_flops(nops*repeat)/1e9;

// L2-norm.
nops = 2*numberof(x);

write, format="%35s ->", "sqrt(sum(x*x))";
k = repeat; vops_warmup; while (--k >= 0) r1 = sqrt(sum(x*x));
write, format="%14.6g / %7.3f Gflops\n", r1, vops_flops(nops*repeat)/1e9;

write, format="%35s ->", "vops_norm2(x)";
k = repeat; vops_warmup; while (--k >= 0) r2 = vops_norm2(x);
write, format="%14.6g / %7.3f Gflops\n\n", r2, vops_flops(nops*repeat)/1e9;

// Infinite-norm.
nops = 2*numberof(x);

write, format="%35s ->", "max(abs(x))";
k = repeat; vops_warmup; while (--k >= 0) r1 = max(abs(x));
write, format="%14.6g / %7.3f Gflops\n", r1, vops_flops(nops*repeat)/1e9;

write, format="%35s ->", "vops_norminf(x)";
k = repeat; vops_warmup; while (--k >= 0) r2 = vops_norminf(x);
write, format="%14.6g / %7.3f Gflops\n\n", r2, vops_flops(nops*repeat)/1e9;

// Inner product.
nops = 2*numberof(x);

write, format="%35s ->", "sum(x*y)";
k = repeat; vops_warmup; while (--k >= 0) r1 = sum(x*y);
write, format="%14.6g / %7.3f Gflops\n", r1, vops_flops(nops*repeat)/1e9;

write, format="%35s ->", "vops_inner(x, y)";
k = repeat; vops_warmup; while (--k >= 0) r2 = vops_inner(x, y);
write, format="%14.6g / %7.3f Gflops\n\n", r2, vops_flops(nops*repeat)/1e9;

// Triple inner product.
nops = 3*numberof(x);

write, format="%35s ->", "sum(w*x*y)";
k = repeat; vops_warmup; while (--k >= 0) r1 = sum(x*x*y);
write, format="%14.6g / %7.3f Gflops\n", r1, vops_flops(nops*repeat)/1e9;

write, format="%35s ->", "vops_inner(w, x, y)";
k = repeat; vops_warmup; while (--k >= 0) r2 = vops_inner(x, x, y);
write, format="%14.6g / %7.3f Gflops\n\n", r2, vops_flops(nops*repeat)/1e9;

// Scale.
nops = numberof(x);
alpha = random();

write, format="%35s: ", "alpha*x";
k = repeat; vops_warmup; while (--k >= 0) r1 = alpha*x;
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="%35s: ", "vops_scale(x, alpha)";
k = repeat; vops_warmup; while (--k >= 0) r2 = vops_scale(x, alpha);
write, format="%7.3f Gflops / max(|dif|) = %g\n\n",
    vops_flops(nops*repeat)/1e9, max(abs(r2 - r1));

// Update.
nops = 2*numberof(x);
alpha = random();

y1 = y;
write, format="%35s: ", "y += alpha*x";
k = repeat; vops_warmup; while (--k >= 0) y1 += alpha*x;
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

y2 = y;
write, format="%35s: ", "vops_update, y, alpha, x";
k = repeat; vops_warmup; while (--k >= 0) vops_update, y2, alpha, x;
r1 = y; r1 += alpha*x;
r2 = y; vops_update, r2, alpha, x;
write, format="%7.3f Gflops / max(|dif|) = %g\n\n",
    vops_flops(nops*repeat)/1e9, max(abs(r2 - r1));

// Combine.
nops = 3*numberof(x);
alpha = random();
beta = random();

write, format="%35s: ", "z = alpha*x + beta*y";
k = repeat; vops_warmup; while (--k >= 0) z1 = alpha*x + beta*y;
write, format="%7.3f Gflops\n", vops_flops(nops*repeat)/1e9;

write, format="%35s: ", "vops_combine(alpha, x, beta, y)";
k = repeat; vops_warmup; while (--k >= 0) z2 = vops_combine(alpha, x, beta, y);
write, format="%7.3f Gflops / max(|dif|) = %g\n",
    vops_flops(nops*repeat)/1e9, max(abs(z2 - z1));

z3 = r2*0;
write, format="%35s: ", "vops_combine, z, alpha, x, beta, y";
k = repeat; vops_warmup; while (--k >= 0) vops_combine, z3, alpha, x, beta, y;
write, format="%7.3f Gflops / max(|dif|) = %g\n\n",
    vops_flops(nops*repeat)/1e9, max(abs(z3 - z1));
