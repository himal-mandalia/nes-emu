#include "Controller.h"

Controller::Controller() {
    reset_buttons();
    
    read_index = 0;
    strobe = 0;
}

inline void Controller::reset_buttons() {
    buttons[BUTTON_A]       = 0;
    buttons[BUTTON_B]       = 0;
    buttons[BUTTON_SELECT]  = 0;
    buttons[BUTTON_START]   = 0;
    buttons[BUTTON_UP]      = 0;
    buttons[BUTTON_DOWN]    = 0;
    buttons[BUTTON_LEFT]    = 0;
    buttons[BUTTON_RIGHT]   = 0;
}

Byte Controller::read() {
    return buttons[read_index++];
}

void Controller::write(Byte value) {
    if(value == 1)
        strobe = 1;
    else if(value == 0 && strobe == 1) {
        if(read_index == 7) reset_buttons();
        read_index = 0;
        strobe = 0;
    }
}

void Controller::set_button_state(int button, Byte state) {
    buttons[button] = state;
}

