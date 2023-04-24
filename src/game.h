#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "player.h"
#include "npc.h"
#include "road.h"
#include "prop.h"

#define GAME_CAR_THRESHOLD 1000
#define GAME_INITIAL_CARS 3
#define GAME_FREE_CARS_T 10
#define GAME_SCORE_PER_KILL 300
#define GAME_FRIENDLY_KILL_PENALTY 5
#define GAME_PEPSILON 0.01
#define GAME_RESPAWN_WAIT 1.5

typedef struct{
    Player * p;
    NPCManager * nm;
    PropManager * pm;
    Road * r;
    float time;
    float penalty_timer;
    float score;
    float car_progress;
    float player_range;
    float respawn_timer;
    int carn;
} Game;

void Game_init(Game * g, Player * p, NPCManager * nm, PropManager * pm, Road * r, float player_range);

void Game_update(Game * g, float time);

void Game_reset(Game * g);

#endif