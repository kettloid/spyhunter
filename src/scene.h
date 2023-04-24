#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include "3D/3D.h"
#include "game.h"

#define SCENE_ROAD_RES 60
#define SCENE_PROP_PALM_SCALE 0.6
#define SCENE_NPC_F_XSPEED 13
#define SCENE_NPC_F_ZSPEED 20
#define SCENE_NPC_F_YSPEED 70
#define SCENE_NPC_F_YACC -10
#define SCENE_ROAD_S 1.9999
#define SCENE_PI 3.1415

typedef enum{
    SCENEOBJECT_PLAYER, SCENEOBJECT_ROAD, SCENEOBJECT_NPC, SCENEOBJECT_PROP
} SceneObjectType;

typedef struct{
    SceneObjectType t;
    MaterialContainer * mcs;
    Model * mos;
} SceneObject;

typedef struct{
    float scale;
    Game * g;
    Renderer * r;
    SceneObject *sos;
    int n;
} Scene;

void Scene_render(Scene * s);

void SceneObject_init(SceneObject * so, SceneObjectType t, MaterialContainer * mcs, Model * mos);

void Scene_init(Scene * s, Game * g, Renderer * r, SceneObject * sos, int n, float scale);

void SceneObject_cleanup(SceneObject * so, Object * obs, int n);

#endif
