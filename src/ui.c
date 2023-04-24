#include "ui.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

#include "text.h"

void UIText_init(UIText * u, SDL_Renderer * r, Font * f, char * line, int width, int height){
    u->r = r;
    u->c = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, width, height);
    u->f = f;
    u->line = line;
}

void UIText_destroy(UIText * u){
    SDL_DestroyTexture(u->c);
}

void UIText_update(UIText * u){
    SDL_SetRenderTarget(u->r, u->c);
    SDL_SetRenderDrawColor(u->r, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(u->r);
    Font_write(u->f, u->c, FONT_HALFWIDTH, 0, 0, u->line);
    
}

void UIState_init(UIState * u, SDL_Renderer * r, Font * f, Game * g, int width, int height){
    u->r = r;
    u->c = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, width, height);
    u->f = f;
    u->g = g;
}

void UIState_destroy(UIState * u){
    SDL_DestroyTexture(u->c);
}

void UIState_update(UIState * u){
    char buffer[300];
    SDL_SetRenderTarget(u->r, u->c);
    SDL_SetRenderDrawColor(u->r, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(u->r);

    sprintf(buffer, "Wynik: %.0f", u->g->score);
    Font_write(u->f, u->c, FONT_HALFWIDTH, 0, 0, buffer);

    sprintf(buffer, "Czas: %.2fs", u->g->time);
    Font_write(u->f, u->c, FONT_HALFWIDTH, 0, u->f->full_h, buffer);

    sprintf(buffer, "Samochody: %d", u->g->carn);
    Font_write(u->f, u->c, FONT_HALFWIDTH, 0, 2 * u->f->full_h, buffer);
}

void UIGame_init(UIGame * u, SDL_Renderer * r, Scene * s){
    u->c = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, s->r->width, s->r->height);
    u->s = s;
}

void UIGame_destroy(UIGame * u){
    SDL_DestroyTexture(u->c);
}

void UIGame_update(UIGame * u){
    void * screen;
    int pitch;
    SDL_LockTexture(u->c, NULL, &screen, &pitch);

    u->s->r->screen = screen;
    Renderer_clear(u->s->r, u->background);
    Scene_render(u->s);
    SDL_UnlockTexture(u->c);
}

void UIGame_setbackground(UIGame * u, unsigned char background[3]){
    memcpy(u->background, background, 3);
}

void UI_init(UI * u, SDL_Renderer * renderer, SDL_Window * window, Scene * s, Font * f){
    u->w = window;
    u->r = renderer;
    
    UIText_init(&u->legend, u->r, f, UI_LEGEND_CONTENT, UI_LEGEND_WIDTH, UI_LEGEND_HEIGHT);
    UIText_init(&u->letters, u->r, f, UI_LETTERS_CONTENT, UI_LETTERS_WIDTH, UI_LETTERS_HEIGHT);
    UIState_init(&u->state, u->r, f, s->g, UI_STATE_WIDTH, UI_STATE_HEIGHT);
    UIGame_init(&u->game, u->r, s);
}

void UI_update(UI * u){
    SDL_Rect game_r = {UI_GAME_X, UI_GAME_Y, UI_GAME_WIDTH, UI_GAME_HEIGHT};
    SDL_Rect legend_r = {UI_LEGEND_X, UI_LEGEND_Y, UI_LEGEND_WIDTH, UI_LEGEND_HEIGHT};
    SDL_Rect letters_r = {UI_LETTERS_X, UI_LETTERS_Y, UI_LETTERS_WIDTH, UI_LETTERS_HEIGHT};
    SDL_Rect state_r = {UI_STATE_X, UI_STATE_Y, UI_STATE_WIDTH, UI_STATE_HEIGHT};
    UIText_update(&u->legend);
    UIText_update(&u->letters);
    UIState_update(&u->state);
    UIGame_update(&u->game);

    SDL_SetRenderTarget(u->r, NULL);
    SDL_RenderCopy(u->r, u->legend.c, NULL, &legend_r);
    SDL_RenderCopy(u->r, u->state.c, NULL, &state_r);
    SDL_RenderCopy(u->r, u->game.c, NULL, &game_r);
    SDL_RenderCopy(u->r, u->letters.c, NULL, &letters_r);
    SDL_RenderPresent(u->r);
    SDL_UpdateWindowSurface(u->w);

}

void UI_handle(UI * u){
        SDL_Event event;
        memset(u->act, 0, sizeof(UIAction) * UI_ACTION_N);

        const uint8_t * keystate = SDL_GetKeyboardState(NULL);
        
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    u->act[UI_ACT_QUIT] = 1;
                    break;

                case SDL_KEYDOWN:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_ESCAPE:
                            u->act[UI_ACT_QUIT]= 1;
                            break;

                        case SDL_SCANCODE_N:
                            u->act[UI_ACT_NEW] = 1;
                            break;

                        case SDL_SCANCODE_P:
                            u->act[UI_ACT_PAUSE] = 1;
                            break;

                        case SDL_SCANCODE_T:
                            u->act[UI_ACT_CAMERA] = 1;
                            break;

                        default:
                            break;
                    }
                    break;
            }
        }

        if(keystate[SDL_SCANCODE_LEFT])
            u->act[UI_ACT_LEFT] = 1;

        if(keystate[SDL_SCANCODE_RIGHT])
            u->act[UI_ACT_RIGHT] = 1;

        if(keystate[SDL_SCANCODE_UP])
            u->act[UI_ACT_UP] = 1;

        if(keystate[SDL_SCANCODE_DOWN])
            u->act[UI_ACT_DOWN] = 1;

}

void UI_destroy(UI * u){
    UIText_destroy(&u->legend);
    UIText_destroy(&u->letters);
    UIState_destroy(&u->state);
    UIGame_destroy(&u->game);
}
