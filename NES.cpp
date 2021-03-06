#include "NES.h"

NES::NES(const char* rom_file, Display &display) :
    rom(),
    cpu_mem(CPU_MEM_SIZE),
    ppu(), 
    mapper(cpu_mem, ppu, controller_1),
    cpu(mapper),
    controller_1(),
    display(display) {
    
    try {
        rom.load_ROM(rom_file);
    }
    catch(const char* ex) {
        cerr << ex << endl;
        exit(-1);
    }
    
    ppu.setup_mirroring(rom.get_mirroring());
    
    // fill ram with PRG bank(s)
    cpu_mem.fast_write(rom.get_PRG_bank_1(), 0x8000, PRG_BANK_SIZE);
    // some games only have 1 16k PRG bank, so the banks should be mirrored
    
    if(rom.get_num_PRG_banks() == 1)
        cpu_mem.fast_write(rom.get_PRG_bank_1(), 0xC000, PRG_BANK_SIZE);
        
    // otherwise write the second PRG bank
    else
        cpu_mem.fast_write(rom.get_PRG_bank_2(), 0xC000, PRG_BANK_SIZE);
    // fill PPU ram with CHRROM
    
    if(rom.get_num_CHR_banks() > 0)
        ppu.load_CHR_bank(rom.get_CHR_bank());
        
    run();
}

void NES::run() {
    cpu.reset();
    ppu.reset();
    
    // Actually 113.66666666666667
    const int cpu_cycles_per_scanline = 113; // make into a global const
    
    // Tracks cycles used/remaining
    long cpu_cycles_remaining = 0;
    
    SDL_Event event;
    
    // Main emulation loop
    for(long frame = 0; ; frame++) {
        
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
                return;
                
            else if(event.type == SDL_KEYDOWN)
                handle_key_input(event.key.keysym.sym, PRESSED);
            else if(event.type == SDL_KEYUP)
                handle_key_input(event.key.keysym.sym, RELEASED);
                
            if(event.type == SDL_JOYBUTTONDOWN
                || event.type == SDL_JOYBUTTONUP
                || event.type == SDL_JOYAXISMOTION)
                
                handle_joy_input(event);
        }
        
        for(int scanline = 0; scanline < 262; scanline++) {
            
            if(ppu.VBlank_occurring()) cpu.set_interrupt(NMI);

            cpu_cycles_remaining = cpu.emulate(cpu_cycles_per_scanline
            + cpu_cycles_remaining
            // This accounts for the remainder cycles - but needs checking
            + ((frame * scanline) % 3 == 0 ? 2 : 0));

            ppu.emulate();
        }
        
        display.show(ppu.get_framebuffer());
    }
}

void NES::handle_key_input(int key, int state) {
    switch(key) {
        case SDLK_s:        controller_1.set_button_state(BUTTON_A, state); break;
        case SDLK_a:        controller_1.set_button_state(BUTTON_B, state); break;
        case SDLK_RETURN:   controller_1.set_button_state(BUTTON_START, state); break;
        case SDLK_TAB:      controller_1.set_button_state(BUTTON_SELECT, state); break;
        case SDLK_UP:       controller_1.set_button_state(BUTTON_UP, state); break;
        case SDLK_DOWN:     controller_1.set_button_state(BUTTON_DOWN, state); break;
        case SDLK_LEFT:     controller_1.set_button_state(BUTTON_LEFT, state); break;
        case SDLK_RIGHT:    controller_1.set_button_state(BUTTON_RIGHT, state); break;
    }
}

void NES::handle_joy_input(const SDL_Event &event) {
    switch(event.type) {
        case SDL_JOYAXISMOTION: {
            switch(event.jaxis.axis) {
                case 0: {
                    if(event.jaxis.value < -32000)
                        controller_1.set_button_state(BUTTON_LEFT, PRESSED);
                    else if(event.jaxis.value > 32000)
                        controller_1.set_button_state(BUTTON_RIGHT, PRESSED);
                    else {
                        controller_1.set_button_state(BUTTON_LEFT, RELEASED);
                        controller_1.set_button_state(BUTTON_RIGHT, RELEASED);
                    }
                    break;
                }
                case 1: {
                    if(event.jaxis.value < -32000)
                        controller_1.set_button_state(BUTTON_UP, PRESSED);
                    else if(event.jaxis.value > 32000)
                        controller_1.set_button_state(BUTTON_DOWN, PRESSED);
                    else {
                        controller_1.set_button_state(BUTTON_UP, RELEASED);
                        controller_1.set_button_state(BUTTON_DOWN, RELEASED);
                    }
                    break;
                }
            }
            break;
        }
        case SDL_JOYBUTTONDOWN: {
            switch(event.jbutton.button) {
                case 0: controller_1.set_button_state(BUTTON_B, PRESSED); break;
                case 1: controller_1.set_button_state(BUTTON_A, PRESSED); break;
                case 2: controller_1.set_button_state(BUTTON_SELECT, PRESSED); break;
                case 3: controller_1.set_button_state(BUTTON_START, PRESSED); break;
            }
            break;
        }
        case SDL_JOYBUTTONUP: {
            switch(event.jbutton.button) {
                case 0: controller_1.set_button_state(BUTTON_B, RELEASED); break;
                case 1: controller_1.set_button_state(BUTTON_A, RELEASED); break;
                case 2: controller_1.set_button_state(BUTTON_SELECT, RELEASED); break;
                case 3: controller_1.set_button_state(BUTTON_START, RELEASED); break;
            }
            break;
        }
    }
}

void NES::print_ascii() {
    for(int i = 0; i < 240; i++) {
        for(int j = 0; j < 256; j++) {
            printf("%d", ppu.get_framebuffer()[i][j]);
        }
        puts("");
    }
}
