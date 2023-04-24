#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

typedef struct{
    float width, height, near, far;
} Frustum;

typedef struct{
    Frustum * f;
    float pos[3];
    float rot[3];
    float view_m[4*4];
    float clip_m[4*4];
    float view_nrm_m[4*4];
} Camera;

void Camera_init(Camera * c, Frustum * f);

void Camera_update(Camera * c);

void Camera_view_tran(Camera * c, float * viewv, float * worldv);

void Camera_view_tran_n(Camera * c, float * viewn, float * worldn);

void Camera_clip_tran(Camera * c, float * clipv, float * viewv);

char Camera_cull(Camera * c, float * clipv);

void Camera_move(Camera * c, float x, float y, float z);

void Camera_rotate(Camera * c, float x, float y, float z);

void Frustum_init(Frustum * f, float width, float height, float near, float far);

#endif