#include "model.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "material.h"
#include "linalg.h"

void Model_rectangle(Model * m, float a, float b){
    float n[4] = {0, 0, 1, 1};
    float off1[4] = {-a, b, 0, 0};
    float off2[4] = {a, b, 0, 0};

    m->tn = 2;
    m->ts = malloc(2 * sizeof(Triangle));

    for(int j = 0; j < 2; j++){
        for(int i = 0; i < 3; i++){
            memset(m->ts[j].vertices[i], 0, 3 * sizeof(float));
            m->ts[j].vertices[i][3] = 1;
            memcpy(m->ts[j].normals[i], n, 4 * sizeof(float));
        }
    }

    LA_vadd(m->ts[0].vertices[2], m->ts[0].vertices[2], off1, 3);
    LA_vadd(m->ts[0].vertices[1], m->ts[0].vertices[1], off2, 3);
    LA_vsub(m->ts[0].vertices[0], m->ts[0].vertices[0], off1, 3);

    LA_vadd(m->ts[1].vertices[2], m->ts[1].vertices[2], off1, 3);
    LA_vsub(m->ts[1].vertices[1], m->ts[1].vertices[1], off1, 3);
    LA_vsub(m->ts[1].vertices[0], m->ts[1].vertices[0], off2, 3);

    m->ts[0].material = 0;
    m->ts[1].material = 0;
}

void Model_destroy(Model * m){
    free(m->ts);
    m->ts = NULL;
    m->tn = 0;
}

static inline int find_material(Material * ms, int mn, char * name){
    if(ms == NULL)
        return -2;

    for(int i = 0; i < mn; i++){
        if(strcmp(name, ms[i].name) == 0)
            return i;
    }

    return -1;
}

void Model_dimensions(Model * m, float size[3]){
    for(int i = 0; i < 3; i++)
        size[i] = 0;

    float maxs[3] = {0};
    float mins[3] = {0};

    for(int i = 0; i < m->tn; i++){
        for(int j = 0; j < 3; j++){
            for(int k = 0; k < 3; k++){
                maxs[k] = fmax(maxs[k], m->ts[i].vertices[j][k]);
                mins[k] = fmin(mins[k], m->ts[i].vertices[j][k]);
            }
        }
    }

    for(int i = 0; i < 3; i++){
        size[i] = maxs[i] - mins[i];
    }
}

void Model_fromfile(Model * m, MaterialContainer * mc, char * filename){
    FILE * f = fopen(filename, "r");
    int vssize = MODEL_MAX_VERTICES, tn = MODEL_INITIAL_TS;
    float * vsbuff = malloc(3 * vssize * sizeof(float));
    float * nsbuff = malloc(3 * vssize * sizeof(float));
    Triangle * ts = malloc(tn * sizeof(Triangle));
    
    int fvbuff[3], fnbuff[3];
    char name[MODEL_NAME_LENGTH + 1], matname[MATERIAL_NAME_LENGTH + 1];
    char line[MODEL_LINE_LENGTH + 1];
    int vsc = 0, nsc = 0, fcs = 0;
    int material = 0, smooth = 0;

    while(fgets(line, MODEL_LINE_LENGTH, f) != NULL){
        if(line[0] == '#' || isspace(line[0]))
            continue;

        if(strstr(line, "mtllib") != NULL)
            continue;

        if(sscanf(line, "o %s", name) == 1)
            continue;

        if(sscanf(line, "v %f %f %f", vsbuff + vsc * 3, vsbuff + vsc * 3 + 1, vsbuff + vsc * 3 + 2) == 3){
            vsc++;
            continue;
        }

        if(sscanf(line, "vn %f %f %f", nsbuff + nsc * 3, nsbuff + nsc * 3 + 1, nsbuff + nsc * 3 + 2) == 3){
            nsc++;
            continue;
        }

        if(sscanf(line, "f %d//%d %d//%d %d//%d", fvbuff, fnbuff, fvbuff + 1, fnbuff + 1,
                    fvbuff + 2, fnbuff + 2) == 6){
            
            for(int i = 0; i < 3; i++){
                memcpy(ts[fcs].vertices[i], vsbuff + (fvbuff[i] - 1) * 3, 3 * sizeof(float));
                memcpy(ts[fcs].normals[i], nsbuff + (fnbuff[i] - 1) * 3, 3 * sizeof(float));
                ts[fcs].vertices[i][3] = 1;
                ts[fcs].normals[i][3] = 1;
                ts[fcs].material = material;
                ts[fcs].smooth = smooth;
            }
            fcs++;
            if(fcs >= tn){
                tn *= 2;
                ts = realloc(ts, tn * sizeof(Triangle));
            }
            continue;
        }

        if(sscanf(line, "s %d", &smooth) == 1)
            continue;

        if(sscanf(line, "usemtl %s", matname) == 1){
            material = find_material(mc->ms, mc->n, matname);
            if(material < 0)
                material = 0;
            continue;
        }
        
    }

    free(vsbuff);
    free(nsbuff);

    m->ts = ts;
    m->tn = fcs;
}