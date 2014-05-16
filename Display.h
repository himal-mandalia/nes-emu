#ifndef DISPLAY_H
#define DISPLAY_H

#include "SDL.h"
#include "Constants.h"

class Display {
    SDL_Surface* screen;
    int scale;
    
public:
    Display(SDL_Surface* _screen);
    Display(SDL_Surface* _screen, int s);
    
    void show(const Byte (*framebuffer)[256]);
};

#endif // DISPLAY_H
