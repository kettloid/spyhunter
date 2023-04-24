#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

typedef struct{
    float x, y;
    float w, h;
} Entity;

float Entity_vdistance(Entity * a, Entity * b);

float Entity_hdistance(Entity * a, Entity * b);

char Entity_collide(Entity * a, Entity * b);

void Entity_init(Entity * e, float x, float y, float w, float h);

#endif