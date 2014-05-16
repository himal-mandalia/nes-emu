// PPU Memory Map
// +---------+-------+-------+--------------------+
// | Address | Size  | Flags | Description        |
// +---------+-------+-------+--------------------+
// | $0000   | $1000 | C     | Pattern Table #0   |
// | $1000   | $1000 | C     | Pattern Table #1   |
// | $2000   | $3C0  |       | Name Table #0      |
// | $23C0   | $40   |  N    | Attribute Table #0 |
// | $2400   | $3C0  |  N    | Name Table #1      |
// | $27C0   | $40   |  N    | Attribute Table #1 |
// | $2800   | $3C0  |  N    | Name Table #2      |
// | $2BC0   | $40   |  N    | Attribute Table #2 |
// | $2C00   | $3C0  |  N    | Name Table #3      |
// | $2FC0   | $40   |  N    | Attribute Table #3 |
// | $3000   | $F00  |   R   |                    |
// | $3F00   | $10   |       | Image Palette #1   |
// | $3F10   | $10   |       | Sprite Palette #1  |
// | $3F20   | $E0   |    P  |                    |
// | $4000   | $C000 |     F |                    |
// +---------+-------+-------+--------------------+
//                     C = Possibly CHR-ROM
//                     N = Mirrored (see Subsection G)
//                     P = Mirrored (see Subsection H)
//                     R = Mirror of $2000-2EFF (VRAM)
//                     F = Mirror of $0000-3FFF (VRAM)

#ifndef PPU_H
#define PPU_H

#include "Constants.h"
#include "Memory.h"

const int VRAM_SIZE = 0x4000;
const int SPR_RAM_SIZE = 0x100;

const int SPRITE_8x8 = 0;
const int SPRITE_8x16 = 1;

const int PATTERN_TABLE_0 = 0x0000;
const int PATTERN_TABLE_1 = 0x1000;

const int NAMETABLE_0 = 0x2000;
const int NAMETABLE_1 = 0x2400;
const int NAMETABLE_2 = 0x2800;
const int NAMETABLE_3 = 0x2C00;
const int NAMETABLE_SIZE = 0x400;
const int ATTRIBUTE_TABLE_OFFSET = 0x3C0;
const int ATTRIBUTE_TABLE_SIZE = 0x40;

const int IMAGE_PALETTE = 0x3F00;
const int SPRITE_PALETTE = 0x3F10;

const int MAX_SCANLINE = 262;

class PPU {
private:
    // Table storing attribute byte lookup information
    Byte attribute_byte_table[960];
    // Table to storing attribute square lookup
    Byte attribute_square_table[960];
    
    // Main PPU memory
    Memory VRAM;
    
    // Sprite RAM
    Memory SPR_RAM;
    
    // Registers
    Byte PPU_Control_Reg_1;     // Mapped to 0x2000, write
    Byte PPU_Control_Reg_2;     // Mapped to 0x2001, write
    Byte PPU_Status_Reg;        // Mapped to 0x2002, read
    Byte SPR_RAM_Address_Reg;   // Mapped to 0x2003, write
    Byte SPR_RAM_IO_Reg;        // Mapped to 0x2004, write
    Byte VRAM_Address_Reg_1;    // Mapped to 0x2005, write x 2
    Byte VRAM_Address_Reg_2;    // Mapped to 0x2006, write x 2
    Byte VRAM_IO_Reg;           // Mapped to 0x2007, read/write
    
    Byte SPR_DMA_Reg;           // Mapped to 0x4014, write
    
    Byte NMI_on_VBlank;
    
    // Sprite size, 8x8 or 8x16
    Byte sprite_size;
    
    // Determines nametable address increment - 1 (horiz.) or 32 (vert.)
    Byte address_increment;
    
    Word VRAM_access_address;
    
    // Pattern tables
    Byte* background_pattern_table;
    Byte* sprite_pattern_table;
    
    // Nametables (used for background)
    Byte* nametable_0;
    Byte* nametable_1;
    Byte* nametable_2;
    Byte* nametable_3;
    
    // Nametable currently in use. Set via bits 0 and 1 of control reg 1
    Byte* current_nametable;
    
    // Determines background visibiliy
    bool render_background;
    // Determines sprite visibiliy
    bool render_sprites;
    
    // Indicates whether PPU is in VBlank currently
    bool VBlank;
    
    // comment required here...
    bool first_read;
    bool first_write;
    
    // Current scanline
    int scanline_count;
    
    // Nametable table row index for current tile
    int nametable_index;
    
    // Vertical offset of current tile
    int v_tile_offset;
    
    // temp - for testing, maybe unnecessary
    Byte framebuffer[240][256];
    
    void render_background_scanline();
    void render_sprite_frame();
    
    void write_PPU_Control_Reg_1(Byte data);
    void write_PPU_Control_Reg_2(Byte data);
    Byte read_PPU_Status_Reg();
    void write_SPR_RAM_Address_Reg(Byte data);
    void write_SPR_RAM(Byte data);
    void write_VRAM_Address_Reg_1(Byte data);
    void write_VRAM_Address_Reg_2(Byte data);
    Byte read_VRAM();
    void write_VRAM(Byte data);
    
    void set_current_nametable();
    
    void load_attribute_byte_table();
    void load_attribute_square_table();
    
public:
    PPU();
    
    const Byte (*(get_framebuffer)() const)[256] { return framebuffer; }
    
    void reset();
    void setup_mirroring(Byte mirroring);
    
    Byte read(Word address);
    void write(Byte data, Word address);
    void write_SPR_DMA(Memory &cpumem, Byte address);
    
    void emulate();
    bool VBlank_occurring();
    
    void load_CHR_bank(const Byte *chr);
};

#endif // PPU_H
