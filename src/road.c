#include "road.h"

#include <math.h>
#include <stdlib.h>

#include "entity.h"

int Road_state(Road * r, float dst, float * positions, float * widths){
    const float pi = acos(-1);
    const float basewidth = 4;
    const float amp = 3;
    const float freq = 2 * pi * 0.016;
    const float offset = -10;
    dst += offset;
    
    if(sin(dst * freq) > 0){
        if(positions){
            positions[0] = basewidth/2 + amp * sin(dst * freq);
            positions[1] = -basewidth/2 - amp * sin(dst * freq);
        }
        if(widths){
            widths[0] = basewidth / 2;
            widths[1] = basewidth / 2;
        }
        return 2;
    }
    if(positions)
        positions[0] = 0;
    if(widths)
        widths[0] = basewidth + sin(dst * freq);
    return 1;

}

int Road_get_branch(Road * r, Entity * e){
    float *positions, *widths;
    int branches;
    int branch = -1;
    positions = malloc(r->max_branch * sizeof(float));
    widths = malloc(r->max_branch * sizeof(float));

    branches = Road_state(r, e->y, positions, widths);

    for(int i = 0; i < branches; i++){
        if(fabs(e->x - positions[i]) < widths[i] - e->w){
            branch = i;
            break;
        }
    }

    free(positions);
    free(widths);

    return branch;
}

float Road_dst_to_nearest(Road * r, Entity * e){
    float min_dst = INFINITY;
    float *positions, *widths;
    int branches;

    positions = malloc(r->max_branch * sizeof(float));
    widths = malloc(r->max_branch * sizeof(float));

    branches = Road_state(r, e->y, positions, widths);

    for(int i = 0; i < branches; i++){
        float dst = fabs(e->x - positions[i]) + e->w - widths[i];
        min_dst = fmin(min_dst, dst);
    }

    free(positions);
    free(widths);

    return min_dst;
}

void Road_init(Road * r, int max_branch){
    r->max_branch = max_branch;
}