#include "Memory.h"

Memory::Memory(unsigned int _size) : size(_size) {
    mem = new Byte[size]; clear();
}

Memory::~Memory() {
    if(mem) delete[] mem;
}

inline Byte Memory::read(Word address) const {
    return mem[address];
}

inline Word Memory::read_word(Word address) const {
    return ((Byte) mem[address] | ((Word) mem[address + 1] << 8));
}

inline void Memory::write(Byte data, Word address) {
    mem[address] = data;
}

void Memory::fast_write(
    const Byte* data, Word address, int length) {
    memcpy(mem + address, data, length);
}
