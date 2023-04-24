#ifndef MATERIAL_H_INCLUDED
#define MATERIAL_H_INCLUDED

#define MATERIAL_LINE_LENGTH 200
#define MATERIAL_NAME_LENGTH 50

typedef struct{
    float Ns;
    float Ka[3];
    float Kd[3];
    float Ks[3];
    float Ke[3];
    char name[64];
} Material;

typedef struct{
    Material * ms;
    int n;
} MaterialContainer;

void MaterialContainer_load(MaterialContainer * mc, char * filename);

void MaterialContainer_destroy(MaterialContainer * mc);

#endif