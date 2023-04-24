#include "scene.h"

#include <stdlib.h>
#include <string.h>

#include "3D/3D.h"
#include "game.h"
#include "npc.h"

int SceneObject_road_update(SceneObject * so, Object **obs, Game * g, float scale){
    float *widths, *positions;
    int branches;

    float seg_len = 2 * g->player_range / SCENE_ROAD_RES;
    float offset = -g->player_range;
    int obc = 0;

    *obs = malloc(SCENE_ROAD_RES * g->r->max_branch * sizeof(Object));

    widths = malloc(g->r->max_branch * sizeof(float));
    positions = malloc(g->r->max_branch * sizeof(float));

    for(int i = 0; i < SCENE_ROAD_RES; i++){
        branches = Road_state(g->r, offset + g->p->e.y, positions, widths);
        // generate road from rectangles
        for(int j = 0; j < branches; j++){
            Model * mo = malloc(sizeof(Model));
            Model_rectangle(mo, widths[j] * scale, seg_len * scale / SCENE_ROAD_S);
            Object_init(*obs + obc, mo, so->mcs);
            Object_rotate(*obs + obc, -SCENE_PI/2, 0, 0);
            Object_move(*obs + obc, positions[j] * scale, 0, offset * scale);
            obc++;
        }
        offset += seg_len;
    }

    free(widths);
    free(positions);
    return obc;
}

void SceneObject_cleanup(SceneObject * so, Object * obs, int n){
    if(so->mos == NULL){
        for(int i = 0; i < n; i++){
            Model_destroy(obs[i].mo);
            free(obs[i].mo);
        }
    }
    free(obs);
}

int SceneObject_npc_update(SceneObject * so, Object **obs, Game * g, float scale){
    int obn = g->nm->n;
    *obs = malloc(obn * sizeof(Object));

    for(int i = 0; i < g->nm->n; i++){
        switch(g->nm->npcs[i].af){
            case NPC_FRIENDLY:
                Object_init(*obs + i, so->mos, so->mcs);
                break;

            case NPC_ENEMY:
                Object_init(*obs + i, so->mos, so->mcs + 1);
                break;
        }

        float t = g->nm->npcs[i].dtime;
        float ix = scale * (g->nm->npcs[i].e.x);
        float iz = scale * (g->nm->npcs[i].e.y - g->p->e.y);
        int dir = (g->nm->npcs[i].rpos > 0) ? 1 : -1;

        if(g->nm->npcs[i].dead){
            // death animation
            Object_move(*obs + i,
                ix + dir * SCENE_NPC_F_XSPEED * t,
                SCENE_NPC_F_YACC * t * t + SCENE_NPC_F_YSPEED * t,
                iz + dir * SCENE_NPC_F_ZSPEED * t);
            Object_rotate(*obs + i, t * 2, t * 3, t * 4);

        }else{
            Object_move(*obs + i,
            scale * (g->nm->npcs[i].e.x),
            0,
            scale * (g->nm->npcs[i].e.y - g->p->e.y));
        }
    }
    return obn;
}

int SceneObject_prop_update(SceneObject * so, Object ** obs, Game * g, float scale){
    int obn = g->pm->n;
    *obs = malloc(obn * sizeof(Object));

    for(int i = 0; i < obn; i++){
        switch(g->pm->props[i].t){
            case PROP_TREE:
                Object_init(*obs + i, so->mos, so->mcs);
                Object_scale(*obs + i, SCENE_PROP_PALM_SCALE);
                break;
        }
            
        Object_move(*obs + i,
            scale * (g->pm->props[i].e.x),
            0,
            scale * (g->pm->props[i].e.y - g->p->e.y));
    }
    return obn;
}

int SceneObject_player_update(SceneObject * so, Object ** obs, Game * g, float scale){
    int obn = 1;
    float t = g->respawn_timer;
    float ix = scale * g->p->e.x;
    float iz = 0;

    *obs = malloc(obn * sizeof(Object));
    Object_init(*obs, so->mos, so->mcs);

    if(g->p->dead){
        // death animation
        Object_move(*obs,
                ix + SCENE_NPC_F_XSPEED * t,
                SCENE_NPC_F_YACC * t * t + SCENE_NPC_F_YSPEED * t,
                iz + SCENE_NPC_F_ZSPEED * t);
        Object_rotate(*obs, t * 2, t * 3, t * 4);
        return obn;
    }

    Object_move(*obs, ix, 0, iz);
    return obn;
}

void Scene_render(Scene * s){
    Object * obs;
    int obn = 0;

    for(int i = 0; i < s->n; i++){
        switch (s->sos[i].t) {
            case SCENEOBJECT_PLAYER:
                obn = SceneObject_player_update(s->sos + i, &obs, s->g, s->scale);
                break;

            case SCENEOBJECT_ROAD:
                obn = SceneObject_road_update(s->sos + i, &obs, s->g, s->scale);
                break;

            case SCENEOBJECT_NPC:
                obn = SceneObject_npc_update(s->sos + i, &obs, s->g, s->scale);
                break;

            case SCENEOBJECT_PROP:
                obn = SceneObject_prop_update(s->sos + i, &obs, s->g, s->scale);
                break;
        }

        Renderer_render(s->r, obs, obn);
        SceneObject_cleanup(s->sos + i, obs, obn);
    }

}

void SceneObject_init(SceneObject * so, SceneObjectType t, MaterialContainer * mcs, Model * mos){
    so->t = t;
    so->mcs = mcs;
    so->mos = mos;
}

void Scene_init(Scene * s, Game * g, Renderer * r, SceneObject * sos, int n, float scale){
    s->g = g;
    s->r = r;
    s->sos = sos;
    s->n = n;
    s->scale = scale;
}
