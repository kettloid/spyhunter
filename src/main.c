#include <SDL2/SDL.h>
#include <stdio.h>
#include <locale.h>

#include "3D/3D.h"

#include "npc.h"
#include "prop.h"
#include "scene.h"
#include "game.h"
#include "text.h"
#include "ui.h"

#define RENDERER_WIDTH (UI_GAME_WIDTH / 2)
#define RENDERER_HEIGHT (UI_GAME_HEIGHT / 2)

#define MODEL_N 2
#define MAT_N 5
#define SCENEOBS_N 4

#define PI 3.14159265359

void sh_camera_back(Camera * camera){
    Camera_move(camera, 0, 63.128, -59.228);
    Camera_rotate(camera, -0.8257, PI, 0);
}

void sh_camera_top(Camera * camera){
    Camera_move(camera, 0, 130, 0);
    Camera_rotate(camera, -PI / 2, PI, 0);
}

void sh_renderer_init(Renderer * r){
    Camera * camera = malloc(sizeof(Camera));
    Frustum * frustum = malloc(sizeof(Frustum));
    Light * light = malloc(sizeof(Light));

    Light_init(light, 0, 60, 100, 0.4);

    Frustum_init(frustum, 2, 1.5, 3, 180);
    Camera_init(camera, frustum);

    sh_camera_back(camera);

    Renderer_init(r, camera, light, RENDERER_WIDTH, RENDERER_HEIGHT, 3);

}

void sh_renderer_destroy(Renderer * r){
    free(r->c->f);
    free(r->c);
    free(r->l);
    Renderer_destroy(r);
}

void sh_game_init(Game * game){
    Player * player = malloc(sizeof(Player));
    Road * road = malloc(sizeof(Road));
    NPCManager * npcm = malloc(sizeof(NPCManager));
    PropManager * pm = malloc(sizeof(PropManager));

    Road_init(road, 2);
    Player_init(player, road, 0.6, 12, 6);
    NPCManager_init(npcm, player, road, 20, 30, 10);
    PropManager_init(pm, player, road, 3, 20, 30, 15);
    Game_init(game, player, npcm, pm, road, 18);
}

void sh_game_destroy(Game * game){
    free(game->r);
    free(game->p);
    free(game->nm);
    free(game->pm);
}

void sh_load_models(Model * mos, MaterialContainer * mc){
    Model_fromfile(mos, mc, "resources/Car.obj");
    Model_fromfile(mos+1, mc + 4, "resources/Palm.obj");
}

void sh_load_materials(MaterialContainer * mc){
    MaterialContainer_load(mc, "resources/Car.mtl");
    MaterialContainer_load(mc + 1, "resources/CarGreen.mtl");
    MaterialContainer_load(mc + 2, "resources/CarBlue.mtl");
    MaterialContainer_load(mc + 3, "resources/road.mtl");
    MaterialContainer_load(mc + 4, "resources/Palm.mtl");
}

void sh_destroy_models(Model * mos){
    for(int i = 0; i < MODEL_N; i++)
        Model_destroy(mos + i);
}

void sh_destroy_materials(MaterialContainer * mc){
    for(int i = 0; i < MAT_N; i++)
        MaterialContainer_destroy(mc + i);
}

void sh_scene_init(Scene * scene, MaterialContainer * mc, Model * mos, Game * game, Renderer * renderer){
    SceneObject * sos = malloc(SCENEOBS_N * sizeof(SceneObject));
    SceneObject_init(sos, SCENEOBJECT_PLAYER, mc, mos);
    SceneObject_init(sos + 1, SCENEOBJECT_NPC, mc + 1, mos);
    SceneObject_init(sos + 2, SCENEOBJECT_ROAD, mc + 3, NULL);
    SceneObject_init(sos + 3, SCENEOBJECT_PROP, mc + 4, mos + 1);
    Scene_init(scene, game, renderer, sos, 4, 3.0);
}

void sh_scene_destroy(Scene * scene){
    free(scene->sos);
}

void sh_ui_init(UI * ui, SDL_Renderer * renderer, SDL_Window * window, Scene * scene){
    Font * font = malloc(sizeof(Font));
    Font_init(font, renderer, 32, 64, 16, 16, "resources/unifonts.bmp");
    UI_init(ui, renderer, window, scene, font);

    unsigned char background[3] = {30, 120, 40};
    UIGame_setbackground(&ui->game, background);
}

void sh_ui_destroy(UI * ui){
    Font_destroy(ui->letters.f);
    free(ui->letters.f);
    UI_destroy(ui);
}

int main(){
    Game game;
    sh_game_init(&game);

    Renderer grenderer;
    sh_renderer_init(&grenderer);

    UI ui;
    Scene scene;
    Model mos[MODEL_N];
    MaterialContainer mc[MAT_N];

    sh_load_materials(mc);
    sh_load_models(mos, mc);
    sh_scene_init(&scene, mc, mos, &game, &grenderer);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow(
        UI_WIN_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        UI_WIN_WIDTH, UI_WIN_HEIGHT, 0);

    SDL_Renderer * renderer = SDL_CreateRenderer(
                window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    sh_ui_init(&ui, renderer, window, &scene);

    setlocale(LC_ALL, "");

    int start = SDL_GetTicks();
    float deltatime = 1;

    int pause = 0;
    int camerapos = 0;

    while(1){
        deltatime = (SDL_GetTicks() - start) / 1000.0;
        start = SDL_GetTicks();

        game.p->dir[0] = 0;
        game.p->dir[1] = 0;

        UI_handle(&ui);
        if(ui.act[UI_ACT_QUIT])
            break;

        if(ui.act[UI_ACT_NEW])
            Game_reset(&game);

        if(ui.act[UI_ACT_PAUSE])
            pause ^= 1;
        if(ui.act[UI_ACT_CAMERA])
            camerapos ^= 1;

        if(ui.act[UI_ACT_LEFT])
            game.p->dir[0] = 1;
        if(ui.act[UI_ACT_RIGHT])
            game.p->dir[0] = -1;
        if(ui.act[UI_ACT_DOWN])
            game.p->dir[1] = -1;
        if(ui.act[UI_ACT_UP])
            game.p->dir[1] = 1;

        if(!pause)
            Game_update(&game, deltatime);

        if(camerapos)
            sh_camera_top(ui.game.s->r->c);
        else
            sh_camera_back(ui.game.s->r->c);

        UI_update(&ui);

        fprintf(stderr, "%f\n", 1/deltatime);

    }
    return 0;
}

