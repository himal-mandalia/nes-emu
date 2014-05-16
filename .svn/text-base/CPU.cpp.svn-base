#include "CPU.h"

CPU::CPU(Mapper &_mem) : mem(_mem) {
    // Load opcode_data table
    load_opcode_data();
}

void CPU::reset() {
    // Reset registers
    A = X = Y = 0x00;
    S = 0xFF; // Stack grows downward from 0xFF. Add 0x100
    
    // Reset flags
    N = V = B = D = I = Z = C = 0;
    U = 1;
    
    // Set to address contained in RESET handler routine
    PC = mem.read_word(RESET_VECTOR);
}

inline void CPU::handle_interrupt() {
    switch(interrupt) {
        case NMI: {
            //puts("NMI happening");

            stack_push_word(PC);
            stack_push(pack_flags());
            PC = mem.read_word(NMI_VECTOR);
            cycle_count += 7;
            break;
        }
        case RESET: {
            //puts("RESET happening");
            
            reset();
            cycle_count += 7;
            break;
        }
        case IRQ: {
            if(I != 0) {
                // Interrupts disabled (I flag set)
                break;
            }
            //puts("IRQ happening");
            
            stack_push_word(PC);
            stack_push(pack_flags());
            I = 1;
            B = 0;
            PC = mem.read_word(IRQ_VECTOR);
            cycle_count += 7;
            break;
        }
    }
    interrupt = -1;
}

inline Word CPU::get_operand_address(Byte opcode) {
    Word address = 0;
    
    // Determine addressing mode from opcode
    switch(opcode_data[opcode][ADDRESS_MODE]) {
        
        // operand is an 8 bit constant which follows the instruction
        case IMMEDIATE:
            address = PC;
            break;
 
        // 16 bit address pointing to operand   
        case ABSOLUTE:
            address = mem.read_word(PC);
            break;
            
        // Uses absolute (16 bit) address and adds X register   
        case ABSOLUTE_X:
            address = mem.read_word(PC);
            if((address & 0xFF00) != ((address + X) & 0xFF00))
                cycle_count++;
            address += X;
            break;
        
        // Same as above, except Y register used
        case ABSOLUTE_Y:
            address = mem.read_word(PC);
            if((address & 0xFF00) != ((address + Y) & 0xFF00))
                cycle_count++;
            address += Y;
            break;
            
        // 8 bit operand, from 0x00 to 0xFF in memory (zero page)   
        case ZERO_PAGE:
            address = mem.read(PC);
            break;
            
        // 8 bit address with X added. Wraps around
        case ZERO_PAGE_X:
            address = (mem.read(PC) + X) & 0xFF;
            break;
            
        // 8 bit address with Y added. Wraps around
        case ZERO_PAGE_Y:
            address = (mem.read(PC) + X) & 0xFF;
            break;
            
        //  Only supported by JMP
        case INDIRECT:
            address = mem.read_word(mem.read_word(PC));
            break;
            
        // like indirect, but add X
        case INDIRECT_X:
            address = mem.read(PC);
            if((address & 0xFF00) != ((address + X) & 0xFF00))
                cycle_count++;
            address = mem.read_word((address + X) & 0xFF);
            break;
            
        //  
        case INDIRECT_Y:
            address = mem.read_word(mem.read(PC));
            if((address & 0xFF00) != ((address + Y) & 0xFF00))
                cycle_count++;
            address += Y;
            break;
            
        // Sets address to PC + relative displacement (if branch taken)
        case RELATIVE:
            address = (((signed char) mem.read(PC)) + PC + 1);
            break;
            
        // Operate directly on Accumulator. Nice and easy, like Implied mode.   
        case ACCUMULATOR:
            break;
            
        // implied mode has no operands (target is implied by instruction)
        case IMPLIED:
            break;
    }
    
    // Wrap address and return
    return address & 0xFFFF;
}

