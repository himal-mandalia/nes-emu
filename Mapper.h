//  Memory Map
//  -------------
//   +---------+-------+-------+-----------------------+
//   | Address | Size  | Flags | Description           |
//   +---------+-------+-------+-----------------------+
//   | $0000   | $800  |       | RAM                   |
//   | $0800   | $800  | M     | RAM                   |
//   | $1000   | $800  | M     | RAM                   |
//   | $1800   | $800  | M     | RAM                   |
//   | $2000   | 8     |       | Registers             |
//   | $2008   | $1FF8 |  R    | Registers             |
//   | $4000   | $20   |       | Registers             |
//   | $4020   | $1FDF |       | Expansion ROM         |
//   | $6000   | $2000 |       | SRAM                  |
//   | $8000   | $4000 |       | PRG-ROM               |
//   | $C000   | $4000 |       | PRG-ROM               |
//   +---------+-------+-------+-----------------------+
//          Flag Legend: M = Mirror of $0000
//                       R = Mirror of $2000-2008 every 8 bytes
//                           (e.g. $2008=$2000, $2018=$2000, etc.)

#ifndef MAPPER_H
#define MAPPER_H

#include "Constants.h"
#include "Memory.h"
#include "PPU.h"
#include "Controller.h"

class Mapper {
private:
    Memory &mem;
    PPU &ppu;
    Controller &controller_1;
    
    Word translate_address(Word address) const;
    
public:
    Mapper(Memory &_mem, PPU &_ppu, Controller &controller_1);
    
    Byte read(Word address) const;
    Word read_word(Word address) const;
    void write(Byte data, Word address);
    void write(const Byte* data, Word address, int length); 
};

#endif // MAPPER_H
