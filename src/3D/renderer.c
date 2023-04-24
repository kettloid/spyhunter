#include "renderer.h"

#include <math.h>
#include <stdlib.h>

#include "linalg.h"
#include "object.h"
#include "camera.h"

void Light_init(Light * l, float x, float y, float z, float ambientCoef){
    l->pos[0] = x;
    l->pos[1] = y;
    l->pos[2] = z;
    l->pos[3] = 0;
    l->ambientCoef = ambientCoef;
}

void Renderer_light_intensity(Renderer * r, Material * m, float v[3], float n[3], float * color){
    float lightDisplacement[3];
    float cameraDisplacement[3];
    float reflection[3];

    LA_vcpy(lightDisplacement, r->l->viewPos, 3);
    LA_vnormal(lightDisplacement, 3);

    float diffuseCoef = LA_vdot(lightDisplacement, n, 3);
    diffuseCoef = (diffuseCoef > 0) ? diffuseCoef : 0;

    //specular
    LA_vscale(cameraDisplacement, v, -1, 3);
    LA_vcpy(reflection, lightDisplacement, 3);
    LA_vreflect(reflection, n);
    LA_vnormal(cameraDisplacement, 3);

    float specularCoef = LA_vdot(reflection, cameraDisplacement, 3);
    specularCoef = (specularCoef > 0) ? specularCoef : 0;
    specularCoef = pow(specularCoef, m->Ns);

    for(int i = 0; i < r->ncolor; i++){
        color[i] = (1 - r->l->ambientCoef) * m->Kd[i] * diffuseCoef + r->l->ambientCoef * m->Ka[i] * m->Kd[i] + specularCoef * m->Ks[i] + m->Ke[i];
        color[i] = (color[i] < 1) ? color[i] : 1;
    }
}

void Renderer_init(Renderer * r, Camera * c, Light * l, int width, int height, int ncolor){
    r->c = c;
    r->height = height;
    r->width = width;
    r->ncolor = ncolor;
    r->l = l;
    r->zbuffer = malloc(width * height * sizeof(float));
}

void Renderer_destroy(Renderer * r){
    free(r->zbuffer);
}

void Renderer_fromscreencord(Renderer * r, float * v, int * s){
    v[0] = 2 * (s[0] / (float)(r->width)) - 1;
    v[1] = 2 * (s[1] / (float)(r->height)) - 1;
}

void Renderer_toscreencord(Renderer * r, int * s, float * v){
    s[0] = MIN(MAX(0, (v[0] + 1) / 2.0 * r->width), r->width - 1);
    s[1] = MIN(MAX(0, (v[1] + 1) / 2.0 * r->height), r->height - 1);
}

void Renderer_boundingbox(Renderer * r, int bbox[2][2], int vs[3][2]){
    bbox[0][0] = MIN3(vs[0][0], vs[1][0], vs[2][0]);
    bbox[0][1] = MIN3(vs[0][1], vs[1][1], vs[2][1]);
    bbox[1][0] = MAX3(vs[0][0], vs[1][0], vs[2][0]);
    bbox[1][1] = MAX3(vs[0][1], vs[1][1], vs[2][1]);
}

void Renderer_clear(Renderer * r, unsigned char * color){
    for(int i = 0; i<r->width * r->height; i++){
        r->zbuffer[i] = RENDERER_INIT_ZBUFFER;
        for(int j = 0; j < 3; j++)
            r->screen[i * r->ncolor + j] = color[j];
    }
}

char Renderer_orientation_cull(Renderer * r, float clipv[3][4]) {
    float t1[3], t2[3], n[3];
    LA_vsub(t1, clipv[1], clipv[0], 3);
    LA_vsub(t2, clipv[2], clipv[0], 3);
    LA_vcross(n, t1, t2);
    return n[2] >= 0;
}

void Renderer_render(Renderer * r, Object * obs, int oc){
    float worldv[3][4], viewv[3][4], clipv[3][4], worldn[3][4], viewn[3][4];
    float irScrv[2], iViewv[4], n[4], color[3], l[3];
    int screenv[3][2], bbox[2][2], iScrv[2];
    char skipIter = 0;

    Camera_update(r->c);
    for(int j = 0; j < oc; j++){
        Object_update(&obs[j]);
        if(obs[j].mo == NULL)
            continue;

        for(int i = 0; i < obs[j].mo->tn; i++){
            skipIter = 1;

            for(int k = 0; k < 3; k++){
                Object_world_tran(&obs[j], worldv[k], obs[j].mo->ts[i].vertices[k]);
                Camera_view_tran(r->c, viewv[k], worldv[k]);
                Camera_clip_tran(r->c, clipv[k], viewv[k]);
                skipIter &= !Camera_cull(r->c, clipv[k]);
            }

            if(skipIter)
                continue;

            if(!Renderer_orientation_cull(r, clipv))
                continue;

            for(int k = 0; k < 3; k++)
                Renderer_toscreencord(r, screenv[k], clipv[k]);

            for(int k = 0; k < 3; k++){
                Object_world_tran_n(&obs[j], worldn[k], obs[j].mo->ts[i].normals[k]);
                Camera_view_tran_n(r->c, viewn[k], worldn[k]);
            }

            Renderer_boundingbox(r, bbox, screenv);
            for(iScrv[0] = bbox[0][0]; iScrv[0] <= bbox[1][0]; (iScrv[0])++){
                for(iScrv[1] = bbox[0][1]; iScrv[1] <= bbox[1][1]; (iScrv[1])++){
                    Renderer_fromscreencord(r, irScrv, iScrv);
                    LA_vtobarycentric(l, irScrv, clipv[0], clipv[1], clipv[2]);
                    
                    if(!LA_vbaryinside(l))
                        continue;

                    LA_vfrombarycentric(l, iViewv, viewv[0], viewv[1], viewv[2]);

                    int pos = iScrv[0] + (r->height - iScrv[1] - 1) * r->width;

                    if(iViewv[2] < r->zbuffer[pos])
                        continue;

                    LA_vlincomb(n, l, viewn, 3);
                    LA_vnormal(n, 3);

                    r->zbuffer[pos] = iViewv[2];
                    Camera_view_tran(r->c, r->l->viewPos, r->l->pos);
                    Renderer_light_intensity(r, obs[j].mc->ms + obs[j].mo->ts[i].material, iViewv, n, color);
                    

                    for(int k = 0; k < 3; k++){
                        r->screen[r->ncolor*pos + k] = color[k] * 255;
                    }
                }
            }
        }
    }
}