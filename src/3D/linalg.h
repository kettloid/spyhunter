#ifndef LINALG_H_INCLUDED
#define LINALG_H_INCLUDED

#include "camera.h"

#include <math.h>
#include <string.h>

static inline void LA_mmul(float * m, float * a, float * b){
    int i, j, k;

    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            m[i + 4 * j] = 0;
            for(k = 0; k < 4; k++){
                m[i + 4 * j] += a[k + 4 * j] * b[i + 4 * k];
            }
        }
    }
}

static inline void LA_mrotx(float * m, float phi){
    float a[4][4] = { 
        {1., 0., 0., 0.},
        {0., cos(phi), -sin(phi), 0.},
        {0., sin(phi), cos(phi), 0.},
        {0., 0., 0., 1.} 
    };

    memcpy(m, a, 16*sizeof(float));
}

static inline void LA_mroty(float * m, float phi){
    float a[4][4] = { 
        {cos(phi), 0., sin(phi), 0.},
        {0., 1., 0., 0.},
        {-sin(phi), 0., cos(phi), 0.},
        {0., 0., 0., 1.} 
    };

    memcpy(m, a, 16*sizeof(float));
}

static inline void LA_mrotz(float * m, float phi){
    float a[4][4] = { 
        {cos(phi), -sin(phi), 0., 0.},
        {sin(phi), cos(phi), 0., 0.},
        {0., 0., 1., 0.},
        {0., 0., 0., 1.} 
    };

    memcpy(m, a, 16*sizeof(float));
}

static inline void LA_mrotxyz(float * m, float * r){
    float mx[16], my[16], mz[16];
    float t1[16];
    LA_mrotx(mx, r[0]);
    LA_mroty(my, r[1]);
    LA_mrotz(mz, r[2]);
    LA_mmul(t1, my, mz);
    LA_mmul(m, mx, t1);
}

static inline void LA_mproj(float * m, Frustum * f){
    float a[4][4] = {
        {2 * f->near / f->width, 0, 0, 0},
        {0., 2 * f->near / f->height, 0., 0.},
        {0., 0., -(f->far + f->near)/(f->far - f->near), 2 * f->far * f->near / (f->near - f->far)},
        {0., 0., -1., 0.}
    };

    memcpy(m, a, 4*4*sizeof(float));
}

static inline void LA_mtran(float * m, float * d){
    float a[4][4] = {
        {1., 0., 0., d[0]},
        {0., 1., 0., d[1]},
        {0., 0., 1., d[2]},
        {0., 0., 0., 1.}
    };

    memcpy(m, a, 4*4*sizeof(float));
}

static inline void LA_mscale(float * m, float s){
    float a[4][4] = {
        {s, 0, 0, 0},
        {0, s, 0, 0},
        {0, 0, s, 0},
        {0, 0, 0, 1}
    };

    memcpy(m, a, 4*4*sizeof(float));
}

static inline void LA_swap(float * a, float * b){
    float c = *a;
    *a = *b;
    *b = c;
}

static inline void LA_mtranspose(float * m){
    for(int x = 1; x < 4; x++){
        for(int y = 0; y < x; y++){
            LA_swap(m + x + y * 4, m + y + x * 4);
        }
    }
}

static inline void LA_vscale(float * sv, float * v, float s, int n){
    int i;

    for(i = 0; i < n; i++)
        sv[i] = v[i] * s;

}

static inline void LA_vadd(float * v, float * a, float * b, int n){
    int i;

    for(i = 0; i < n; i++)
        v[i] = a[i] + b[i];
}

static inline void LA_vsub(float * v, float * a, float * b, int n){
    int i;

    for(i = 0; i < n; i++)
        v[i] = a[i] - b[i];
}

static inline float LA_vdot(float * a, float * b, int n){
    int i;
    float s = 0;

    for(i = 0; i < n; i++)
        s += a[i] * b[i];

    return s;
}

static inline void LA_vcpy(float * dest, float * src, int n){
    memcpy(dest, src, n * sizeof(float));
}

static inline void LA_mvmul(float * v, float * m, float * a){
    int i, k;

    for(i = 0; i < 4; i++){
        v[i] = 0;
        for(k = 0; k<4; k++){
            v[i] += m[k + 4 * i] * a[k];
        }
    }
}

static inline float LA_vmag(float * v, int n){
    return sqrt(LA_vdot(v, v, n));
}

static inline void LA_vnormal(float * v, int n){
    LA_vscale(v, v, 1.0/LA_vmag(v, n), n);
}

static inline void LA_vcross(float * v, float * a, float * b){
    v[0] = a[1]*b[2] - a[2]*b[1];
    v[1] = a[2]*b[0] - a[0]*b[2];
    v[2] = a[0]*b[1] - a[1]*b[0];
}

static inline void LA_vreflect(float * v, float * n){
    float t[3];
    LA_vcpy(t, n, 3);
    LA_vscale(t, t, 2 * LA_vdot(v, t, 3), 3);
    LA_vsub(v, t, v, 3);
}

static inline void LA_vtobarycentric(float * l, float * p, float * v1, float * v2, float * v3){
    float dT = (v2[1] - v3[1]) * (v1[0] - v3[0]) + (v3[0] - v2[0]) * (v1[1] - v3[1]);
    l[0] = ((v2[1] - v3[1]) * (p[0] - v3[0]) + (v3[0] - v2[0]) * (p[1] - v3[1])) / dT;
    l[1] = ((v3[1] - v1[1]) * (p[0] - v3[0]) + (v1[0] - v3[0]) * (p[1] - v3[1])) / dT;
    l[2] = 1 - l[1] - l[0];
}

static inline void LA_vfrombarycentric(float * l, float * p, float * v1, float * v2, float * v3){
    float invz = l[0] / v1[2] + l[1] / v2[2] + l[2] / v3[2];
    float z = 1 / invz;
    float x = (l[0] * v1[0] / v1[2] + l[1] * v2[0] / v2[2] + l[2] * v3[0] / v3[2]) * z;
    float y = (l[0] * v1[1] / v1[2] + l[1] * v2[1] / v2[2] + l[2] * v3[1] / v3[2]) * z;
    p[0] = x;
    p[1] = y;
    p[2] = z;

}

static inline char LA_vbaryinside(float * l){
    return l[0] >= 0 && l[1] >= 0 && l[2] >= 0;
}

static inline void LA_vlincomb(float * v, float * coefs, float vs[][4], int n){
    int i;
    for(i = 0; i < n; i++)
        v[i] = coefs[0] * vs[0][i] + coefs[1] * vs[1][i] + coefs[2] * vs[2][i];
    
}

#endif