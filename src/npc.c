#include "npc.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "player.h"
#include "entity.h"
#include "road.h"

static inline float randf(){
    return rand() / (float)RAND_MAX;
}

static inline float randfn(){
    return (2 * randf() - 1);
}

void NPC_init(NPC * n, NPCType t, NPCAffiliation af, Entity * e, float speed, float rpos){
    n->t = t;
    n->af = af;
    n->speed = speed;
    n->rpos = rpos;
    n->bspeed = 0;
    n->dead = 0;
    n->dtime = 0;
    n->move = 0;
    memcpy(&n->e, e, sizeof(Entity));
}

char NPCManager_attempt_spawn(NPCManager * nm){
    float delay =
            nm->spwn_delay * (1 + nm->spwn_delay_d *
            randfn());

    if(nm->spwn_timer > delay){
        nm->spwn_timer = 0;
        return 1;
    }
    return 0;
}

void NPCManager_update_single(NPCManager * nm, NPC * n, float time){
    float dst = Entity_vdistance(&n->e, &nm->p->e);
    int bflag = 0;
    n->move = dst > nm->dspwn_b;

    if(n->dead){
        n->dtime += time;
        if(n->dtime > NPCMNGR_DSPWN_DELAY)
            n->move = 1;
        return;
    }

    if(fabs(n->bspeed) > NPCMNGR_BEPSILON){
        n->bspeed -= SIGN(n->bspeed) * NPCMNGR_BDECCEL * time;
        n->e.x += n->bspeed * time;
        bflag = 1;
    }

    int prev_branches = Road_state(nm->r, n->e.y, NULL, NULL);
    n->e.y += n->speed * time;
        
    int cur_branches = Road_state(nm->r, n->e.y, NULL, NULL);
    int branch = Road_get_branch(nm->r, &n->e);

    if(branch == -1){
        if(fabs(n->bspeed) > NPCMNGR_BEPSILON)
            nm->kill_counter[n->af]++;

        n->dead = 1;
        n->dtime = 0;
        return;
    }
        
    float *widths = malloc(nm->r->max_branch * sizeof(float));
    float *positions = malloc(nm->r->max_branch * sizeof(float));
    Road_state(nm->r, n->e.y, positions, widths);

    if(cur_branches == prev_branches && !bflag){
        n->e.x = n->rpos * widths[branch] + positions[branch];
    }else{
        n->rpos = (n->e.x - positions[branch]) / widths[branch];
    }

    free(widths);
    free(positions);
}

void NPCManager_update(NPCManager * nm, float time){
    nm->spwn_timer += time;
    for(int i = 0; i < nm->n; i++){
        NPCManager_update_single(nm, nm->npcs + i, time);
    }
}

void NPCManager_respawn_single(NPCManager * nm, NPC * n){
    NPCType t = NPC_CAR;
    NPCAffiliation af = rand() % NPC_AFFILIATION_N;

    float nspeed;

    float rpos = NPCMNGR_RPOS_RANGE * randfn();
    float vpos, hpos;

    if(nm->speeds[t] > nm->p->speed){
        vpos = nm->p->e.y - nm->spwn_b;
        nspeed = (1 + nm->spwn_delay_d * randf()) * nm->speeds[t];
    }else{
        vpos = nm->p->e.y + nm->spwn_b;
        nspeed = (1 - nm->spwn_delay_d * randf()) * nm->speeds[t];
    }

    float *widths = malloc(nm->r->max_branch * sizeof(float));
    float *positions = malloc(nm->r->max_branch * sizeof(float));

    int branches = Road_state(nm->r, vpos, positions, widths);
    int branch = rand() % branches;

    hpos = positions[branch] + rpos * widths[branch];

    Entity e = {hpos, vpos, nm->ents[t].w, nm->ents[t].h};

    NPC_init(n, t, af, &e, nspeed, rpos);

    free(widths);
    free(positions);
}

void NPCManager_respawn(NPCManager * nm){
    for(int i = 0; i < nm->n; i++){
        if(nm->npcs[i].move && NPCManager_attempt_spawn(nm)){
            NPCManager_respawn_single(nm, nm->npcs + i);
        }
    }
}

void NPCManager_inner_collide(NPCManager * nm){
    for(int i = 0; i < nm->n; i++){
        for(int j = i + 1; j < nm->n; j++){
            if(nm->npcs[i].dead || nm->npcs[j].dead)
                continue;

            if(Entity_collide(&nm->npcs[i].e, &nm->npcs[j].e)){
                if(nm->npcs[i].e.y > nm->npcs[j].e.y){
                    nm->npcs[i].speed *= 1.0 + NPCMNGR_BUMP_SPEED;
                    nm->npcs[j].speed *= 1.0 - NPCMNGR_BUMP_SPEED;
                }else{
                    nm->npcs[j].speed *= 1.0 + NPCMNGR_BUMP_SPEED;
                    nm->npcs[i].speed *= 1.0 - NPCMNGR_BUMP_SPEED;
                }
            }

        }
    }
}

char NPCManager_player_collide(NPCManager * nm){
    for(int i = 0; i < nm->n; i++){
        if(nm->npcs[i].dead)
            continue;

        if(fabs(nm->npcs[i].bspeed) > NPCMNGR_BEPSILON)
            continue;
        
        if(Entity_collide(&nm->npcs[i].e, &nm->p->e)){
            if (Entity_vdistance(&nm->npcs[i].e, &nm->p->e) < nm->p->e.h){
                if(nm->npcs[i].e.x > nm->p->e.x){
                    nm->npcs[i].bspeed = NPCMNGR_BSPEED_MUL * nm->p->turning_coef * nm->p->speed;
                }else{
                    nm->npcs[i].bspeed = -NPCMNGR_BSPEED_MUL * nm->p->turning_coef * nm->p->speed;
                }
                continue;
            }
            return 1;
        }
    }
    return 0;
}

void NPCManager_reset_kill_counter(NPCManager * nm){
    memset(nm->kill_counter, 0, NPC_AFFILIATION_N * sizeof(int));
}

void NPCManager_reset(NPCManager * nm){
    for(int i = 0; i < nm->n; i++){
        memset(&nm->npcs[i], 0, sizeof(NPC));
        nm->npcs[i].move = 1;
        nm->npcs[i].e.y = NPCMNGR_INIT_Y;
    }
}

void NPCManager_init(NPCManager * nm, Player * p, Road * r, float spawn_dst, float despawn_dst, int n){
    nm->n = n;
    nm->npcs = malloc(n * sizeof(NPC));
    nm->p = p;
    nm->r = r;
    nm->spwn_b = spawn_dst;
    nm->dspwn_b = despawn_dst;
    nm->spwn_timer = 0;
    nm->spwn_delay = NPCMNGR_DEF_SPWN_DELAY;
    nm->spwn_delay_d = NPCMNGR_DEF_SPWN_DELAY_D;
    nm->speed_d = NPCMNGR_DEF_SPEED_D;

    memcpy(nm->ents, NPCMNGR_DEF_ENTS, NPC_TYPE_N * sizeof(Entity));
    memcpy(nm->speeds, NPCMNGR_DEF_SPEEDS, NPC_TYPE_N * sizeof(float));

    NPCManager_reset_kill_counter(nm);

    NPCManager_reset(nm);
}

void NPCManager_destroy(NPCManager * nm){
    free(nm->npcs);
}