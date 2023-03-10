#define PJ_LIB__
#include <projects.h>

PROJ_HEAD(robin, "Robinson") "\n\tPCyl., Sph.";

#define V(C,z) (C.c0 + z * (C.c1 + z * (C.c2 + z * C.c3)))
#define DV(C,z) (C.c1 + z * (C.c2 + C.c2 + z * 3. * C.c3))

/*
note: following terms based upon 5 deg. intervals in degrees.

Some background on these coefficients is available at:

http://article.gmane.org/gmane.comp.gis.proj-4.devel/6039
http://trac.osgeo.org/proj/ticket/113
*/

struct COEFS {
    float c0, c1, c2, c3;
};

static const struct COEFS X[] = {
    {1, 2.2199e-17, -7.15515e-05, 3.1103e-06},
    {0.9986, -0.000482243, -2.4897e-05, -1.3309e-06},
    {0.9954, -0.00083103, -4.48605e-05, -9.86701e-07},
    {0.99, -0.00135364, -5.9661e-05, 3.6777e-06},
    {0.9822, -0.00167442, -4.49547e-06, -5.72411e-06},
    {0.973, -0.00214868, -9.03571e-05, 1.8736e-08},
    {0.96, -0.00305085, -9.00761e-05, 1.64917e-06},
    {0.9427, -0.00382792, -6.53386e-05, -2.6154e-06},
    {0.9216, -0.00467746, -0.00010457, 4.81243e-06},
    {0.8962, -0.00536223, -3.23831e-05, -5.43432e-06},
    {0.8679, -0.00609363, -0.000113898, 3.32484e-06},
    {0.835, -0.00698325, -6.40253e-05, 9.34959e-07},
    {0.7986, -0.00755338, -5.00009e-05, 9.35324e-07},
    {0.7597, -0.00798324, -3.5971e-05, -2.27626e-06},
    {0.7186, -0.00851367, -7.01149e-05, -8.6303e-06},
    {0.6732, -0.00986209, -0.000199569, 1.91974e-05},
    {0.6213, -0.010418, 8.83923e-05, 6.24051e-06},
    {0.5722, -0.00906601, 0.000182, 6.24051e-06},
    {0.5322, -0.00677797, 0.000275608, 6.24051e-06}
};

static const struct COEFS Y[] = {
    {-5.20417e-18, 0.0124, 1.21431e-18, -8.45284e-11},
    {0.062, 0.0124, -1.26793e-09, 4.22642e-10},
    {0.124, 0.0124, 5.07171e-09, -1.60604e-09},
    {0.186, 0.0123999, -1.90189e-08, 6.00152e-09},
    {0.248, 0.0124002, 7.10039e-08, -2.24e-08},
    {0.31, 0.0123992, -2.64997e-07, 8.35986e-08},
    {0.372, 0.0124029, 9.88983e-07, -3.11994e-07},
    {0.434, 0.0123893, -3.69093e-06, -4.35621e-07},
    {0.4958, 0.0123198, -1.02252e-05, -3.45523e-07},
    {0.5571, 0.0121916, -1.54081e-05, -5.82288e-07},
    {0.6176, 0.0119938, -2.41424e-05, -5.25327e-07},
    {0.6769, 0.011713, -3.20223e-05, -5.16405e-07},
    {0.7346, 0.0113541, -3.97684e-05, -6.09052e-07},
    {0.7903, 0.0109107, -4.89042e-05, -1.04739e-06},
    {0.8435, 0.0103431, -6.4615e-05, -1.40374e-09},
    {0.8936, 0.00969686, -6.4636e-05, -8.547e-06},
    {0.9394, 0.00840947, -0.000192841, -4.2106e-06},
    {0.9761, 0.00616527, -0.000256, -4.2106e-06},
    {1, 0.00328947, -0.000319159, -4.2106e-06}
};

#define FXC 0.8487
#define FYC 1.3523
#define C1  11.45915590261646417544
#define RC1 0.08726646259971647884
#define NODES   18
#define ONEEPS  1.000001
#define EPS 1e-8


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    int i;
    double dphi;
    (void) P;

    i = floor((dphi = fabs(lp.phi)) * C1);
    if (i >= NODES) i = NODES - 1;
    dphi = RAD_TO_DEG * (dphi - RC1 * i);
    xy.x = V(X[i], dphi) * FXC * lp.lam;
    xy.y = V(Y[i], dphi) * FYC;
    if (lp.phi < 0.) xy.y = -xy.y;

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    int i;
    double t, t1;
    struct COEFS T;

    lp.lam = xy.x / FXC;
    lp.phi = fabs(xy.y / FYC);
    if (lp.phi >= 1.) { /* simple pathologic cases */
        if (lp.phi > ONEEPS) I_ERROR
        else {
            lp.phi = xy.y < 0. ? -M_HALFPI : M_HALFPI;
            lp.lam /= X[NODES].c0;
        }
    } else { /* general problem */
        /* in Y space, reduce to table interval */
        for (i = floor(lp.phi * NODES);;) {
            if (Y[i].c0 > lp.phi) --i;
            else if (Y[i+1].c0 <= lp.phi) ++i;
            else break;
        }
        T = Y[i];
        /* first guess, linear interp */
        t = 5. * (lp.phi - T.c0)/(Y[i+1].c0 - T.c0);
        /* make into root */
        T.c0 -= lp.phi;
        for (;;) { /* Newton-Raphson reduction */
            t -= t1 = V(T,t) / DV(T,t);
            if (fabs(t1) < EPS)
                break;
        }
        lp.phi = (5 * i + t) * DEG_TO_RAD;
        if (xy.y < 0.) lp.phi = -lp.phi;
        lp.lam /= V(X[i], t);
    }
    return lp;
}


static void *freeup_new (PJ *P) {                       /* Destructor */
    if (0==P)
        return 0;

    return pj_dealloc(P);
}


static void freeup (PJ *P) {
    freeup_new (P);
    return;
}


PJ *PROJECTION(robin) {
    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}


#ifndef PJ_SELFTEST
int pj_robin_selftest (void) {return 0;}
#else

int pj_robin_selftest (void) {
    double tolerance_lp = 1e-10;
    double tolerance_xy = 1e-7;

    char s_args[] = {"+proj=robin   +a=6400000    +lat_1=0.5 +lat_2=2"};

    LP fwd_in[] = {
        { 2, 1},
        { 2,-1},
        {-2, 1},
        {-2,-1}
    };

    XY s_fwd_expect[] = {
        { 189588.423282507836,  107318.530350702888},
        { 189588.423282507836, -107318.530350702888},
        {-189588.423282507836,  107318.530350702888},
        {-189588.423282507836, -107318.530350702888},
    };

    XY inv_in[] = {
        { 200, 100},
        { 200,-100},
        {-200, 100},
        {-200,-100}
    };

    LP s_inv_expect[] = {
        { 0.002109689065506131,  0.000931805533547745983},
        { 0.002109689065506131, -0.000931805533547745983},
        {-0.002109689065506131,  0.000931805533547745983},
        {-0.002109689065506131, -0.000931805533547745983},
    };

    return pj_generic_selftest (0, s_args, tolerance_xy, tolerance_lp, 4, 4, fwd_in, 0, s_fwd_expect, inv_in, 0, s_inv_expect);
}


#endif
