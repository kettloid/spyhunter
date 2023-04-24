#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include "material.h"
#include "model.h"

typedef struct{
    Model * mo;
    MaterialContainer * mc;
    float pos[3];
    float rot[3];
    float world_m[4*4];
    float world_nrm_m[4*4];
    float scale;
} Object;

void Object_init(Object * ob, Model * mo, MaterialContainer * mc);

void Object_move(Object * ob, float x, float y, float z);

void Object_rotate(Object * ob, float x, float y, float z);

void Object_scale(Object * ob, float scale);

void Object_world_tran(Object * ob, float * worldv, float * modelv);

void Object_world_tran_n(Object * ob, float * worldn, float * modeln);

void Object_update(Object * ob);


#endif