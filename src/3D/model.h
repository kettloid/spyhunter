#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include "material.h"

#define MODEL_MAX_VERTICES 70000
#define MODEL_INITIAL_TS 100
#define MODEL_NAME_LENGTH 100
#define MODEL_LINE_LENGTH 300

typedef struct{
    float vertices[3][4];
    float normals[3][4];
    char smooth;
    int material;
} Triangle;

typedef struct{
    Triangle * ts;
    int tn;
} Model;

void Model_dimensions(Model * m, float size[3]);

void Model_rectangle(Model * m, float a, float b);

void Model_fromfile(Model * m, MaterialContainer * mc, char * filename);

void Model_destroy(Model * m);

#endif