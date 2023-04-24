#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include "camera.h"
#include "object.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN3(a, b, c) MIN(MIN(a, b), c)
#define MAX3(a, b, c) MAX(MAX(a, b), c)

#define RENDERER_INIT_ZBUFFER (-70000.0)

typedef struct{
    float pos[4]; //world pos
    float viewPos[4];
    float ambientCoef;
} Light;

typedef struct{
    Camera * c;
    Light * l;
    float * zbuffer;
    unsigned char * screen;
    int width, height, ncolor;
} Renderer;

void Renderer_light_intensity(Renderer * r, Material * m, float v[3], float n[3], float * color);

void Renderer_init(Renderer * r, Camera * c, Light * l, int width, int height, int ncolor);

void Renderer_fromscreencord(Renderer * r, float * v, int * s);

void Renderer_toscreencord(Renderer * r, int * s, float * v);

void Renderer_boundingbox(Renderer * r, int bbox[2][2], int vs[3][2]);

void Renderer_clear(Renderer * r, unsigned char  * color);

char Renderer_orientation_cull(Renderer * r, float vertices[3][4]);

void Renderer_render(Renderer * r, Object * obs, int oc);

void Renderer_destroy(Renderer * r);

void Light_init(Light * l, float x, float y, float z, float ambientCoef);

#endif