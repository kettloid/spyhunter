#include "game.h"

#include "player.h"
#include "npc.h"
#include "prop.h"
#include "road.h"
#include <stdio.h>

void Game_init(Game * g, Player * p, NPCManager * nm, PropManager * pm, Road * r, float player_range){
    g->p = p;
    g->nm = nm;
    g->pm = pm;
    g->r = r;
    g->player_range = player_range;
    g->time = 0;
    g->score = 0;
    g->carn = GAME_INITIAL_CARS;
    g->car_progress = 0;
    g->penalty_timer = 0;
    g->respawn_timer = 0;

}

void Game_reset(Game * g){
    g->time = 0;
    g->score = 0;
    g->carn = GAME_INITIAL_CARS;
    g->car_progress = 0;
    g->penalty_timer = 0;
    g->respawn_timer = 0;
    Player_reset(g->p);
    NPCManager_reset(g->nm);
}

void Game_update_penalty(Game * g, float time){
    g->penalty_timer += g->nm->kill_counter[NPC_FRIENDLY] *
                        GAME_FRIENDLY_KILL_PENALTY;

    if(g->penalty_timer >= GAME_PEPSILON)
        g->penalty_timer -= time;
}

float Game_score_change(Game * g, float time, float dst){
    float score_ch = 0;
    if(Road_dst_to_nearest(g->r, &g->p->e) <= 0)
        score_ch += dst;

    score_ch += GAME_SCORE_PER_KILL * g->nm->kill_counter[NPC_ENEMY];

    NPCManager_reset_kill_counter(g->nm);
    if(g->penalty_timer < GAME_PEPSILON)
        return score_ch;

    return 0;
}

void Game_cars_update(Game * g, float score_ch){
    g->car_progress += score_ch;
    if(g->car_progress > GAME_CAR_THRESHOLD){
        g->car_progress -= GAME_CAR_THRESHOLD;
        g->carn++;
    }
}

int Game_player_respawn(Game * g, float time){
    if(g->p->dead){
        g->respawn_timer += time;
        if(g->respawn_timer > GAME_RESPAWN_WAIT){
            if(g->time > GAME_FREE_CARS_T){
                if(g->carn <= 0)
                    return -1;

                g->carn--;
            }
            Player_respawn(g->p);
            NPCManager_reset(g->nm);
            g->respawn_timer = 0;
            return 0;
        }

        return -3;
    }
    return -2;
}

void Game_update(Game * g, float time){
    float player_pr_y = g->p->e.y, score_ch, player_dst;
    
    Player_update(g->p, time);

    player_dst = g->p->e.y - player_pr_y;

    NPCManager_update(g->nm, time);
    NPCManager_respawn(g->nm);
    NPCManager_inner_collide(g->nm);

    if(NPCManager_player_collide(g->nm))
        g->p->dead = 1;

    PropManager_update(g->pm, time);
    PropManager_respawn(g->pm);

    Game_player_respawn(g, time);

    Game_update_penalty(g, time);

    score_ch = Game_score_change(g, time, player_dst);
    Game_cars_update(g, score_ch);

    if(!g->p->dead){
        g->score += score_ch;
        g->time += time;
    }
}