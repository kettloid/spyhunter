#include "material.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void MaterialContainer_load(MaterialContainer * mc, char * filename){
    char line[MATERIAL_LINE_LENGTH + 1];
    char name[MATERIAL_NAME_LENGTH + 1];
    FILE * f = fopen(filename, "r");
    int i = -1;

    mc->n = 0;
    mc->ms = NULL;

    while(fgets(line, MATERIAL_LINE_LENGTH, f) != NULL){
        if(line[0] == '#' || isspace(line[0]))
            continue;

        if(sscanf(line, "newmtl %s", name) == 1){
            i++;
            mc->n++;
            mc->ms = realloc(mc->ms, mc->n * sizeof(Material));
            strcpy(mc->ms[i].name, name);
            continue;
        }

        if(sscanf(line, "Ns %f", &mc->ms[i].Ns) == 1)
            continue;
        
        if(sscanf(line, "Ka %f %f %f", mc->ms[i].Ka, mc->ms[i].Ka + 1, mc->ms[i].Ka + 2) == 3)
            continue;

        if(sscanf(line, "Kd %f %f %f", mc->ms[i].Kd, mc->ms[i].Kd + 1, mc->ms[i].Kd + 2) == 3)
            continue;

        if(sscanf(line, "Ks %f %f %f", mc->ms[i].Ks, mc->ms[i].Ks + 1, mc->ms[i].Ks + 2) == 3)
            continue;

        if(sscanf(line, "Ke %f %f %f", mc->ms[i].Ke, mc->ms[i].Ke + 1, mc->ms[i].Ke + 2) == 3)
            continue;
        
    }

    fclose(f);
}

void MaterialContainer_destroy(MaterialContainer * mc){
    free(mc->ms);
}