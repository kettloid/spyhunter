#include "camera.h"

#include <stdlib.h>

#include "linalg.h"

void Camera_init(Camera * c, Frustum * f) {
    c->f = f;
    memset(c->pos, 0, 4 * sizeof(float));
    memset(c->rot, 0, 4 * sizeof(float));
}

void Camera_update(Camera * c){
    float tpos[3], trot[3];
    float mtran[16];
    LA_vscale(tpos, c->pos, -1, 3);
    LA_vscale(trot, c->rot, -1, 3);
    LA_mrotxyz(c->view_nrm_m, trot);
    LA_mtran(mtran, tpos);
    LA_mproj(c->clip_m, c->f);
    LA_mmul(c->view_m, c->view_nrm_m, mtran);
}

void Camera_view_tran(Camera * c, float * viewv, float * worldv){
    LA_mvmul(viewv, c->view_m, worldv);
}

void Camera_view_tran_n(Camera * c, float * viewn, float * worldn){
    LA_mvmul(viewn, c->view_nrm_m, worldn);
}

void Camera_clip_tran(Camera * c, float * clipv, float * viewv){
    LA_mvmul(clipv, c->clip_m, viewv);
    LA_vscale(clipv, clipv, 1 / clipv[3], 4);
}

char Camera_cull(Camera * c, float * v){
    return fabs(v[0]) <= 1 && fabs(v[1]) <= 1 && fabs(v[2]) <= 1;
}

void Camera_move(Camera * c, float x, float y, float z){
    c->pos[0] = x;
    c->pos[1] = y;
    c->pos[2] = z;
}

void Camera_rotate(Camera * c, float x, float y, float z){
    c->rot[0] = x;
    c->rot[1] = y;
    c->rot[2] = z;
}

void Frustum_init(Frustum * f, float width, float height, float near, float far){
    f->width = width;
    f->height = height;
    f->near = near;
    f->far = far;
}