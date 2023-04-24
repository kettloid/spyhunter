#include "prop.h"

#include <string.h>
#include <stdlib.h>

#include "entity.h"
#include "road.h"

void Prop_init(Prop * p, PropType t, PropLocation l, Entity * e){
    p->t = t;
    p->l = l;
    memcpy(&p->e, e, sizeof(Entity));
    p->move = 0;
}

char PropManager_attempty_spawn(PropManager * pm, Entity * e){
    Entity t = {0};
    if(pm->spawn_distance > pm->spawn_distance_threshold){
        t.x = (2 * (rand() / (float)RAND_MAX) - 1) * pm->prop_range;
        t.y = pm->spwn_b + pm->p->e.y;
        if(Road_get_branch(pm->r, &t) != -1)
            return 0;

        pm->spawn_distance = 0;

        e->x = t.x;
        e->y = t.y;
        return 1;
    }
    return 0;
}

void PropManager_update(PropManager * pm, float time){
    pm->spawn_distance += pm->p->speed * time;
    for(int i = 0; i < pm->n; i++){
        if(Entity_vdistance(&pm->props[i].e, &pm->p->e) > pm->dspwn_b){
            pm->props[i].move = 1;
        }
    }
}

void PropManager_respawn(PropManager * pm){
    PropType t = rand() % PROP_TYPE_N;
    Entity e = pm->ents[t];
    for(int i = 0; i < pm->n; i++){
        if(pm->props[i].move && PropManager_attempty_spawn(pm, &e)){
            Prop_init(pm->props + i, t, pm->locs[t], &e);
        }
    }
}

void PropManager_init(PropManager * pm, Player * p, Road * r, float spawn_interval, float spawn_dst, float despawn_dst, int n){
    pm->prop_range = spawn_dst;
    pm->spawn_distance = 0;
    pm->spawn_distance_threshold = spawn_interval;
    pm->spwn_b = spawn_dst;
    pm->dspwn_b = despawn_dst;
    pm->n = n;
    pm->props = malloc(n * sizeof(Prop));
    pm->p = p;
    pm->r = r;
    memcpy(pm->ents, PROPMNGR_DEF_ENTS, sizeof(Entity) * PROP_TYPE_N);
    memcpy(pm->locs, PROPMNGR_DEF_LOCS, sizeof(PropLocation) * PROP_TYPE_N);
    for(int i = 0; i < pm->n; i++){
        Prop_init(pm->props + i, PROP_TREE, pm->locs[PROP_TREE], &pm->ents[PROP_TREE]);
        pm->props[i].e.y = PMNGR_INIT_Y;
    }
}

void PropManager_destroy(PropManager * pm){
    free(pm->props);
}