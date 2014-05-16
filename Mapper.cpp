#include "Mapper.h"

Mapper::Mapper(Memory &_mem, PPU &_ppu, Controller &c_1) 
    : mem(_mem), ppu(_ppu), controller_1(c_1) {}

Byte Mapper::read(Word address) const {
    address = translate_address(address);
    
    switch(address) {
        // PPU Status Register
        case 0x2002:
        // PPU VRAM Register
        case 0x2007: return ppu.read(address);
        
        // Controller 1
        case 0x4016: return controller_1.read();
        
        // Controller 2
        //case 0x4017: return 0;
    }
    
    return mem[address];
}

Word Mapper::read_word(Word address) const {
    address = translate_address(address);
    Word temp = address;
    address = translate_address(address + 1);
    return ((Byte) read(temp) | ((Word) read(address) << 8));
}

void Mapper::write(Byte data, Word address) {
    address = translate_address(address);
    
    switch(address) {
        // PPU control register 1 - write only
        case 0x2000:
        // PPU control register 2 - write only
        case 0x2001:
        // Sprite memory address - write only
        case 0x2003:
        // sprite memory data - read/write
        case 0x2004:
        // Background control - write only  
        case 0x2005:
        // PPU memory address - write only
        case 0x2006:
        // PPU memory data - read/write 
        case 0x2007:
            ppu.write(data, address);
            break;
        
        // DMA access to sprite memory - write only 
        case 0x4014:
            //puts("DMA write to sprite memory");
            ppu.write_SPR_DMA(mem, data);
            break;
            
        // Joypad reset address
        case 0x4016:
            controller_1.write(data);
            break;
            
        default:
            mem[address] = data;
    }
}

void Mapper::write(const Byte* data, Word address, int length) {
    for(int i = 0; i < length; i++) write(data[i], address++);
}

// Memory locations 0x800, 0x1000, and 0x1800 mirror the 2MB at 0x0000 to 0x7FF
// 0x2000 to 0x3FFF mirror the 7 bytes at 0x2000 to 0x2007 (PPU registers)
inline Word Mapper::translate_address(Word address) const {    
    if(address < 0x2000) return address & 0x7FF;
    else if(address < 0x4000) return address & 0x2007;
    
    return address;
}
