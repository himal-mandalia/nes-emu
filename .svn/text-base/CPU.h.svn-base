// MOS 6502 CPU emulator
//
// Does NOT support Binary Coded Decimal (BCD) mode.
// Mode is not used on the NES, setting/clearing D flag will no effect.


#ifndef CPU_H
#define CPU_H

#include "Constants.h"
#include "Mapper.h"
#include "PPU.h"

// Interrupt types
const int NMI           = 0;
const int RESET         = 1;
const int IRQ           = 2;

// Addresses of interrupt routines
const Word NMI_VECTOR    = 0xFFFA;
const Word RESET_VECTOR  = 0xFFFC;
const Word IRQ_VECTOR    = 0xFFFE;

class CPU {
private:
    // opcode_data table index
    enum {
        INSTRUCTION,
        ADDRESS_MODE,
        OP_LENGTH,
        OP_TIME
    };

    // Addressing modes
    enum {
        IMMEDIATE,
        ABSOLUTE,
        ZERO_PAGE,
        IMPLIED,
        INDIRECT,
        ABSOLUTE_X,
        ABSOLUTE_Y,
        ZERO_PAGE_X,
        ZERO_PAGE_Y,
        INDIRECT_X,
        INDIRECT_Y,
        RELATIVE,
        ACCUMULATOR
    };

    // Instructions (56 total)
    enum {
        ADC, AND, ASL, BCC, BCS, BEQ, BIT,
        BMI, BNE, BPL, BRK, BVC, BVS, CLC,
        CLD, CLI, CLV, CMP, CPX, CPY, DEC,
        DEX, DEY, EOR, INC, INX, INY, JMP,
        JSR, LDA, LDX, LDY, LSR, NOP, ORA,
        PHA, PHP, PLA, PLP, ROL, ROR, RTI,
        RTS, SBC, SEC, SED, SEI, STA, STX,
        STY, TAX, TAY, TSX, TXA, TXS, TYA
    };
    
    // Memory mapper
    Mapper &mem;
    
    // Opcode information table
    Byte opcode_data[0x100][4];
    
    // Registers
    Byte A;     // Accumulator
    Byte X;     // X index register
    Byte Y;     // Y index register
    Byte S;     // Stack Pointer
    Word PC;    // 16-bit program counter
    
    // Flags
    Flag N;     // Negative
    Flag V;     // oVerflow
    Flag U;     // Unused
    Flag B;     // Break
    Flag D;     // Decimal - not used on the NES
    Flag I;     // Interrupt
    Flag Z;     // Zero
    Flag C;     // Carry
    
    // Keep track of cycles to add
    unsigned int cycle_count;
    
    // Current interrupt type
    int interrupt;
    
    void handle_interrupt();
    
    Word get_operand_address(Byte opcode);
    
    void stack_push(Byte data);
    
    Byte stack_pull();
    
    void stack_push_word(Word data);
    
    Word stack_pull_word();
    
    Byte pack_flags();
    
    void unpack_flags(Byte flags);
    
    void load_opcode_data();
    
    void print_regs() const;
    
public:
    CPU(Mapper &_mem);
    
    void reset();
    
    long emulate(long cycles);
    
    void set_interrupt(int i) { interrupt = i; }
};

#endif // CPU_H
