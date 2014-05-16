#ifndef NES_H
#define NES_H

// For events
#include "SDL.h"

#include "Constants.h"
#include "CPU.h"
#include "PPU.h"
#include "Memory.h"
#include "Mapper.h"
#include "ROM.h"
#include "Display.h"
#include "Controller.h"

const int NTSC_FPS = 60;

class NES {
    ROM rom;
    Memory cpu_mem;
    PPU ppu;
    Mapper mapper;
    CPU cpu;
    
    Controller controller_1, controller_2;
    
    Display& display;
    
    void handle_key_input(int key, int state);
    void handle_joy_input(const SDL_Event &event);
    
    void print_ascii();
    
public:
    NES(const char* rom_file, Display &display);
    
    void run();
};

#endif // NES_H
