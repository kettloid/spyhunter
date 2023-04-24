#include "text.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

void Font_init(Font * f, SDL_Renderer * r, int off_x, int off_y, int full_w, int full_h, char * filename){
    SDL_Surface * bmp = SDL_LoadBMP(filename);
    f->atlas = SDL_CreateTextureFromSurface(r, bmp);
    SDL_FreeSurface(bmp);

    f->off_x = off_x;
    f->off_y =off_y;
    f->full_w = full_w;
    f->full_h = full_h;
    f->r = r;
}

void Font_destroy(Font * f){
    SDL_DestroyTexture(f->atlas);
}

void Font_write(Font * f, SDL_Texture * target, FontWidth width, int dx, int dy, char * content){
    const int buffer_size = 200;
    wchar_t * buffer = malloc(buffer_size * sizeof(wchar_t));
    int n = mbstowcs(buffer, content, buffer_size);
    int s_x = (width == FONT_FULLWIDTH) ? f->full_w : f->full_w / 2;
    int s_y = f->full_h;

    SDL_SetRenderTarget(f->r, target);

    for(int i = 0; i<n; i++){
        int x = f->off_x + (buffer[i] & 0xFF) * f->full_w;
        int y = f->off_y + ((buffer[i] >> 8) & 0xFF) * f->full_h;
        SDL_Rect out_r = {s_x  * i + dx, dy, s_x, s_y};
        SDL_Rect in_r = {x, y, s_x, s_y};
        SDL_RenderCopy(f->r, f->atlas, &in_r, &out_r);
    }

    free(buffer);
}


