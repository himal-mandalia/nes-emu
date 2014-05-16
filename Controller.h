//  ___________________________________
// |    _                              |
// |  _| |_                Nintendo    |
// | |_   _| SELECT START              |
// |   |_|    (==)  (==)  ( B ) ( A )  |
// |___________________________________|

// ******************************* Joysticks ******************************
// 
//   There are two joysticks which are accessed via locations $4016 and
// $4017. To reset joysticks, write first 1, then 0 into $4016. This way, you
// will generate a strobe in the joysticks' circuitry. Then, read either from
// $4016 (for joystick 0) or from $4017 (for joystick 1).  Each read will
// give you the status of a single button in the 0th bit (1 if pressed, 0
// otherwise): 
// 
// Read # |    1      2      3      4      5      6      7      8
// -------+---------------------------------------------------------
// Button |    A      B   SELECT   START   UP    DOWN   LEFT  RIGHT
// 
//   Bit 1 indicates whether joystick is connected to the port or not. It is
// set to 0 if the joystick is connected, 1 otherwise. Bits 6 and 7 of
// $4016/$4017 also seem to have some significance, which is not clear yet.
// The rest of bits is set to zeroes. Some games expect to get *exactly* $41
// from $4016/$4017, if a button is pressed, which has to be taken into
// account.

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Constants.h"

class Controller {
    Byte buttons[8];
    
    Byte strobe;
    
    int read_index;
    
public:
    Controller();
    
    void reset_buttons();
    
    Byte read();
    
    void write(Byte value);
    
    void set_button_state(int button, Byte state);
};

#endif // CONTROLLER_H
