#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "entity.h"
#include "road.h"

#define PLAYER_INITIAL_SPEED 0.0
#define PLAYER_MAX_OFFROAD 1.0
#define PLAYER_INITIAL_X 0
#define PLAYER_INITIAL_Y 0

static const Entity PLAYER_DEF_ENT = {0, 0, 0.35, 0.7775};

typedef struct{
    Entity e;
    Road * r;
    float dir[2];
    float speed;
    float turning_coef;
    float max_speed;
    float acceleration;
    int dead;
} Player;

void Player_update(Player * p, float time);

void Player_reset(Player * p);

void Player_respawn(Player * p);

void Player_init(Player * p, Road * r, float turning_coef, float max_speed, float acceleration);

#endif