long CPU::emulate(long cycles) {
    // Stores current opcode for decoding instruction
    Byte opcode;
    
    // Address of current operand
    Word address;
    
    while(cycles > 0) {
        // Reset extra cycles counter
        cycle_count = 0;
        
        // Dispatch interrupts
        handle_interrupt();
        
        // Read opcode from memory and increment program counter
        opcode = mem.read(PC++);
        
        // Get the operand address, and set page_crossed if page boundary crossed
        address = get_operand_address(opcode);
        
        // Increment PC by length of instruction's operand
        // (minus one due to previous increment)
        PC += (opcode_data[opcode][OP_LENGTH] - 1) & 0xFFFF;
        
        //printf("Address: %x\n", address);
        
        // Execute instructions
        
        switch(opcode_data[opcode][INSTRUCTION]) {
            
            // ADC - Add with Carry
            // Flags: C, Z, V, N
            
            case ADC: {
                //puts("ADC");
                short temp1 = A + mem.read(address) + C;
                C = temp1 > 0xFF ? 1 : 0;
                Z = temp1 & 0xFF;
                V = ((!(((A ^ mem.read(address)) & 0x80) != 0)
                    && (((A ^ temp1) & 0x80)) != 0) ? 1 : 0);
                N = (temp1 >> 7) & 1;
                A = temp1 & 0xFF;
                break;
            }
                
            // AND - Logical AND
            // Flags: Z, N
            
            case AND: {
                //puts("AND");
                short temp1 = A & mem.read(address);
                Z = temp1;
                N = (temp1 >> 7) & 1;
                A = temp1 & 0xFF;
                break;
            }   
                
            // ASL - Arithmetic Shift Left
            // Flags: C, Z, N
            
            case ASL: {
                //puts("ASL");
                if(opcode_data[opcode][ADDRESS_MODE] == ACCUMULATOR) {
                    C = (A >> 7) & 1;
                    A = (A << 1) & 0xFF;
                    Z = A;
                    N = (A >> 7) & 1;
                }
                else {
                    short temp1 = mem.read(address);
                    C = (temp1 >> 7) & 1;
                    temp1 = (temp1 << 1) & 0xFF;
                    Z = temp1;
                    N = (temp1 >> 7) & 1;
                    mem.write((temp1 & 0xFF), address);
                }
                break;
            }
                
            // BIT - Bit Test
            // Flags: Z, V, N
            
            case BIT: {
                //puts("BIT");
                short temp1 = mem.read(address);
                Z = temp1 & A;
                V = (temp1 >> 6) & 1;
                N = (temp1 >> 7) & 1;
                break;
            }
            
            // BPL - Branch if Positive
            // Flags: none
            
            case BPL: {
                //puts("BPL");
                if(N == 0) {
                    cycle_count += 
                    (((PC - opcode_data[opcode][OP_LENGTH]) & 0xFF00) 
                    != (address & 0xFF00));
                    cycle_count++;
                    PC = address;
                }
                break;
            }
                        
            // BMI - Branch if Minus
            // Flags: none
            
            case BMI: {
                //puts("BMI");
                if(N != 0) {
                    cycle_count += 
                    (((PC - opcode_data[opcode][OP_LENGTH]) & 0xFF00) 
                    != (address & 0xFF00));
                    cycle_count++;
                    PC = address;
                }
                break;
            }
                        
            // BVC - Branch if Overflow Clear
            // Flags: none
            
            case BVC: {
                //puts("BVC");
                if(V == 0) {
                    cycle_count += 
                    (((PC - opcode_data[opcode][OP_LENGTH]) & 0xFF00) 
                    != (address & 0xFF00));
                    cycle_count++;
                    PC = address;
                }
                break;
            }
                
            // BVS - Branch if Overflow Set
            // Flags: none
            
            case BVS: {
                //puts("BVS");
                if(V != 0) {
                    cycle_count += 
                    (((PC - opcode_data[opcode][OP_LENGTH]) & 0xFF00) 
                    != (address & 0xFF00));
                    cycle_count++;
                    PC = address;
                }
                break;
            }
                
            // BCC - Branch if Carry Clear
            // Flags: none
            
            case BCC: {
                //puts("BCC");
                if(C == 0) {
                    cycle_count += 
                    (((PC - opcode_data[opcode][OP_LENGTH]) & 0xFF00) 
                    != (address & 0xFF00));
                    cycle_count++;
                    PC = address;
                }
                break;
            }
                
            // BCS - Branch if Carry Set
            // Flags: none
            
            case BCS: {
                //puts("BCS");
                if(C != 0) {
                    cycle_count += 
                    (((PC - opcode_data[opcode][OP_LENGTH]) & 0xFF00) 
                    != (address & 0xFF00));
                    cycle_count++;
                    PC = address;
                }
                break;
            }   
                
            // BNE - Branch if Not Equal
            // Flags: none
            
            case BNE: {
                //puts("BNE");
                if(Z != 0) {
                    cycle_count += 
                    (((PC - opcode_data[opcode][OP_LENGTH]) & 0xFF00) 
                    != (address & 0xFF00));
                    cycle_count++;
                    PC = address;
                }
                break;
            }
                
            // BEQ - Branch if Equal
            // Flags: none
            
            case BEQ: {
                //puts("BEQ");
                if(Z == 0) {
                    cycle_count += 
                    (((PC - opcode_data[opcode][OP_LENGTH]) & 0xFF00) 
                    != (address & 0xFF00));
                    cycle_count++;
                    PC = address;
                }
                break;
            }   
                
            // BRK - Force Interrupt
            // Flags: B
            
            case BRK: {
                //puts("BRK");
                stack_push_word(PC + 1);
                B = 1; // Flags pushed with B set
                stack_push(pack_flags());
                I = 1;
                PC = mem.read_word(IRQ_VECTOR);
                break;
            }
                
            // CMP - Compare
            // Flags: Z, C, N
            
            case CMP: {
                //puts("CMP");
                short temp1 = A - mem.read(address);
                C = temp1 >= 0 ? 1 : 0;
                Z = temp1 & 0xFF;
                N = (temp1 >> 7) & 1;
                break;
            }   
                
            // CPX - Compare X Register
            // Flags: Z, C, N
            
            case CPX: {
                //puts("CPX");
                short temp1 = X - mem.read(address);
                C = temp1 >= 0 ? 1 : 0;
                Z = temp1 & 0xFF;
                N = (temp1 >> 7) & 1;
                break;
            }
                
            // CPY - Compare Y Register
            // Flags: Z, C, N
            
            case CPY: {
                //puts("CPY");
                short temp1 = Y - mem.read(address);
                C = temp1 >= 0 ? 1 : 0;
                Z = temp1 & 0xFF;
                N = (temp1 >> 7) & 1;
                break;
            }           

            // DEC - Decrement Memory
            // Flags: Z, N
            
            case DEC: {
                //puts("DEC");
                short temp1 = (mem.read(address) - 1) & 0xFF;
                Z = temp1;
                N = (temp1 >> 7) & 1;
                mem.write((temp1 & 0xFF), address);
                break;
            }       

            // EOR - Exclusive OR
            // Flags: Z, N
            
            case EOR: {
                //puts("EOR");
                A ^= mem.read(address) & 0xFF;
                Z = A;
                N = (A >> 7) & 1;
                break;
            }

            // CLC - Clear Carry Flag
            // Flags: C
            
            case CLC: {
                //puts("CLC");
                C = 0;
                break;
            }

            // SEC - Set Carry Flag
            // Flags: C

            case SEC: {
                //puts("SEC");
                C = 1;
                break;
            }
            
            // CLI - Clear Interrupt Disable
            // Flags: I
            
            case CLI: {
                //puts("CLI");
                I = 0;
                break;
            }       

            // SEI - Set Interrupt Disable
            // Flags: I
            
            case SEI: {
                //puts("SEI");
                I = 1;
                break;
            }           

            // CLV - Clear Overflow Flag
            // Flags: V
            
            case CLV: {
                //puts("CLV");
                V = 0;
                break;
            }
                
            // CLD - Clear Decimal Mode
            // Flags: D
            
            case CLD: {
                //puts("CLD");
                D = 0;
                break;
            }               

            // SED - Set Decimal Flag
            // Flags: D
            
            case SED: {
                //puts("SED");
                D = 1;
                break;
            }
                
            // INC - Increment Memory
            // Flags: Z, N
            
            case INC: {
                //puts("INC");
                short temp1 = (mem.read(address) + 1) &0xFF;
                Z = temp1;
                N = (temp1 >> 7) & 1;
                mem.write((temp1 & 0xFF), address);
                break;
            }           

            // JMP - Jump - check this
            // Flags: none
            
            case JMP: {
                //puts("JMP");
                PC = address;
                break;
            }
                
            // JSR - Jump to Subroutine - check this
            // Flags: none
            
            case JSR: {
                //puts("JSR");
                stack_push_word(PC - 1);
                PC = address;
                break;
            }               

            // LDA - Load Accumulator
            // Flags: Z, N
            
            case LDA: {
                //puts("LDA");
                A = mem.read(address);
                Z = A;
                N = (A >> 7) & 1; 
                break;
            }           

            // LDX - Load X Register
            // Flags: Z, N
            
            case LDX: {
                //puts("LDX");
                X = mem.read(address);
                Z = X;
                N = (X >> 7) & 1;
                break;
            }       

            // LDY - Load Y Register
            // Flags: Z, N
            
            case LDY: {
                //puts("LDY");
                Y = mem.read(address);
                Z = Y;
                N = (Y >> 7) & 1;
                break;
            }

            // LSR - Logical Shift Right
            // Flags: C, Z, N
            
            case LSR: {
                //puts("LSR");
                if(opcode_data[opcode][ADDRESS_MODE] == ACCUMULATOR) {
                    C = A & 1;
                    A = (A >> 1) & 0xFF;
                    Z = A;
                    N = (A >> 7) & 1;
                }
                else {
                    short temp1 = mem.read(address);
                    C = temp1 & 1;
                    temp1 = (temp1 >> 1) & 0xFF;
                    Z = temp1;
                    N = (temp1 >> 7) & 1;
                    mem.write((temp1 & 0xFF), address);
                }
                break;
            }
                
            // NOP - No Operation
            
            case NOP: {
                //puts("NOP");
                // Move along, nothing to see here...
                break;
            }       

            // ORA - Logical Inclusive OR
            // Flags: Z, N
            
            case ORA: {
                //puts("ORA");
                short temp1 = (A | mem.read(address)) & 0xFF;
                Z = temp1;
                N = (temp1 >> 7) & 1;
                A = temp1;
                break;
            }
                
            // TAX - Transfer Accumulator to X
            // Flags: Z, N
            
            case TAX: {
                //puts("TAX");
                X = A;
                Z = X;
                N = (X >> 7) & 1;
                break;
            }
                
            // TXA - Transfer X to Accumulator
            // Flags: Z, N
            
            case TXA: {
                //puts("TXA");
                A = X;
                Z = A;
                N = (A >> 7) & 1;
                break;
            }   

            // DEX - Decrement X Register
            // Flags: Z, N
            
            case DEX: {
                //puts("DEX");
                X = (X - 1) & 0xFF;
                Z = X;
                N = (X >> 7) & 1;
                break;
            }   

            // INX - Increment X Register
            // Flags: Z, N
            
            case INX: {
                //puts("INX");
                X = (X + 1) & 0xFF;
                Z = X;
                N = (X >> 7) & 1;
                break;
            }       

            // TAY - Transfer Accumulator to Y
            // Flags: Z, N
            
            case TAY: {
                //puts("TAY");
                Y = A;
                Z = Y;
                N = (Y >> 7) & 1;
                break;
            }
                
            // TYA - Transfer Y to Accumulator
            // Flags: Z, N
            
            case TYA: {
                //puts("TYA");
                A = Y;
                Z = A;
                N = (A >> 7) & 1;
                break;
            }
                
            // DEY - Decrement Y Register
            // Flags: Z, N
            
            case DEY: {
                //puts("DEY");
                Y = (Y - 1) & 0xFF;
                Z = Y;
                N = (Y >> 7) & 1;
                break;
            }
                
            // INY - Increment Y Register
            // Flags: Z, N
            
            case INY: {
                //puts("INY"); 
                Y = (Y + 1) & 0xFF;
                Z = Y;
                N = (Y >> 7) & 1;
                break;
            }           

            // ROL - Rotate Left
            // Flags: C, Z, N
            
            case ROL: {
                //puts("ROL");
                if(opcode_data[opcode][ADDRESS_MODE] == ACCUMULATOR) {
                    short temp1 = A;
                    short temp2 = C;
                    C = (temp1 >> 7) & 1;
                    temp1 = ((temp1 << 1) & 0xFF) + temp2;
                    A = temp1;
                    Z = A;
                    N = (A >> 7) & 1;
                }
                else {
                    short temp1 = mem.read(address);
                    short temp2 = C;
                    C = (temp1 >> 7) & 1;
                    temp1 = ((temp1 << 1) & 0xFF) + temp2;
                    Z = temp1;
                    N = (temp1 >> 7) & 1;
                    mem.write((temp1 & 0xFF), address);
                }
                break;
            }
                
            // ROR - Rotate Right
            // Flags: C, Z, N
            
            case ROR: {
                //puts("ROR");
                if(opcode_data[opcode][ADDRESS_MODE] == ACCUMULATOR) {
                    short temp1 = A;
                    short temp2 = C << 7;
                    C = temp1 & 1;
                    temp1 = (temp1 >> 1) + temp2;
                    A = temp1;
                    Z = A;
                    N = (A >> 7) & 1;
                }
                else {
                    short temp1 = mem.read(address);
                    short temp2 = C << 7;
                    C = temp1 & 1;
                    temp1 = (temp1 >> 1) + temp2;
                    Z = temp1;
                    N = (temp1 >> 7) & 1;
                    mem.write((temp1 & 0xFF), address);
                }
                break;
            }
                
            // RTI - Return from Interrupt
            // Flags: set from stack
            
            case RTI: {
                //puts("RTI");
                unpack_flags(stack_pull());
                PC = stack_pull_word() & 0xFFFF;
                break;
            }

            // RTS - Return from Subroutine
            // Flags: none
            
            case RTS: {
                //puts("RTS");
                // Check this, it isn't working properly
                PC = (stack_pull_word() + 1) & 0xFFFF;
                break;
            }       

            // SBC - Subtract with Carry
            // Flags: Z, C, N, V
            
            case SBC: {
                //puts("SBC");
                short temp1 = A - mem.read(address) - (1 - C);
                Z = temp1 & 0xFF;
                C = temp1 < 0 ? 0 : 1;
                N = (temp1 >> 7) & 1;
                V = (((((A ^ mem.read(address)) & 0x80) != 0)
                    && (((A ^ temp1) & 0x80)) != 0) ? 1 : 0);
                A = temp1 & 0xFF;   
                break;
            }
                
            // STA - Store Accumulator
            // Flags: none
            
            case STA: {
                //puts("STA");
                mem.write(A, address);
                break;
            }
                
            // STX - Store X Register
            // Flags: none
            
            case STX: {
                //puts("STX");
                mem.write(X, address);
                break;
            }
                
            // STY - Store Y Register
            // Flags: none
            
            case STY: {
                //puts("STY");
                mem.write(Y, address);
                break;
            }
                
            // TXS - Transfer X to Stack Pointer
            // Flags: none
            
            case TXS: {
                //puts("TXS");
                S = X;
                break;
            }
                
            // TSX - Transfer Stack Pointer to X
            // Flags: Z, N
            
            case TSX: {
                //puts("TSX");
                X = S;
                Z = X;
                N = (S >> 7) & 1;
                break;
            }   

            // PHA - Push Accumulator
            // Flags: none
            
            case PHA: {
                //puts("PHA");
                stack_push(A);
                break;
            }
                
            // PLA - Pull Accumulator
            // Flags: Z, N
            
            case PLA: {
                //puts("PLA");
                A = stack_pull();
                Z = A;
                N = (A >> 7) & 1;
                break;
            }

            // PHP - Push Processor Status
            // Flags: none
            
            case PHP: {
                //puts("PHP");
                stack_push(pack_flags());
                break;
            }   

            // PLP - Pull Processor Status
            // Flags: all (set from stack)
            
            case PLP: {
                //puts("PLP");
                unpack_flags(stack_pull());
                break;
            }   
            
            // Get out of here if we find a nutty opcode
             
            default: {
                // Maybe trigger a reset...
                printf("Invalid opcode: %X. Exiting...\n", opcode);
                exit(-1);
            }       
        }
        
        // Subtract the number of cycles used by the instruction + 
        // the extra cycles
        cycles -= (opcode_data[opcode][OP_TIME] + cycle_count);
        
        //print_regs();
    }
    return cycles;
}

