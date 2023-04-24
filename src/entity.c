#include "entity.h"

#include <math.h>

float Entity_vdistance(Entity * a, Entity * b){
    return fabs(a->y - b->y);
}

float Entity_hdistance(Entity * a, Entity * b){
    return fabs(a->x - b->x);
}

char Entity_collide(Entity * a, Entity * b){
    return Entity_hdistance(a, b) < a->w + b->w
        && Entity_vdistance(a, b) < a->h + b->h;
}

void Entity_init(Entity * e, float x, float y, float w, float h){
    e->x = x;
    e->y = y;
    e->w = w;
    e->h = h;
}