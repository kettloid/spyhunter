#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include <SDL2/SDL.h>

#include "text.h"
#include "game.h"
#include "scene.h"

#define UI_WIN_HEIGHT 900
#define UI_WIN_WIDTH 1800
#define UI_WIN_TITLE "Spyhunter"

#define UI_GAME_X 0
#define UI_GAME_Y 0
#define UI_GAME_HEIGHT UI_WIN_HEIGHT
#define UI_GAME_WIDTH 1200

#define UI_LEGEND_X UI_GAME_WIDTH
#define UI_LEGEND_Y 0
#define UI_LEGEND_HEIGHT 60
#define UI_LEGEND_WIDTH (UI_WIN_WIDTH - UI_GAME_WIDTH)

#define UI_LEGEND_CONTENT "Spyhunter"

#define UI_STATE_X UI_GAME_WIDTH
#define UI_STATE_Y UI_LEGEND_HEIGHT
#define UI_STATE_HEIGHT 800
#define UI_STATE_WIDTH UI_LEGEND_WIDTH

#define UI_LETTERS_X UI_GAME_WIDTH
#define UI_LETTERS_Y (UI_STATE_Y + UI_STATE_HEIGHT)
#define UI_LETTERS_HEIGHT (UI_WIN_HEIGHT - UI_LETTERS_Y)
#define UI_LETTERS_WIDTH UI_LEGEND_WIDTH

#define UI_LETTERS_CONTENT ""

#define UI_ACTION_N 8

typedef enum{
    UI_ACT_QUIT, UI_ACT_NEW, UI_ACT_PAUSE, UI_ACT_LEFT, UI_ACT_RIGHT, UI_ACT_UP, UI_ACT_DOWN, UI_ACT_CAMERA
} UIAction;


typedef struct{
    SDL_Texture * c;
    SDL_Renderer * r;
    Font * f;
    char * line;
} UIText;

typedef struct{
    SDL_Texture * c;
    SDL_Renderer * r;
    Font * f;
    Game * g;
} UIState;

typedef struct{
    SDL_Texture * c;
    unsigned char background[3];
    Scene * s;
} UIGame;

typedef struct{
    SDL_Window * w;
    SDL_Renderer * r;
    UIText legend;
    UIText letters;
    UIState state;
    UIGame game;
    UIAction act[UI_ACTION_N];
} UI;

void UIGame_setbackground(UIGame * u, unsigned char background[3]);

void UI_init(UI * u, SDL_Renderer * renderer, SDL_Window * window, Scene * s, Font * f);

void UI_update(UI * u);

void UI_handle(UI * u);

void UI_destroy(UI * u);

#endif