inline Byte CPU::pack_flags() {
    return C
         | Z << 1 
         | I << 2 
         | D << 3 
         | B << 4 
         | U << 5 
         | V << 6 
         | N << 7;
}

inline void CPU::unpack_flags(Byte flags) {
    C = flags & 1;
    Z = (flags >> 1) & 1;
    I = (flags >> 2) & 1;
    D = (flags >> 3) & 1;
    B = (flags >> 4) & 1;
    U = (flags >> 5) & 1;
    V = (flags >> 6) & 1;
    N = (flags >> 7) & 1;
}

inline void CPU::stack_push(Byte data) {
    mem.write(data, (S-- & 0xFF) | 0x100);
}

inline Byte CPU::stack_pull() {
    return mem.read((++S & 0xFF) | 0x100);
}

inline void CPU::stack_push_word(Word data) {
    stack_push((Byte) (data >> 8));
    stack_push((Byte) (data & 0xFF));
}

inline Word CPU::stack_pull_word() {
    return ((Byte) stack_pull() | ((Word) stack_pull() << 8));
}

void CPU::print_regs() const {
    printf("\nRegs: A: %x\tX: %x\tY: %x\tSP: %x\t PC: %x\n", A, X, Y, S, PC);
    printf("Flags: N: %d\tV: %d\tU: %d\tB: %d\tD: %d\tI: %d\t Z: %d\tC: %d\n\n",
    N, V, U, B, D, I, Z ,C);
}

