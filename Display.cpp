#include "Display.h"

Display::Display(SDL_Surface* _screen) 
    : screen(_screen), scale(1) {
}

Display::Display(SDL_Surface* _screen, int s) 
    : screen(_screen), scale(s) {}

void Display::show(const Byte (*framebuffer)[256]) {        
    for(int i = 0; i < 240; i++) {
        for(int j = 0; j < 256; j++) {
            SDL_Rect pixel = {j * scale, i * scale, scale, scale };
            SDL_FillRect(screen, &pixel, NTSC_HEX_PALETTE[framebuffer[i][j]]);
        }
    }
    SDL_Flip(screen);
}
