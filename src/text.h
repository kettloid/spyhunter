#ifndef TEXT_H_INCLUDED
#define TEXT_H_INCLUDED

#include <SDL2/SDL.h>

typedef enum{
    FONT_FULLWIDTH, FONT_HALFWIDTH
} FontWidth;

typedef struct{
    SDL_Texture * atlas;
    SDL_Renderer * r;
    int off_x, off_y;
    int full_w, full_h;
} Font;

void Font_init(Font * f, SDL_Renderer * r, int off_x, int off_y, int full_w, int full_h, char * filename);

void Font_write(Font * f, SDL_Texture * target, FontWidth width, int x, int y, char * content);

void Font_destroy(Font * f);



#endif