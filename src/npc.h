#ifndef NPC_H_INCLUDED
#define NPC_H_INCLUDED

#include "entity.h"
#include "player.h"
#include "road.h"

#define NPC_TYPE_N 1
#define NPC_AFFILIATION_N 2

#define NPCMNGR_DEF_SPWN_DELAY 0.5
#define NPCMNGR_DEF_SPWN_DELAY_D 0.2
#define NPCMNGR_DSPWN_DELAY 5
#define NPCMNGR_DEF_SPEED_D 0.7
#define NPCMNGR_BUMP_SPEED 0.1
#define NPCMNGR_BSPEED_MUL 2
#define NPCMNGR_BDECCEL 15
#define NPCMNGR_INIT_Y -200
#define NPCMNGR_RPOS_RANGE 0.9

#define NPCMNGR_BEPSILON 0.5

#define SIGN(a) (((a) > 0) ? (1) : (-1))

static const Entity NPCMNGR_DEF_ENTS[] = {
    {0, 0, 0.35, 0.7775}
};

static const float NPCMNGR_DEF_SPEEDS[] = {
    8
};

typedef enum{
    NPC_CAR = 0
} NPCType;

typedef enum{
    NPC_ENEMY = 0, NPC_FRIENDLY = 1
} NPCAffiliation;

typedef struct{
    NPCType t;
    NPCAffiliation af;
    Entity e;
    float speed;
    float rpos;
    float bspeed;
    char move;
    char dead;
    float dtime;
} NPC;

typedef struct{
    NPC * npcs;
    Player * p;
    Road * r;
    Entity ents[NPC_TYPE_N];
    float speeds[NPC_TYPE_N];
    float speed_d;
    float spwn_b, dspwn_b;
    float spwn_timer, spwn_delay, spwn_delay_d;
    int kill_counter[NPC_AFFILIATION_N];
    int n;
} NPCManager;

void NPCManager_update(NPCManager * nm, float time);

void NPCManager_respawn(NPCManager * nm);

void NPCManager_inner_collide(NPCManager * nm);

char NPCManager_player_collide(NPCManager * nm);

void NPCManager_reset_kill_counter(NPCManager * nm);

void NPCManager_init(NPCManager * nm, Player * p, Road * r, float spawn_dst, float despawn_dst, int n);

void NPCManager_destroy(NPCManager * nm);

void NPCManager_reset(NPCManager * nm);

#endif