#ifndef ROAD_H_INCLUDED
#define ROAD_H_INCLUDED

#include "entity.h"

typedef struct{
    int max_branch;
} Road;

int Road_state(Road * r, float dst, float * positions, float * widths);

int Road_get_branch(Road * r, Entity * e);

void Road_init(Road * r, int max_branch);

float Road_dst_to_nearest(Road * r, Entity * e);

#endif