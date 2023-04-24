#include "object.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linalg.h"
#include "material.h"
#include "model.h"

void Object_init(Object * ob, Model * mo, MaterialContainer * mc){
    ob->mc = mc;
    ob->mo = mo;

    memset(ob->pos, 0, 3 * sizeof(float));
    memset(ob->rot, 0, 3 * sizeof(float));
    ob->scale = 1;
}

void Object_move(Object * ob, float x, float y, float z){
    ob->pos[0] = x;
    ob->pos[1] = y;
    ob->pos[2] = z;
}

void Object_rotate(Object * ob, float x, float y, float z){
    ob->rot[0] = x;
    ob->rot[1] = y;
    ob->rot[2] = z;
}

void Object_scale(Object * ob, float scale){
    ob->scale = scale;
}

void Object_world_tran(Object * ob, float * worldv, float * modelv){
    LA_mvmul(worldv, ob->world_m, modelv);
}

void Object_world_tran_n(Object * ob, float * worldn, float * modeln){
    LA_mvmul(worldn, ob->world_nrm_m, modeln);
}

void Object_update(Object * ob){
    float mscale[16];
    float mtran[16];
    float t[16];
    LA_mrotxyz(ob->world_nrm_m, ob->rot);
    LA_mscale(mscale, ob->scale);
    LA_mtran(mtran, ob->pos);
    LA_mmul(t, mtran, mscale);
    LA_mmul(ob->world_m, t, ob->world_nrm_m);
}