#include "player.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "entity.h"
#include "road.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void Player_update(Player * p, float time){
    float nspeed;
    if(p->dead)
        return;

    p->e.x += p->turning_coef * p->dir[0] * p->speed * time;
   
    nspeed = MAX(0, MIN(p->max_speed, p->speed + p->dir[1] * p->acceleration * time));
    if(nspeed != p->speed){
        p->e.y += MAX(0, p->speed * time + p->dir[1] * p->acceleration * time * time);
    }else{
        p->e.y += MAX(0, p->speed * time);
    }
    p->speed = nspeed;

    if(Road_dst_to_nearest(p->r, &p->e) > PLAYER_MAX_OFFROAD)
        p->dead = 1;

}

void Player_reset(Player * p){
    p->e.x = PLAYER_INITIAL_X;
    p->e.y = PLAYER_INITIAL_Y;
    p->dir[0] = 0;
    p->dir[1] = 0;
    p->speed = 0;
    p->dead = 0;
}

void Player_respawn(Player * p){
    float *positions, *widths;
    positions = malloc(p->r->max_branch * sizeof(float));
    widths = malloc(p->r->max_branch * sizeof(float));
    Road_state(p->r, p->e.y, positions, widths);
    p->e.x = positions[0] + widths[0] - p->e.w;

    free(positions);
    free(widths);

    p->speed = PLAYER_INITIAL_SPEED;
    p->dead = 0;
}

void Player_init(Player * p, Road * r, float turning_coef, float max_speed, float acceleration){
    p->speed = PLAYER_INITIAL_SPEED;
    p->r = r;
    p->dead = 0;
    p->turning_coef = turning_coef;
    p->max_speed = max_speed;
    p->acceleration = acceleration;
    memcpy(&p->e, &PLAYER_DEF_ENT, sizeof(Entity));
}