// There are 151 valid opcodes (out of a possible 256/0x100).
void CPU::load_opcode_data() {
    for(int i = 0; i < 0x100; i++) memset(opcode_data[i], 0xFF, 4);
    
    // BRK - IMPLIED
    opcode_data[0x00][INSTRUCTION]      = BRK;
    opcode_data[0x00][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x00][OP_LENGTH]        = 1;
    opcode_data[0x00][OP_TIME]          = 7;
    
    // ORA - INDIRECT X
    opcode_data[0x01][INSTRUCTION]      = ORA;
    opcode_data[0x01][ADDRESS_MODE]     = INDIRECT_X;
    opcode_data[0x01][OP_LENGTH]        = 2;
    opcode_data[0x01][OP_TIME]          = 6;
    
    // ORA - ZERO PAGE
    opcode_data[0x05][INSTRUCTION]      = ORA;
    opcode_data[0x05][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0x05][OP_LENGTH]        = 2;
    opcode_data[0x05][OP_TIME]          = 2;
    
    // ASL - ZERO PAGE
    opcode_data[0x06][INSTRUCTION]      = ASL;
    opcode_data[0x06][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0x06][OP_LENGTH]        = 2;
    opcode_data[0x06][OP_TIME]          = 5;
    
    // PHP - IMPLIED
    opcode_data[0x08][INSTRUCTION]      = PHP;
    opcode_data[0x08][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x08][OP_LENGTH]        = 1;
    opcode_data[0x08][OP_TIME]          = 3;
    
    // ORA - IMMEDIATE
    opcode_data[0x09][INSTRUCTION]      = ORA;
    opcode_data[0x09][ADDRESS_MODE]     = IMMEDIATE;
    opcode_data[0x09][OP_LENGTH]        = 2;
    opcode_data[0x09][OP_TIME]          = 2;
    
    // ASL - ACCUMULATOR
    opcode_data[0x0A][INSTRUCTION]      = ASL;
    opcode_data[0x0A][ADDRESS_MODE]     = ACCUMULATOR;
    opcode_data[0x0A][OP_LENGTH]        = 1;
    opcode_data[0x0A][OP_TIME]          = 2;
    
    // ORA - ABSOLUTE
    opcode_data[0x0D][INSTRUCTION]      = ORA;
    opcode_data[0x0D][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x0D][OP_LENGTH]        = 3;
    opcode_data[0x0D][OP_TIME]          = 4;
    
    // ASL - ABSOLUTE
    opcode_data[0x0E][INSTRUCTION]      = ASL;
    opcode_data[0x0E][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x0E][OP_LENGTH]        = 3;
    opcode_data[0x0E][OP_TIME]          = 6;
    
    // BPL - RELATIVE
    opcode_data[0x10][INSTRUCTION]      = BPL;
    opcode_data[0x10][ADDRESS_MODE]     = RELATIVE;
    opcode_data[0x10][OP_LENGTH]        = 2;
    opcode_data[0x10][OP_TIME]          = 2;
    
    // ORA - INDIRECT Y
    opcode_data[0x11][INSTRUCTION]      = ORA;
    opcode_data[0x11][ADDRESS_MODE]     = INDIRECT_Y;
    opcode_data[0x11][OP_LENGTH]        = 2;
    opcode_data[0x11][OP_TIME]          = 5; // 5+
    
    // ORA - ZERO PAGE X
    opcode_data[0x15][INSTRUCTION]      = ORA;
    opcode_data[0x15][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0x15][OP_LENGTH]        = 2;
    opcode_data[0x15][OP_TIME]          = 3;
    
    // ASL - ZERO PAGE X
    opcode_data[0x16][INSTRUCTION]      = ASL;
    opcode_data[0x16][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0x16][OP_LENGTH]        = 2;
    opcode_data[0x16][OP_TIME]          = 6;
    
    // CLC - IMPLIED
    opcode_data[0x18][INSTRUCTION]      = CLC;
    opcode_data[0x18][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x18][OP_LENGTH]        = 1;
    opcode_data[0x18][OP_TIME]          = 2;
    
    // ORA - ABSOLUTE Y
    opcode_data[0x19][INSTRUCTION]      = ORA;
    opcode_data[0x19][ADDRESS_MODE]     = ABSOLUTE_Y;
    opcode_data[0x19][OP_LENGTH]        = 3;
    opcode_data[0x19][OP_TIME]          = 4; // 4+
    
    // ORA - ABSOLUTE X
    opcode_data[0x1D][INSTRUCTION]      = ORA;
    opcode_data[0x1D][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0x1D][OP_LENGTH]        = 3;
    opcode_data[0x1D][OP_TIME]          = 4; // 4+
    
    // ASL - ABSOLUTE X
    opcode_data[0x1E][INSTRUCTION]      = ASL;
    opcode_data[0x1E][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0x1E][OP_LENGTH]        = 3;
    opcode_data[0x1E][OP_TIME]          = 7;
    
    // JSR - ABSOLUTE
    opcode_data[0x20][INSTRUCTION]      = JSR;
    opcode_data[0x20][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x20][OP_LENGTH]        = 3;
    opcode_data[0x20][OP_TIME]          = 6;
    
    // AND - INDIRECT X
    opcode_data[0x21][INSTRUCTION]      = AND;
    opcode_data[0x21][ADDRESS_MODE]     = INDIRECT_X;
    opcode_data[0x21][OP_LENGTH]        = 2;
    opcode_data[0x21][OP_TIME]          = 6;
    
    // BIT - ZERO PAGE
    opcode_data[0x24][INSTRUCTION]      = BIT;
    opcode_data[0x24][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0x24][OP_LENGTH]        = 2;
    opcode_data[0x24][OP_TIME]          = 3;
    
    // AND - ZERO PAGE
    opcode_data[0x25][INSTRUCTION]      = AND;
    opcode_data[0x25][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0x25][OP_LENGTH]        = 2;
    opcode_data[0x25][OP_TIME]          = 2;
    
    // ROL - ZERO PAGE
    opcode_data[0x26][INSTRUCTION]      = ROL;
    opcode_data[0x26][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0x26][OP_LENGTH]        = 2;
    opcode_data[0x26][OP_TIME]          = 5;
    
    // PLP - IMPLIED
    opcode_data[0x28][INSTRUCTION]      = PLP;
    opcode_data[0x28][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x28][OP_LENGTH]        = 1;
    opcode_data[0x28][OP_TIME]          = 4;
    
    // AND - IMMEDIATE
    opcode_data[0x29][INSTRUCTION]      = AND;
    opcode_data[0x29][ADDRESS_MODE]     = IMMEDIATE;
    opcode_data[0x29][OP_LENGTH]        = 2;
    opcode_data[0x29][OP_TIME]          = 2;
    
    // ROL - ACCUMULATOR
    opcode_data[0x2A][INSTRUCTION]      = ROL;
    opcode_data[0x2A][ADDRESS_MODE]     = ACCUMULATOR;
    opcode_data[0x2A][OP_LENGTH]        = 1;
    opcode_data[0x2A][OP_TIME]          = 2;
    
    // BIT - ABSOLUTE
    opcode_data[0x2C][INSTRUCTION]      = BIT;
    opcode_data[0x2C][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x2C][OP_LENGTH]        = 3;
    opcode_data[0x2C][OP_TIME]          = 4;
    
    // AND - ABSOLUTE
    opcode_data[0x2D][INSTRUCTION]      = AND;
    opcode_data[0x2D][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x2D][OP_LENGTH]        = 3;
    opcode_data[0x2D][OP_TIME]          = 4;
    
    // ROL - ABSOLUTE
    opcode_data[0x2E][INSTRUCTION]      = ROL;
    opcode_data[0x2E][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x2E][OP_LENGTH]        = 3;
    opcode_data[0x2E][OP_TIME]          = 6;
    
    // BMI - RELATIVE
    opcode_data[0x30][INSTRUCTION]      = BMI;
    opcode_data[0x30][ADDRESS_MODE]     = RELATIVE;
    opcode_data[0x30][OP_LENGTH]        = 2;
    opcode_data[0x30][OP_TIME]          = 2;
    
    // AND - INDIRECT Y
    opcode_data[0x31][INSTRUCTION]      = AND;
    opcode_data[0x31][ADDRESS_MODE]     = INDIRECT_Y;
    opcode_data[0x31][OP_LENGTH]        = 2;
    opcode_data[0x31][OP_TIME]          = 5; // 5+
    
    // AND - ZERO PAGE X
    opcode_data[0x35][INSTRUCTION]      = AND;
    opcode_data[0x35][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0x35][OP_LENGTH]        = 2;
    opcode_data[0x35][OP_TIME]          = 3;
    
    // ROL - ZERO PAGE X
    opcode_data[0x36][INSTRUCTION]      = ROL;
    opcode_data[0x36][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0x36][OP_LENGTH]        = 2;
    opcode_data[0x36][OP_TIME]          = 6;
    
    // SEC - IMPLIED
    opcode_data[0x38][INSTRUCTION]      = SEC;
    opcode_data[0x38][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x38][OP_LENGTH]        = 1;
    opcode_data[0x38][OP_TIME]          = 2;
    
    // AND - ABSOLUTE Y
    opcode_data[0x39][INSTRUCTION]      = AND;
    opcode_data[0x39][ADDRESS_MODE]     = ABSOLUTE_Y;
    opcode_data[0x39][OP_LENGTH]        = 3;
    opcode_data[0x39][OP_TIME]          = 4; // 4+
    
    // AND - ABSOLUTE X
    opcode_data[0x3D][INSTRUCTION]      = AND;
    opcode_data[0x3D][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0x3D][OP_LENGTH]        = 3;
    opcode_data[0x3D][OP_TIME]          = 4; // 4+
    
    // ROL - ABSOLUTE X
    opcode_data[0x3E][INSTRUCTION]      = ROL;
    opcode_data[0x3E][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0x3E][OP_LENGTH]        = 3;
    opcode_data[0x3E][OP_TIME]          = 7;
    
    // RTI - IMPLIED
    opcode_data[0x40][INSTRUCTION]      = RTI;
    opcode_data[0x40][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x40][OP_LENGTH]        = 1;
    opcode_data[0x40][OP_TIME]          = 6;
    
    // EOR - INDIRECT X
    opcode_data[0x41][INSTRUCTION]      = EOR;
    opcode_data[0x41][ADDRESS_MODE]     = INDIRECT_X;
    opcode_data[0x41][OP_LENGTH]        = 2;
    opcode_data[0x41][OP_TIME]          = 6;
    
    // EOR - ZERO PAGE
    opcode_data[0x45][INSTRUCTION]      = EOR;
    opcode_data[0x45][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0x45][OP_LENGTH]        = 2;
    opcode_data[0x45][OP_TIME]          = 3;
    
    // LSR - ZERO PAGE
    opcode_data[0x46][INSTRUCTION]      = LSR;
    opcode_data[0x46][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0x46][OP_LENGTH]        = 2;
    opcode_data[0x46][OP_TIME]          = 5;
    
    // PHA - IMPLIED
    opcode_data[0x48][INSTRUCTION]      = PHA;
    opcode_data[0x48][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x48][OP_LENGTH]        = 1;
    opcode_data[0x48][OP_TIME]          = 3;
    
    // EOR - IMMEDIATE
    opcode_data[0x49][INSTRUCTION]      = EOR;
    opcode_data[0x49][ADDRESS_MODE]     = IMMEDIATE;
    opcode_data[0x49][OP_LENGTH]        = 2;
    opcode_data[0x49][OP_TIME]          = 2;
    
    // LSR - ACCUMULATOR
    opcode_data[0x4A][INSTRUCTION]      = LSR;
    opcode_data[0x4A][ADDRESS_MODE]     = ACCUMULATOR;
    opcode_data[0x4A][OP_LENGTH]        = 1;
    opcode_data[0x4A][OP_TIME]          = 2;
    
    // JMP - ABSOLUTE
    opcode_data[0x4C][INSTRUCTION]      = JMP;
    opcode_data[0x4C][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0X4C][OP_LENGTH]        = 3;
    opcode_data[0x4C][OP_TIME]          = 3;
    
    // EOR - ABSOLUTE
    opcode_data[0x4D][INSTRUCTION]      = EOR;
    opcode_data[0x4D][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x4D][OP_LENGTH]        = 3;
    opcode_data[0x4D][OP_TIME]          = 4;
    
    // LSR - ABSOLUTE
    opcode_data[0x4E][INSTRUCTION]      = LSR;
    opcode_data[0x4E][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x4E][OP_LENGTH]        = 3;
    opcode_data[0x4E][OP_TIME]          = 6;
    
    // BVC - RELATIVE
    opcode_data[0x50][INSTRUCTION]      = BVC;
    opcode_data[0x50][ADDRESS_MODE]     = RELATIVE;
    opcode_data[0x50][OP_LENGTH]        = 2;
    opcode_data[0x50][OP_TIME]          = 2;
    
    // EOR - INDIRECT Y
    opcode_data[0x51][INSTRUCTION]      = EOR;
    opcode_data[0x51][ADDRESS_MODE]     = INDIRECT_Y;
    opcode_data[0x51][OP_LENGTH]        = 2;
    opcode_data[0x51][OP_TIME]          = 5; // 5+
    
    // EOR - ZERO PAGE X
    opcode_data[0x55][INSTRUCTION]      = EOR;
    opcode_data[0x55][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0x55][OP_LENGTH]        = 2;
    opcode_data[0x55][OP_TIME]          = 4;
    
    // LSR - ZERO PAGE X
    opcode_data[0x56][INSTRUCTION]      = LSR;
    opcode_data[0x56][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0x56][OP_LENGTH]        = 2;
    opcode_data[0x56][OP_TIME]          = 6;
    
    // CLI - IMPLIED
    opcode_data[0x58][INSTRUCTION]      = CLI;
    opcode_data[0x58][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x58][OP_LENGTH]        = 1;
    opcode_data[0x58][OP_TIME]          = 2;
    
    // EOR - ABSOLUTE Y
    opcode_data[0x59][INSTRUCTION]      = EOR;
    opcode_data[0x59][ADDRESS_MODE]     = ABSOLUTE_Y;
    opcode_data[0x59][OP_LENGTH]        = 3;
    opcode_data[0x59][OP_TIME]          = 4; // 4+
    
    // EOR - ABSOLUTE X
    opcode_data[0x5D][INSTRUCTION]      = EOR;
    opcode_data[0x5D][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0x5D][OP_LENGTH]        = 3;
    opcode_data[0x5D][OP_TIME]          = 4; // 4+
    
    // LSR - ABSOLUTE X
    opcode_data[0x5E][INSTRUCTION]      = LSR;
    opcode_data[0x5E][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0x5E][OP_LENGTH]        = 3;
    opcode_data[0x5E][OP_TIME]          = 7;
    
    // RTS - IMPLIED
    opcode_data[0x60][INSTRUCTION]      = RTS;
    opcode_data[0x60][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x60][OP_LENGTH]        = 1;
    opcode_data[0x60][OP_TIME]          = 6;
    
    // ADC - INDIRECT X
    opcode_data[0x61][INSTRUCTION]      = ADC;
    opcode_data[0x61][ADDRESS_MODE]     = INDIRECT_X;
    opcode_data[0x61][OP_LENGTH]        = 2;
    opcode_data[0x61][OP_TIME]          = 6;
    
    // ADC - ZERO PAGE
    opcode_data[0x65][INSTRUCTION]      = ADC;
    opcode_data[0x65][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0x65][OP_LENGTH]        = 2;
    opcode_data[0x65][OP_TIME]          = 3;
    
    // ROR - ZERO PAGE
    opcode_data[0x66][INSTRUCTION]      = ROR;
    opcode_data[0x66][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0x66][OP_LENGTH]        = 2;
    opcode_data[0x66][OP_TIME]          = 5;
    
    // PLA - IMPLIED
    opcode_data[0x68][INSTRUCTION]      = PLA;
    opcode_data[0x68][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x68][OP_LENGTH]        = 1;
    opcode_data[0x68][OP_TIME]          = 4;
    
    // ADC - IMMEDIATE
    opcode_data[0x69][INSTRUCTION]      = ADC;
    opcode_data[0x69][ADDRESS_MODE]     = IMMEDIATE;
    opcode_data[0x69][OP_LENGTH]        = 2;
    opcode_data[0x69][OP_TIME]          = 2;
    
    // ROR - ACCUMULATOR
    opcode_data[0x6A][INSTRUCTION]      = ROR;
    opcode_data[0x6A][ADDRESS_MODE]     = ACCUMULATOR;
    opcode_data[0x6A][OP_LENGTH]        = 1;
    opcode_data[0x6A][OP_TIME]          = 2;
    
    // JMP - INDIRECT
    opcode_data[0x6C][INSTRUCTION]      = JMP;
    opcode_data[0x6C][ADDRESS_MODE]     = INDIRECT;
    opcode_data[0x6C][OP_LENGTH]        = 3;
    opcode_data[0x6C][OP_TIME]          = 5;
    
    // ADC - ABSOLUTE
    opcode_data[0x6D][INSTRUCTION]      = ADC;
    opcode_data[0x6D][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x6D][OP_LENGTH]        = 3;
    opcode_data[0x6D][OP_TIME]          = 4;
    
    // ROR - ABSOLUTE
    opcode_data[0x6E][INSTRUCTION]      = ROR;
    opcode_data[0x6E][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x6E][OP_LENGTH]        = 3;
    opcode_data[0x6E][OP_TIME]          = 6;
    
    // BVS - RELATIVE
    opcode_data[0x70][INSTRUCTION]      = BVS;
    opcode_data[0x70][ADDRESS_MODE]     = RELATIVE;
    opcode_data[0x70][OP_LENGTH]        = 2;
    opcode_data[0x70][OP_TIME]          = 2;
    
    // ADC - INDIRECT Y
    opcode_data[0x71][INSTRUCTION]      = ADC;
    opcode_data[0x71][ADDRESS_MODE]     = INDIRECT_Y;
    opcode_data[0x71][OP_LENGTH]        = 2;
    opcode_data[0x71][OP_TIME]          = 5; // 5+
    
    // ADC - ZERO PAGE X
    opcode_data[0x75][INSTRUCTION]      = ADC;
    opcode_data[0x75][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0x75][OP_LENGTH]        = 2;
    opcode_data[0x75][OP_TIME]          = 4;
    
    // ROR - ZERO PAGE X
    opcode_data[0x76][INSTRUCTION]      = ROR;
    opcode_data[0x76][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0x76][OP_LENGTH]        = 2;
    opcode_data[0x76][OP_TIME]          = 6;
    
    // SEI - IMPLIED
    opcode_data[0x78][INSTRUCTION]      = SEI;
    opcode_data[0x78][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x78][OP_LENGTH]        = 1;
    opcode_data[0x78][OP_TIME]          = 2;
    
    // ADC - ABSOLUTE Y
    opcode_data[0x79][INSTRUCTION]      = ADC;
    opcode_data[0x79][ADDRESS_MODE]     = ABSOLUTE_Y;
    opcode_data[0x79][OP_LENGTH]        = 3;
    opcode_data[0x79][OP_TIME]          = 4; // 4+
    
    // ADC - ABSOLUTE X
    opcode_data[0x7D][INSTRUCTION]      = ADC;
    opcode_data[0x7D][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0x7D][OP_LENGTH]        = 3;
    opcode_data[0x7D][OP_TIME]          = 4; // 4+
    
    // ROR - ABSOLUTE X
    opcode_data[0x7E][INSTRUCTION]      = ROR;
    opcode_data[0x7E][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0x7E][OP_LENGTH]        = 3;
    opcode_data[0x7E][OP_TIME]          = 7;
    
    // STA - INDIRECT X
    opcode_data[0x81][INSTRUCTION]      = STA;
    opcode_data[0x81][ADDRESS_MODE]     = INDIRECT_X;
    opcode_data[0x81][OP_LENGTH]        = 2;
    opcode_data[0x81][OP_TIME]          = 6;
    
    // STY - ZERO PAGE
    opcode_data[0x84][INSTRUCTION]      = STY;
    opcode_data[0x84][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0x84][OP_LENGTH]        = 2;
    opcode_data[0x84][OP_TIME]          = 3;
    
    // STA - ZERO PAGE
    opcode_data[0x85][INSTRUCTION]      = STA;
    opcode_data[0x85][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0x85][OP_LENGTH]        = 2;
    opcode_data[0x85][OP_TIME]          = 3;
    
    // STX - ZERO PAGE
    opcode_data[0x86][INSTRUCTION]      = STX;
    opcode_data[0x86][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0x86][OP_LENGTH]        = 2;
    opcode_data[0x86][OP_TIME]          = 3;
    
    // DEY - IMPLIED
    opcode_data[0x88][INSTRUCTION]      = DEY;
    opcode_data[0x88][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x88][OP_LENGTH]        = 1;
    opcode_data[0x88][OP_TIME]          = 2;
    
    // TXA - IMPLIED
    opcode_data[0x8A][INSTRUCTION]      = TXA;
    opcode_data[0x8A][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x8A][OP_LENGTH]        = 1;
    opcode_data[0x8A][OP_TIME]          = 2;
    
    // STY - ABSOLUTE
    opcode_data[0x8C][INSTRUCTION]      = STY;
    opcode_data[0x8C][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x8C][OP_LENGTH]        = 3;
    opcode_data[0x8C][OP_TIME]          = 4;
    
    // STA - ABSOLUTE
    opcode_data[0x8D][INSTRUCTION]      = STA;
    opcode_data[0x8D][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x8D][OP_LENGTH]        = 3;
    opcode_data[0x8D][OP_TIME]          = 4;
    
    // STX - ABSOLUTE
    opcode_data[0x8E][INSTRUCTION]      = STX;
    opcode_data[0x8E][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0x8E][OP_LENGTH]        = 3;
    opcode_data[0x8E][OP_TIME]          = 4;
    
    // BCC - RELATIVE
    opcode_data[0x90][INSTRUCTION]      = BCC;
    opcode_data[0x90][ADDRESS_MODE]     = RELATIVE;
    opcode_data[0x90][OP_LENGTH]        = 2;
    opcode_data[0x90][OP_TIME]          = 2;
    
    // STA - INDIRECT Y
    opcode_data[0x91][INSTRUCTION]      = STA;
    opcode_data[0x91][ADDRESS_MODE]     = INDIRECT_Y;
    opcode_data[0x91][OP_LENGTH]        = 2;
    opcode_data[0x91][OP_TIME]          = 6;
    
    // STY - ZERO PAGE X
    opcode_data[0x94][INSTRUCTION]      = STY;
    opcode_data[0x94][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0x94][OP_LENGTH]        = 2;
    opcode_data[0x94][OP_TIME]          = 4;
    
    // STA - ZERO PAGE X
    opcode_data[0x95][INSTRUCTION]      = STA;
    opcode_data[0x95][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0x95][OP_LENGTH]        = 2;
    opcode_data[0x95][OP_TIME]          = 4;
    
    // STX - ZERO PAGE Y
    opcode_data[0x96][INSTRUCTION]      = STX;
    opcode_data[0x96][ADDRESS_MODE]     = ZERO_PAGE_Y;
    opcode_data[0x96][OP_LENGTH]        = 2;
    opcode_data[0x96][OP_TIME]          = 4;
    
    // TYA - IMPLIED
    opcode_data[0x98][INSTRUCTION]      = TYA;
    opcode_data[0x98][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x98][OP_LENGTH]        = 1;
    opcode_data[0x98][OP_TIME]          = 2;
    
    // STA - ABSOLUTE Y
    opcode_data[0x99][INSTRUCTION]      = STA;
    opcode_data[0x99][ADDRESS_MODE]     = ABSOLUTE_Y;
    opcode_data[0x99][OP_LENGTH]        = 3;
    opcode_data[0x99][OP_TIME]          = 5;
    
    // TXS - IMPLIED
    opcode_data[0x9A][INSTRUCTION]      = TXS;
    opcode_data[0x9A][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0x9A][OP_LENGTH]        = 1;
    opcode_data[0x9A][OP_TIME]          = 2;
    
    // STA - ABSOLUTE X
    opcode_data[0x9D][INSTRUCTION]      = STA;
    opcode_data[0x9D][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0x9D][OP_LENGTH]        = 3;
    opcode_data[0x9D][OP_TIME]          = 5;
    
    // LDY - IMMEDIATE
    opcode_data[0xA0][INSTRUCTION]      = LDY;
    opcode_data[0xA0][ADDRESS_MODE]     = IMMEDIATE;
    opcode_data[0xA0][OP_LENGTH]        = 2;
    opcode_data[0xA0][OP_TIME]          = 2;
    
    // LDA - INDIRECT X
    opcode_data[0xA1][INSTRUCTION]      = LDA;
    opcode_data[0xA1][ADDRESS_MODE]     = INDIRECT_X;
    opcode_data[0xA1][OP_LENGTH]        = 2;
    opcode_data[0xA1][OP_TIME]          = 6;
    
    // LDX - IMMEDIATE
    opcode_data[0xA2][INSTRUCTION]      = LDX;
    opcode_data[0xA2][ADDRESS_MODE]     = IMMEDIATE;
    opcode_data[0xA2][OP_LENGTH]        = 2;
    opcode_data[0xA2][OP_TIME]          = 2;
    
    // LDY - ZERO PAGE
    opcode_data[0xA4][INSTRUCTION]      = LDY;
    opcode_data[0xA4][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0xA4][OP_LENGTH]        = 2;
    opcode_data[0xA4][OP_TIME]          = 3;
    
    // LDA - ZERO PAGE
    opcode_data[0xA5][INSTRUCTION]      = LDA;
    opcode_data[0xA5][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0xA5][OP_LENGTH]        = 2;
    opcode_data[0xA5][OP_TIME]          = 3;
    
    // LDX - ZERO PAGE
    opcode_data[0xA6][INSTRUCTION]      = LDX;
    opcode_data[0xA6][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0xA6][OP_LENGTH]        = 2;
    opcode_data[0xA6][OP_TIME]          = 3;
    
    // TAY - IMPLIED
    opcode_data[0xA8][INSTRUCTION]      = TAY;
    opcode_data[0xA8][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0xA8][OP_LENGTH]        = 1;
    opcode_data[0xA8][OP_TIME]          = 2;
    
    // LDA - IMMEDIATE
    opcode_data[0xA9][INSTRUCTION]      = LDA;
    opcode_data[0xA9][ADDRESS_MODE]     = IMMEDIATE;
    opcode_data[0xA9][OP_LENGTH]        = 2;
    opcode_data[0xA9][OP_TIME]          = 2;
    
    // TAX - IMPLIED
    opcode_data[0xAA][INSTRUCTION]      = TAX;
    opcode_data[0xAA][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0xAA][OP_LENGTH]        = 1;
    opcode_data[0xAA][OP_TIME]          = 2;
    
    // LDY - ABSOLUTE
    opcode_data[0xAC][INSTRUCTION]      = LDY;
    opcode_data[0xAC][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0xAC][OP_LENGTH]        = 3;
    opcode_data[0xAC][OP_TIME]          = 4;
    
    // LDA - ABSOLUTE
    opcode_data[0xAD][INSTRUCTION]      = LDA;
    opcode_data[0xAD][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0xAD][OP_LENGTH]        = 3;
    opcode_data[0xAD][OP_TIME]          = 4;
    
    // LDX - ABSOLUTE
    opcode_data[0xAE][INSTRUCTION]      = LDX;
    opcode_data[0xAE][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0xAE][OP_LENGTH]        = 3;
    opcode_data[0xAE][OP_TIME]          = 4;
    
    // BCS - RELATIVE
    opcode_data[0xB0][INSTRUCTION]      = BCS;
    opcode_data[0xB0][ADDRESS_MODE]     = RELATIVE;
    opcode_data[0xB0][OP_LENGTH]        = 2;
    opcode_data[0xB0][OP_TIME]          = 2;
    
    // LDA - INDIRECT Y
    opcode_data[0xB1][INSTRUCTION]      = LDA;
    opcode_data[0xB1][ADDRESS_MODE]     = INDIRECT_Y;
    opcode_data[0xB1][OP_LENGTH]        = 2;
    opcode_data[0xB1][OP_TIME]          = 5; // 5+
    
    // LDY - ZERO PAGE X
    opcode_data[0xB4][INSTRUCTION]      = LDY;
    opcode_data[0xB4][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0xB4][OP_LENGTH]        = 2;
    opcode_data[0xB4][OP_TIME]          = 4;
    
    // LDA - ZERO PAGE X
    opcode_data[0xB5][INSTRUCTION]      = LDA;
    opcode_data[0xB5][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0xB5][OP_LENGTH]        = 2;
    opcode_data[0xB5][OP_TIME]          = 4;
    
    // LDX - ZERO PAGE Y
    opcode_data[0xB6][INSTRUCTION]      = LDX;
    opcode_data[0xB6][ADDRESS_MODE]     = ZERO_PAGE_Y;
    opcode_data[0xB6][OP_LENGTH]        = 2;
    opcode_data[0xB6][OP_TIME]          = 4;
    
    // CLV - IMPLIED
    opcode_data[0xB8][INSTRUCTION]      = CLV;
    opcode_data[0xB8][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0xB8][OP_LENGTH]        = 1;
    opcode_data[0xB8][OP_TIME]          = 2;
    
    // LDA - ABSOLUTE Y
    opcode_data[0xB9][INSTRUCTION]      = LDA;
    opcode_data[0xB9][ADDRESS_MODE]     = ABSOLUTE_Y;
    opcode_data[0xB9][OP_LENGTH]        = 3;
    opcode_data[0xB9][OP_TIME]          = 4; // 4+
    
    // TSX - IMPLIED
    opcode_data[0xBA][INSTRUCTION]      = TSX;
    opcode_data[0xBA][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0xBA][OP_LENGTH]        = 1;
    opcode_data[0xBA][OP_TIME]          = 2;
    
    // LDY - ABSOLUTE X
    opcode_data[0xBC][INSTRUCTION]      = LDY;
    opcode_data[0xBC][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0xBC][OP_LENGTH]        = 3;
    opcode_data[0xBC][OP_TIME]          = 4; // 4+
    
    // LDA - ABSOLUTE X
    opcode_data[0xBD][INSTRUCTION]      = LDA;
    opcode_data[0xBD][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0xBD][OP_LENGTH]        = 3;
    opcode_data[0xBD][OP_TIME]          = 4; // 4+
    
    // LDX - ABSOLUTE Y
    opcode_data[0xBE][INSTRUCTION]      = LDX;
    opcode_data[0xBE][ADDRESS_MODE]     = ABSOLUTE_Y;
    opcode_data[0xBE][OP_LENGTH]        = 3;
    opcode_data[0xBE][OP_TIME]          = 4; // 4+
    
    // CPY - IMMEDIATE
    opcode_data[0xC0][INSTRUCTION]      = CPY;
    opcode_data[0xC0][ADDRESS_MODE]     = IMMEDIATE;
    opcode_data[0xC0][OP_LENGTH]        = 2;
    opcode_data[0xC0][OP_TIME]          = 2;
    
    // CMP - INDIRECT X
    opcode_data[0xC1][INSTRUCTION]      = CMP;
    opcode_data[0xC1][ADDRESS_MODE]     = INDIRECT_X;
    opcode_data[0xC1][OP_LENGTH]        = 2;
    opcode_data[0xC1][OP_TIME]          = 6;
    
    // CPY - ZERO PAGE
    opcode_data[0xC4][INSTRUCTION]      = CPY;
    opcode_data[0xC4][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0xC4][OP_LENGTH]        = 2;
    opcode_data[0xC4][OP_TIME]          = 3;
    
    // CMP - ZERO PAGE
    opcode_data[0xC5][INSTRUCTION]      = CMP;
    opcode_data[0xC5][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0xC5][OP_LENGTH]        = 2;
    opcode_data[0xC5][OP_TIME]          = 3;
    
    // DEC - ZERO PAGE
    opcode_data[0xC6][INSTRUCTION]      = DEC;
    opcode_data[0xC6][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0xC6][OP_LENGTH]        = 2;
    opcode_data[0xC6][OP_TIME]          = 5;
    
    // INY - IMPLIED
    opcode_data[0xC8][INSTRUCTION]      = INY;
    opcode_data[0xC8][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0xC8][OP_LENGTH]        = 1;
    opcode_data[0xC8][OP_TIME]          = 2;
    
    // CMP - IMMEDIATE
    opcode_data[0xC9][INSTRUCTION]      = CMP;
    opcode_data[0xC9][ADDRESS_MODE]     = IMMEDIATE;
    opcode_data[0xC9][OP_LENGTH]        = 2;
    opcode_data[0xC9][OP_TIME]          = 2;
    
    // DEX - IMPLIED
    opcode_data[0xCA][INSTRUCTION]      = DEX;
    opcode_data[0xCA][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0xCA][OP_LENGTH]        = 1;
    opcode_data[0xCA][OP_TIME]          = 2;
    
    // CPY - ABSOLUTE
    opcode_data[0xCC][INSTRUCTION]      = CPY;
    opcode_data[0xCC][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0xCC][OP_LENGTH]        = 3;
    opcode_data[0xCC][OP_TIME]          = 4;
    
    // CMP - ABSOLUTE
    opcode_data[0xCD][INSTRUCTION]      = CMP;
    opcode_data[0xCD][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0xCD][OP_LENGTH]        = 3;
    opcode_data[0xCD][OP_TIME]          = 4;
    
    // DEC - ABSOLUTE
    opcode_data[0xCE][INSTRUCTION]      = DEC;
    opcode_data[0xCE][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0xCE][OP_LENGTH]        = 3;
    opcode_data[0xCE][OP_TIME]          = 6;
    
    // BNE - RELATIVE
    opcode_data[0xD0][INSTRUCTION]      = BNE;
    opcode_data[0xD0][ADDRESS_MODE]     = RELATIVE;
    opcode_data[0xD0][OP_LENGTH]        = 2;
    opcode_data[0xD0][OP_TIME]          = 2;
    
    // CMP - INDIRECT Y
    opcode_data[0xD1][INSTRUCTION]      = CMP;
    opcode_data[0xD1][ADDRESS_MODE]     = INDIRECT_Y;
    opcode_data[0xD1][OP_LENGTH]        = 2;
    opcode_data[0xD1][OP_TIME]          = 5; // 5+
    
    // CMP - ZERO PAGE X
    opcode_data[0xD5][INSTRUCTION]      = CMP;
    opcode_data[0xD5][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0xD5][OP_LENGTH]        = 2;
    opcode_data[0xD5][OP_TIME]          = 4;
    
    // DEC - ZERO PAGE X
    opcode_data[0xD6][INSTRUCTION]      = DEC;
    opcode_data[0xD6][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0xD6][OP_LENGTH]        = 2;
    opcode_data[0xD6][OP_TIME]          = 6;
    
    // CLD - IMPLIED
    opcode_data[0xD8][INSTRUCTION]      = CLD;
    opcode_data[0xD8][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0xD8][OP_LENGTH]        = 1;
    opcode_data[0xD8][OP_TIME]          = 2;
    
    // CMP - ABSOLUTE Y
    opcode_data[0xD9][INSTRUCTION]      = CMP;
    opcode_data[0xD9][ADDRESS_MODE]     = ABSOLUTE_Y;
    opcode_data[0xD9][OP_LENGTH]        = 3;
    opcode_data[0xD9][OP_TIME]          = 4; // 4+
    
    // CMP - ABSOLUTE X
    opcode_data[0xDD][INSTRUCTION]      = CMP;
    opcode_data[0xDD][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0xDD][OP_LENGTH]        = 3;
    opcode_data[0xDD][OP_TIME]          = 4; // 4+
    
    // DEC - ABSOLUTE X
    opcode_data[0xDE][INSTRUCTION]      = DEC;
    opcode_data[0xDE][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0xDE][OP_LENGTH]        = 3;
    opcode_data[0xDE][OP_TIME]          = 7;
    
    // CPX - IMMEDIATE
    opcode_data[0xE0][INSTRUCTION]      = CPX;
    opcode_data[0xE0][ADDRESS_MODE]     = IMMEDIATE;
    opcode_data[0xE0][OP_LENGTH]        = 2;
    opcode_data[0xE0][OP_TIME]          = 2;
    
    // SBC - INDIRECT X
    opcode_data[0xE1][INSTRUCTION]      = SBC;
    opcode_data[0xE1][ADDRESS_MODE]     = INDIRECT_X;
    opcode_data[0xE1][OP_LENGTH]        = 2;
    opcode_data[0xE1][OP_TIME]          = 6;
    
    // CPX - ZERO PAGE
    opcode_data[0xE4][INSTRUCTION]      = CPX;
    opcode_data[0xE4][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0xE4][OP_LENGTH]        = 2;
    opcode_data[0xE4][OP_TIME]          = 3;
    
    // SBC - ZERO PAGE
    opcode_data[0xE5][INSTRUCTION]      = SBC;
    opcode_data[0xE5][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0xE5][OP_LENGTH]        = 2;
    opcode_data[0xE5][OP_TIME]          = 3;
    
    // INC - ZERO PAGE
    opcode_data[0xE6][INSTRUCTION]      = INC;
    opcode_data[0xE6][ADDRESS_MODE]     = ZERO_PAGE;
    opcode_data[0xE6][OP_LENGTH]        = 2;
    opcode_data[0xE6][OP_TIME]          = 5;
    
    // INX - IMPLIED
    opcode_data[0xE8][INSTRUCTION]      = INX;
    opcode_data[0xE8][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0xE8][OP_LENGTH]        = 1;
    opcode_data[0xE8][OP_TIME]          = 2;
    
    // SBC - IMMEDIATE
    opcode_data[0xE9][INSTRUCTION]      = SBC;
    opcode_data[0xE9][ADDRESS_MODE]     = IMMEDIATE;
    opcode_data[0xE9][OP_LENGTH]        = 2;
    opcode_data[0xE9][OP_TIME]          = 2;
    
    // NOP - IMPLIED
    opcode_data[0xEA][INSTRUCTION]      = NOP;
    opcode_data[0xEA][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0xEA][OP_LENGTH]        = 1;
    opcode_data[0xEA][OP_TIME]          = 2;
    
    // CPX - ABSOLUTE
    opcode_data[0xEC][INSTRUCTION]      = CPX;
    opcode_data[0xEC][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0xEC][OP_LENGTH]        = 3;
    opcode_data[0xEC][OP_TIME]          = 4;
    
    // SBC - ABSOLUTE
    opcode_data[0xED][INSTRUCTION]      = SBC;
    opcode_data[0xED][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0xED][OP_LENGTH]        = 3;
    opcode_data[0xED][OP_TIME]          = 4;
    
    // INC - ABSOLUTE
    opcode_data[0xEE][INSTRUCTION]      = INC;
    opcode_data[0xEE][ADDRESS_MODE]     = ABSOLUTE;
    opcode_data[0xEE][OP_LENGTH]        = 3;
    opcode_data[0xEE][OP_TIME]          = 6;
    
    // BEQ - RELATIVE
    opcode_data[0xF0][INSTRUCTION]      = BEQ;
    opcode_data[0xF0][ADDRESS_MODE]     = RELATIVE;
    opcode_data[0xF0][OP_LENGTH]        = 2;
    opcode_data[0xF0][OP_TIME]          = 2;
    
    // SBC - INDIRECT Y
    opcode_data[0xF1][INSTRUCTION]      = SBC;
    opcode_data[0xF1][ADDRESS_MODE]     = INDIRECT_Y;
    opcode_data[0xF1][OP_LENGTH]        = 2;
    opcode_data[0xF1][OP_TIME]          = 5; // 5+
    
    // SBC - ZERO PAGE X
    opcode_data[0xF5][INSTRUCTION]      = SBC;
    opcode_data[0xF5][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0xF5][OP_LENGTH]        = 2;
    opcode_data[0xF5][OP_TIME]          = 4;
    
    // INC - ZERO PAGE X
    opcode_data[0xF6][INSTRUCTION]      = INC;
    opcode_data[0xF6][ADDRESS_MODE]     = ZERO_PAGE_X;
    opcode_data[0xF6][OP_LENGTH]        = 2;
    opcode_data[0xF6][OP_TIME]          = 6;
    
    // SED - IMPLIED
    opcode_data[0xF8][INSTRUCTION]      = SED;
    opcode_data[0xF8][ADDRESS_MODE]     = IMPLIED;
    opcode_data[0xF8][OP_LENGTH]        = 1;
    opcode_data[0xF8][OP_TIME]          = 2;
    
    // SBC - ABSOLUTE Y
    opcode_data[0xF9][INSTRUCTION]      = SBC;
    opcode_data[0xF9][ADDRESS_MODE]     = ABSOLUTE_Y;
    opcode_data[0xF9][OP_LENGTH]        = 3;
    opcode_data[0xF9][OP_TIME]          = 4; // 4+
    
    // SBC - ABSOLUTE X
    opcode_data[0xFD][INSTRUCTION]      = SBC;
    opcode_data[0xFD][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0xFD][OP_LENGTH]        = 3;
    opcode_data[0xFD][OP_TIME]          = 4; // 4+
    
    // INC - ABSOLUTE X
    opcode_data[0xFE][INSTRUCTION]      = INC;
    opcode_data[0xFE][ADDRESS_MODE]     = ABSOLUTE_X;
    opcode_data[0xFE][OP_LENGTH]        = 3;
    opcode_data[0xFE][OP_TIME]          = 7;
    
    /*for(int i = 0; i < 0x100; i++) {
        if(opcode_data[i][0] != 0xFF) {
            printf("0x%02X %02d %02d %d %d\n", i, opcode_data[i][0], opcode_data[i][1],
            opcode_data[i][2], opcode_data[i][3]);
        }
    }*/
}
