#ifndef PROP_H_INCLUDED
#define PROP_H_INCLUDED

#include "entity.h"
#include "road.h"
#include "player.h"

#define PROP_TYPE_N 1

#define PMNGR_DEF_SPWN_THRES 100
#define PMNGR_INIT_Y -200

typedef enum{
    PROP_TREE = 0
} PropType;

typedef enum{
    PROP_ONROAD = 0, PROP_OFFROAD = 1
} PropLocation;

typedef struct{
    PropType t;
    PropLocation l;
    Entity e;
    char move;
} Prop;

typedef struct{
    Prop * props;
    Player * p;
    Road * r;
    Entity ents[PROP_TYPE_N];
    PropLocation locs[PROP_TYPE_N];
    float spwn_b, dspwn_b;
    float prop_range;
    float spawn_distance, spawn_distance_threshold;
    int n;
} PropManager;

static const Entity PROPMNGR_DEF_ENTS[] = {
    {0, 0, 0, 0}
};

static const PropLocation PROPMNGR_DEF_LOCS[] = {
    PROP_OFFROAD
};

void Prop_init(Prop * p, PropType t, PropLocation l, Entity * e);

char PropManager_attempty_spawn(PropManager * pm, Entity * e);

void PropManager_update(PropManager * pm, float time);

void PropManager_respawn(PropManager * pm);

void PropManager_init(PropManager * pm, Player * p, Road * r, float spawn_interval, float spawn_dst, float despawn_dst, int n);

void PropManager_destroy(PropManager * pm);

#endif