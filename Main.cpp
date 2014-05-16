#include "NES.h"

const char* DEFAULT_ROM = "roms/Balloon Fight.nes";

SDL_Surface* screen;
SDL_Joystick* joystick1;

bool init_SDL(int scale, bool fs) {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) 
		return false;
	
    int sdl_flags = SDL_HWSURFACE | SDL_DOUBLEBUF | (fs ? SDL_FULLSCREEN : 0);
	
	if((screen = SDL_SetVideoMode(256 * scale, 240 * scale, 
	    24, sdl_flags)) == NULL)
		return false;
	
	// Hide the mouse cursor if in fullscreen mode
    if(fs) SDL_ShowCursor(false);
    
    // Open joystick 0, if available
    if(SDL_NumJoysticks() > 0) {
        SDL_JoystickEventState(SDL_ENABLE);
        joystick1 = SDL_JoystickOpen(0);
    }
    
	return true;
}

void clean_up() {
    if(joystick1) SDL_JoystickClose(joystick1);
    if(screen) SDL_FreeSurface(screen);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    bool fs = false;
    int scale = 2;
    if(argc == 3) scale = atoi(argv[2]);
    if(argc == 4) fs = true;
    
    if (!init_SDL(scale, fs)) {
        cerr << "Couldn't initialise SDL graphics" << endl;
        clean_up();
        exit(-1);
    }
    
    Display disp(screen, scale);
    
    if(argc < 2) NES nes(DEFAULT_ROM, disp);
    else NES nes(argv[1], disp);
    
    clean_up();
    
    return 0;
}
