#ifndef MEMORY_H
#define MEMORY_H

#include "Constants.h"

class Memory {
private:
    Byte *mem;
    const unsigned int size;
    void clear() { memset(mem, 0, size); }
    
public:
    Memory(unsigned int _size);
    ~Memory();
    
    Byte& operator[](int subscript) { return mem[subscript]; }
    
    unsigned int mem_size() const { return size; }
    
    Byte read(Word address) const;
    Word read_word(Word address) const;
    void write(Byte data, Word address);
    void fast_write(const Byte* data, Word address, int length);
};

#endif // MEMORY_H
