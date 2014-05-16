#include "PPU.h"

PPU::PPU() : VRAM(VRAM_SIZE), SPR_RAM(SPR_RAM_SIZE) {
    
}

void PPU::load_CHR_bank(const Byte* chr) {
    memcpy(&VRAM[0], chr, CHR_BANK_SIZE);
    
    load_attribute_byte_table();
    load_attribute_square_table();
}

void PPU::reset() {
    PPU_Control_Reg_1       = 0;
    PPU_Control_Reg_2       = 0;
    PPU_Status_Reg          = 0;
    SPR_RAM_Address_Reg     = 0;
    SPR_RAM_IO_Reg          = 0;
    VRAM_Address_Reg_1      = 0;
    VRAM_Address_Reg_2      = 0;
    VRAM_IO_Reg             = 0;
    SPR_DMA_Reg             = 0;
    
    NMI_on_VBlank           = 0;
    sprite_size             = SPRITE_8x8;
    background_pattern_table = 0;
    sprite_pattern_table    = 0;
    address_increment       = 0;
    
    VRAM_access_address     = 0;
    
    VBlank                  = false;
    
    first_write             = true;
    first_read              = true;
    
    scanline_count          = 0;
    
    render_background       = false;
    render_sprites          = false;
    
    nametable_index         = 0;
    v_tile_offset           = 0;
    
    // Zero framebuffer
    for(int i = 0; i < 240; i++)
        memset(framebuffer[i], 0, 256);
}

void PPU::setup_mirroring(Byte mirroring) {
    switch(mirroring) {
        // This may need work
        case SINGLE_SCREEN_MIRRORING: {
            // +-----+-----+
            // |  0  |  0  |
            // +-----+-----+
            // |  0  |  0  |
            // +-----+-----+
            nametable_0 = &VRAM[NAMETABLE_0];
            nametable_1 = &VRAM[NAMETABLE_0];
            nametable_2 = &VRAM[NAMETABLE_0];
            nametable_3 = &VRAM[NAMETABLE_0];
            break;
        }
        case HORIZONTAL_MIRRORING: {
            // +-----+-----+
            // |  0  |  0  |
            // +-----+-----+
            // |  1  |  1  |
            // +-----+-----+
            nametable_0 = &VRAM[NAMETABLE_0];
            nametable_1 = &VRAM[NAMETABLE_0];
            nametable_2 = &VRAM[NAMETABLE_1];
            nametable_3 = &VRAM[NAMETABLE_1];
            
            break;
        }
        case VERTICAL_MIRRORING: {
            // +-----+-----+
            // |  0  |  1  |
            // +-----+-----+
            // |  0  |  1  |
            // +-----+-----+
            nametable_0 = &VRAM[NAMETABLE_0];
            nametable_1 = &VRAM[NAMETABLE_1];
            nametable_2 = &VRAM[NAMETABLE_0];
            nametable_3 = &VRAM[NAMETABLE_1];
            
            break;
        }
        // This may need work
        case FOUR_SCREEN_MIRRORING: {
            // +-----+-----+
            // |  0  |  1  |
            // +-----+-----+
            // |  2  |  3  |
            // +-----+-----+
            nametable_0 = &VRAM[NAMETABLE_0];
            nametable_1 = &VRAM[NAMETABLE_1];
            nametable_2 = &VRAM[NAMETABLE_2];
            nametable_3 = &VRAM[NAMETABLE_3];
            
            break;
        }
    }
}

Byte PPU::read(Word address) {
    switch(address) {
        // PPU Status Register - read only
        case 0x2002:
            //puts("Reading PPU status reg");
            return read_PPU_Status_Reg();
        
        // PPU memory data - read/write
        case 0x2007:
            //puts("Reading PPU memory");
            return read_VRAM();
    }
    return 0xFF;
}

void PPU::write(Byte data, Word address) {    
    switch(address) {
        // PPU control register 1 - write only
        case 0x2000:
            //puts("Writing to PPU control reg 1");
            write_PPU_Control_Reg_1(data);
            break;
            
        // PPU control register 2 - write only
        case 0x2001:
            //puts("Writing to PPU control reg 2");
            write_PPU_Control_Reg_2(data);
            break;
        
        // Sprite memory address - write only
        case 0x2003:
            //puts("Setting sprite memory address");
            write_SPR_RAM_Address_Reg(data);
            break;
            
        // sprite memory data - read/write
        case 0x2004:
            //puts("Writing to sprite memory");
            write_SPR_RAM(data);
            break;
        
        // Background control - write only  
        case 0x2005:
            //puts("Setting screen scroll reg");
            write_VRAM_Address_Reg_1(data);
            break;
        
        // PPU memory address - write only
        case 0x2006:
            //puts("Setting PPU memory address");
            write_VRAM_Address_Reg_2(data);
            break;
        
        // PPU memory data - read/write 
        case 0x2007:
            //puts("Writing to PPU memory");
            write_VRAM(data);
            break;
    }
}

// +---------+----------------------------------------------------------+
// |  $2000  | PPU Control Register #1 (W)                              |
// |         |                                                          |
// |         |    D7: Execute NMI on VBlank                             |
// |         |           0 = Disabled                                   |
// |         |           1 = Enabled                                    |
// |         |    D6: PPU Master/Slave Selection --+                    |
// |         |           0 = Master                +-- UNUSED           |
// |         |           1 = Slave               --+                    |
// |         |    D5: Sprite Size                                       |
// |         |           0 = 8x8                                        |
// |         |           1 = 8x16                                       |
// |         |    D4: Background Pattern Table Address                  |
// |         |           0 = $0000 (VRAM)                               |
// |         |           1 = $1000 (VRAM)                               |
// |         |    D3: Sprite Pattern Table Address                      |
// |         |           0 = $0000 (VRAM)                               |
// |         |           1 = $1000 (VRAM)                               |
// |         |    D2: PPU Address Increment                             |
// |         |           0 = Increment by 1                             |
// |         |           1 = Increment by 32                            |
// |         | D1-D0: Name Table Address                                |
// |         |         00 = $2000 (VRAM)                                |
// |         |         01 = $2400 (VRAM)                                |
// |         |         10 = $2800 (VRAM)                                |
// |         |         11 = $2C00 (VRAM)                                |
// +---------+----------------------------------------------------------+
inline void PPU::write_PPU_Control_Reg_1(Byte data) {
    PPU_Control_Reg_1 = data;
    
    // Whether NMI are enabled or not (1, 0)
    NMI_on_VBlank = (PPU_Control_Reg_1 >> 7) & 1;
    
    // Select (0)8x8 or (1)8x16 sizes
    sprite_size = (PPU_Control_Reg_1 >> 5) & 1;
    
    background_pattern_table = 
        ((PPU_Control_Reg_1 >> 4) & 1) ? 
        &VRAM[PATTERN_TABLE_1] : &VRAM[PATTERN_TABLE_0];
    
    sprite_pattern_table =
        ((PPU_Control_Reg_1 >> 3) & 1) ? 
        &VRAM[PATTERN_TABLE_1] : &VRAM[PATTERN_TABLE_0];
    
    address_increment = (PPU_Control_Reg_1 >> 2) & 1 ? 32 : 1;
    
    set_current_nametable();
}

// Set the current nametable based on bits 0 and 1 of PPU_Control_Reg_1
inline void PPU::set_current_nametable() {
    switch(((PPU_Control_Reg_1 & 2) << 1) | (PPU_Control_Reg_1 & 1)) {
        case 0: current_nametable = &VRAM[NAMETABLE_0]; break;
        case 1: current_nametable = &VRAM[NAMETABLE_1]; break;
        case 2: current_nametable = &VRAM[NAMETABLE_2]; break;
        case 3: current_nametable = &VRAM[NAMETABLE_3]; break;
    }
}

// +---------+----------------------------------------------------------+
// |  $2001  | PPU Control Register #2 (W)                              |
// |         |                                                          |
// |         | D7-D5: Full Background Colour (when D0 == 1)             |
// |         |         000 = None  +------------+                       |
// |         |         001 = Green              | NOTE: Do not use more |
// |         |         010 = Blue               |       than one type   |
// |         |         100 = Red   +------------+                       |
// |         | D7-D5: Colour Intensity (when D0 == 0)                   |
// |         |         000 = None            +--+                       |
// |         |         001 = Intensify green    | NOTE: Do not use more |
// |         |         010 = Intensify blue     |       than one type   |
// |         |         100 = Intensify red   +--+                       |
// |         |    D4: Sprite Visibility                                 |
// |         |           0 = Sprites not displayed                      |
// |         |           1 = Sprites visible                            |
// |         |    D3: Background Visibility                             |
// |         |           0 = Background not displayed                   |
// |         |           1 = Background visible                         |
// |         |    D2: Sprite Clipping                                   |
// |         |           0 = Sprites invisible in left 8-pixel column   |
// |         |           1 = No clipping                                |
// |         |    D1: Background Clipping                               |
// |         |           0 = BG invisible in left 8-pixel column        |
// |         |           1 = No clipping                                |
// |         |    D0: Display Type                                      |
// |         |           0 = Colour display                             |
// |         |           1 = Monochrome display                         |
// +---------+----------------------------------------------------------+
inline void PPU::write_PPU_Control_Reg_2(Byte data) {
    PPU_Control_Reg_2 = data;
    
    render_background = PPU_Control_Reg_2 >> 3 & 1;
    render_sprites = PPU_Control_Reg_2 >> 4 & 1;
    // to be implemented
}

// +---------+----------------------------------------------------------+
// |  $2002  | PPU Status Register (R)                                  |
// |         |                                                          |
// |         |    D7: VBlank Occurance                                  |
// |         |          0 = Not occuring                                |
// |         |          1 = In VBlank                                   |
// |         |    D6: Sprite #0 Occurance                               |
// |         |          0 = Sprite #0 not found                         |
// |         |          1 = PPU has hit Sprite #0                       |
// |         |    D5: Scanline Sprite Count                             |
// |         |          0 = Eight (8) sprites or less on current scan-  |
// |         |              line                                        |
// |         |          1 = More than 8 sprites on current scanline     |
// |         |    D4: VRAM Write Flag                                   |
// |         |          0 = Writes to VRAM are respected                |
// |         |          1 = Writes to VRAM are ignored                  |
// |         |                                                          |
// |         | NOTE: D7 is set to 0 after read occurs.                  |
// |         | NOTE: After a read occurs, $2005 is reset, hence the     |
// |         |       next write to $2005 will be Horizontal.            |
// |         | NOTE: After a read occurs, $2006 is reset, hence the     |
// |         |       next write to $2006 will be the high byte portion. |
// |         |                                                          |
// |         | For detailed information regarding D6, see Section 4,    |
// |         | Subsection L.                                            |
// +---------+----------------------------------------------------------+
inline Byte PPU::read_PPU_Status_Reg() {
    Byte temp = PPU_Status_Reg;
    
    // Reset bit 7
    PPU_Status_Reg &= ~0x80;
    //Reset 2005;
    //VRAM_Address_Reg_1 = 0;
    //Reset 2006;
    //VRAM_Address_Reg_2 = 0;
    
    //http://nesdev.parodius.com/bbs/viewtopic.php?t=2914&postdays=0&postorder=asc&highlight=nestress&start=15
    // $2005 and $2006 use a sort of toggle, which determines which write is the "first" write, and which write is the "second" write. Reading $2002 does not change $2005 or $2006, but it does reset this toggle so that the next write will be the first write: 
    
    first_write = true;
    
    return temp;
}

// +---------+----------------------------------------------------------+
// |  $2003  | SPR-RAM Address Register (W)                             |
// |         |                                                          |
// |         | D7-D0: 8-bit address in SPR-RAM to access via $2004.     |
// +---------+----------------------------------------------------------+
inline void PPU::write_SPR_RAM_Address_Reg(Byte data) {
    SPR_RAM_Address_Reg = data;
}

// +---------+----------------------------------------------------------+
// |  $2004  | SPR-RAM I/O Register (W)                                 |
// |         |                                                          |
// |         | D7-D0: 8-bit data written to SPR-RAM.                    |
// +---------+----------------------------------------------------------+
inline void PPU::write_SPR_RAM(Byte data) {
    SPR_RAM[SPR_RAM_Address_Reg++] = data;
}

// +---------+----------------------------------------------------------+
// |  $2005  | VRAM Address Register #1 (W2)                            |
// |         |                                                          |
// |         |  Commonly used used to "pan/scroll" the screen (sprites  |
// |         |  excluded) horizontally and vertically. However, there   |
// |         |  is no actual panning hardware inside the NES. This      |
// |         |  register controls VRAM addressing lines.                |
// |         |                                                          |
// |         | Refer to Section 4, Subsection N, for more information.  |
// +---------+----------------------------------------------------------+
// ------+-----+---------------------------------------------------------------
// $2005 | W   | Screen Scroll Offsets
//       |     | There are two scroll registers, vertical and horizontal, 
//       |     | which are both written via this port. The first value written
//       |     | will go into the Vertical Scroll Register (unless it is >239,
//       |     | then it will be ignored). The second value will appear in the
//       |     | Horizontal Scroll Register. Name Tables are assumed to be
//       |     | arranged in the following way:
//       |     |
//       |     |           +-----------+-----------+
//       |     |           | 2 ($2800) | 3 ($2C00) |
//       |     |           +-----------+-----------+
//       |     |           | 0 ($2000) | 1 ($2400) |
//       |     |           +-----------+-----------+
//       |     |
//       |     | When scrolled, the picture may span over several Name Tables.
//       |     | Remember that because of the mirroring there are only 2 real
//       |     | Name Tables, not 4.
// ------+-----+---------------------------------------------------------------
// $2005: VRAM Register 1 
// 2 consecutive writes tell PPU where to start next scan line in name table 
// X-index into name table, x-offset of pattern tiles 
// Y-index into name table, y-offset of pattern tilesX-index is incremented as each tile is drawn. 
// Alternate name table used if X-index wraps over 31Y-index is incremented as each row is drawn 
// Alternate name table used if Y-index wraps over 29
inline void PPU::write_VRAM_Address_Reg_1(Byte data) {
    VRAM_Address_Reg_1 = data;
}

// Writing to PPU memory:
// a) Write upper address byte into $2006
// b) Write lower address byte into $2006
// c) Write data into $2007. After each write, the
//    address will increment either by 1 (bit 2 of
//    $2000 is 0) or by 32 (bit 2 of $2000 is 1).
//  
// Reading from PPU memory:
// a) Write upper address byte into $2006
// b) Write lower address byte into $2006
// c) Read data from $2007. The first byte read from
//    $2007 will be invalid (see "PPU Details"). Then,
//    the address will increment by 1 after each read.
// +---------+----------------------------------------------------------+
// |  $2006  | VRAM Address Register #2 (W2)                            |
// |         |                                                          |
// |         |  Commonly used to specify the 16-bit address in VRAM to  |
// |         |  access via $2007. However, this register controls VRAM  |
// |         |  addressing bits, and therefore should be used with      |
// |         |  knowledge of how it works, and when it works.           |
// |         |                                                          |
// |         | Refer to Section 4, Subsection N, for more information.  |
// +---------+----------------------------------------------------------+
inline void PPU::write_VRAM_Address_Reg_2(Byte data) {
    if(first_write) {
        VRAM_Address_Reg_2 = data;
        first_write = false;
    }
    else {
        VRAM_access_address = ((Word) (VRAM_Address_Reg_2 << 8) | (Byte) data);
        first_write = true;
        first_read = true;
    }
}

// +---------+----------------------------------------------------------+
// |  $2007  | VRAM I/O Register (RW)                                   |
// |         |                                                          |
// |         | D7-D0: 8-bit data read/written from/to VRAM.             |
// +---------+----------------------------------------------------------+
inline Byte PPU::read_VRAM() {
    // first read is invalid - UNLESS IT'S FROM THE COLOUR PALETTES
    if(first_read && 
        !(VRAM_access_address >= 0x3F00 && VRAM_access_address < 0x4000)) {
        first_read = false;
        return 0;
    }
    Byte temp = VRAM[VRAM_access_address];
    VRAM_access_address += address_increment;
    return temp;
}

inline void PPU::write_VRAM(Byte data) {
    VRAM[VRAM_access_address] = data;
    VRAM_access_address += address_increment;
}

// +---------+----------------------------------------------------------+
// |  $4014  | Sprite DMA Register (W)                                  |
// |         |                                                          |
// |         |  Transfers 256 bytes of memory into SPR-RAM. The address |
// |         |  read from is $100*N, where N is the value written.      |
// +---------+----------------------------------------------------------+
// CPU has to wait 512 cycles before it can do anything else.
// Remember to take this into account.
void PPU::write_SPR_DMA(Memory &cpumem, Byte address) {
    memcpy(&SPR_RAM[0], &cpumem[address << 8], 0x100);
}

// The picture is scanlines 0 through 239, and vertical blanking is scanlines 241 through 260 (PAL 310) inclusive. On scanlines 240 and 261 (PAL 311), the PPU goes through the motions of VRAM fetching but renders nothing, in order to get the prefetch buffers into a known state for scanline 0.
void PPU::emulate() {
    // Clear VBlank occurring flag on the first scanline
    if(scanline_count == 0) {
        // Reset VBlank flag
        PPU_Status_Reg &= ~0x80;
    }
    if(scanline_count < 240) {
        
        // Picture is scanlines 0 - 239
        
        // Nametable current column position
        nametable_index = (scanline_count >> 3) << 5;
        
        // Vertical offset within current tile
        v_tile_offset = scanline_count & 0x7;
        
        if(render_background) render_background_scanline();
    }
    else if(scanline_count == 240) {
        // Do nothing
    }
    else if(scanline_count == 241) {
        // enter vblank for 20 scanlines
        PPU_Status_Reg |= 0x80;
        VBlank = true;
    }
    else if(scanline_count < 261) {
        // In VBlank
        
        // Reset Sprite 0 HIT flag
        PPU_Status_Reg &= ~0x40;
    }
    else if(scanline_count == 261) {
        // This is a dummy scanline on the actual NES, but is used here
        // to draw all the sprites at once and merge them with the
        // background.
        if(render_sprites) render_sprite_frame();
    }
    
    scanline_count++;
    scanline_count %= MAX_SCANLINE;
}

// Render a background scanline (256 pixels)
inline void PPU::render_background_scanline() {
    // 1 nametable entry represents 8 pixels (8 * 32 == 256)
    for(int i = 0; i < 32; i++) {
        // Get the tile # to look up in the pattern table
        int tile_index = current_nametable[nametable_index + i] << 4;
        
        Byte tile_plane_1
            = background_pattern_table[tile_index + v_tile_offset];
        Byte tile_plane_2
            = background_pattern_table[tile_index + v_tile_offset + 8];
               
        Byte attribute
            = current_nametable[ATTRIBUTE_TABLE_OFFSET
            + attribute_byte_table[nametable_index + i]];
        
        int palette_square = attribute_square_table[nametable_index + i] << 1;
        
        int palette_index = (((attribute >> (palette_square + 1)) & 1) << 1)
            | ((attribute >> palette_square) & 1);
        
        Byte pixels[8];
        
        int k = 0;
        for(int bit = 7; bit >= 0; bit--) {
            int colour_index = (((tile_plane_2 >> bit) & 1) << 1) |
                ((tile_plane_1 >> bit) & 1);
            
            pixels[k++] = VRAM[IMAGE_PALETTE + (palette_index << 2) + colour_index];
        }
        
        // Copy pixels into current scanline
        memcpy(framebuffer[scanline_count] + (i << 3), pixels, 8);
    }
}

// This approach to sprite rendering draws the whole lot and merges
// them into the framebuffer AFTTER the background is completed.

// TODO:
// Priority / background rendering
// 8x16 sprites
// More than 8 sprites on a scanline
inline void PPU::render_sprite_frame() {
    for(int i = 252; i >= 0; i -= 4) {
        Byte* sprite = &SPR_RAM[i];
        
        int y_pos = sprite[0] + 1;
        int h_pos = sprite[3];
        
        int tile_index = sprite[1] << 4;
        
        Byte* tile = &sprite_pattern_table[tile_index];
        
        int palette_index = (((sprite[2] >> 1) & 1) << 1)
            | (sprite[2] & 1);
        
        bool h_flip = (sprite[2] >> 6) & 1;
        bool v_flip = (sprite[2] >> 7) & 1;
        
        //bool bg_priority = (sprite[2] >> 5) & 1;
        
        int pixels[8][8];
        
        for(int j = 0; j < 8; j++) {
            int k = 0;
            for(int bit = 7; bit >= 0; bit--) {
                int colour_index = (((tile[j + 8] >> bit) & 1) << 1) |
                    ((tile[j] >> bit) & 1);
                
                // If NOT a transparent colour...
                if(colour_index > 0)
                    pixels[j][k]
                        = VRAM[SPRITE_PALETTE + (palette_index << 2) + colour_index];
                // Otherwise flag pixel as transparent (-1)
                else
                    pixels[j][k] = -1;
                    
                k++;
            }
        }
        
        // Copy tile pixels into framebuffer
        for(int v = 0; v < 8; v++)
            
            for(int h = 0; h < 8; h++)
                
                if(y_pos + v < 240 && pixels[v][h] != -1) {
                    int y = y_pos + (v_flip ? 7 - v : v);
                    int x = h_pos + (h_flip ? 7 - h : h);
                    Byte bg_pixel = framebuffer[y][x];
                    
                    // Only for sprite 0
                    if(i == 0 && (bg_pixel != VRAM[IMAGE_PALETTE] && pixels[v][h] 
                        != VRAM[SPRITE_PALETTE])) {
                        // If bg pixel and sprite 0 pixel are non-transparent set hit flag
                        PPU_Status_Reg |= 0x40;
                    }
                    
                    framebuffer[y][x] = pixels[v][h];
                }
    }
}

// for testing only at this stage
bool PPU::VBlank_occurring() {
    if(((PPU_Status_Reg >> 7) & 1) && VBlank && NMI_on_VBlank) {
        VBlank = false;
        return true;
    }
    return false;
}

void PPU::load_attribute_byte_table() {
    attribute_byte_table[0]    = 0;
    attribute_byte_table[1]    = 0;
    attribute_byte_table[2]    = 0;
    attribute_byte_table[3]    = 0;
    attribute_byte_table[4]    = 1;
    attribute_byte_table[5]    = 1;
    attribute_byte_table[6]    = 1;
    attribute_byte_table[7]    = 1;
    attribute_byte_table[8]    = 2;
    attribute_byte_table[9]    = 2;
    attribute_byte_table[10]   = 2;
    attribute_byte_table[11]   = 2;
    attribute_byte_table[12]   = 3;
    attribute_byte_table[13]   = 3;
    attribute_byte_table[14]   = 3;
    attribute_byte_table[15]   = 3;
    attribute_byte_table[16]   = 4;
    attribute_byte_table[17]   = 4;
    attribute_byte_table[18]   = 4;
    attribute_byte_table[19]   = 4;
    attribute_byte_table[20]   = 5;
    attribute_byte_table[21]   = 5;
    attribute_byte_table[22]   = 5;
    attribute_byte_table[23]   = 5;
    attribute_byte_table[24]   = 6;
    attribute_byte_table[25]   = 6;
    attribute_byte_table[26]   = 6;
    attribute_byte_table[27]   = 6;
    attribute_byte_table[28]   = 7;
    attribute_byte_table[29]   = 7;
    attribute_byte_table[30]   = 7;
    attribute_byte_table[31]   = 7;
    attribute_byte_table[32]   = 0;
    attribute_byte_table[33]   = 0;
    attribute_byte_table[34]   = 0;
    attribute_byte_table[35]   = 0;
    attribute_byte_table[36]   = 1;
    attribute_byte_table[37]   = 1;
    attribute_byte_table[38]   = 1;
    attribute_byte_table[39]   = 1;
    attribute_byte_table[40]   = 2;
    attribute_byte_table[41]   = 2;
    attribute_byte_table[42]   = 2;
    attribute_byte_table[43]   = 2;
    attribute_byte_table[44]   = 3;
    attribute_byte_table[45]   = 3;
    attribute_byte_table[46]   = 3;
    attribute_byte_table[47]   = 3;
    attribute_byte_table[48]   = 4;
    attribute_byte_table[49]   = 4;
    attribute_byte_table[50]   = 4;
    attribute_byte_table[51]   = 4;
    attribute_byte_table[52]   = 5;
    attribute_byte_table[53]   = 5;
    attribute_byte_table[54]   = 5;
    attribute_byte_table[55]   = 5;
    attribute_byte_table[56]   = 6;
    attribute_byte_table[57]   = 6;
    attribute_byte_table[58]   = 6;
    attribute_byte_table[59]   = 6;
    attribute_byte_table[60]   = 7;
    attribute_byte_table[61]   = 7;
    attribute_byte_table[62]   = 7;
    attribute_byte_table[63]   = 7;
    attribute_byte_table[64]   = 0;
    attribute_byte_table[65]   = 0;
    attribute_byte_table[66]   = 0;
    attribute_byte_table[67]   = 0;
    attribute_byte_table[68]   = 1;
    attribute_byte_table[69]   = 1;
    attribute_byte_table[70]   = 1;
    attribute_byte_table[71]   = 1;
    attribute_byte_table[72]   = 2;
    attribute_byte_table[73]   = 2;
    attribute_byte_table[74]   = 2;
    attribute_byte_table[75]   = 2;
    attribute_byte_table[76]   = 3;
    attribute_byte_table[77]   = 3;
    attribute_byte_table[78]   = 3;
    attribute_byte_table[79]   = 3;
    attribute_byte_table[80]   = 4;
    attribute_byte_table[81]   = 4;
    attribute_byte_table[82]   = 4;
    attribute_byte_table[83]   = 4;
    attribute_byte_table[84]   = 5;
    attribute_byte_table[85]   = 5;
    attribute_byte_table[86]   = 5;
    attribute_byte_table[87]   = 5;
    attribute_byte_table[88]   = 6;
    attribute_byte_table[89]   = 6;
    attribute_byte_table[90]   = 6;
    attribute_byte_table[91]   = 6;
    attribute_byte_table[92]   = 7;
    attribute_byte_table[93]   = 7;
    attribute_byte_table[94]   = 7;
    attribute_byte_table[95]   = 7;
    attribute_byte_table[96]   = 0;
    attribute_byte_table[97]   = 0;
    attribute_byte_table[98]   = 0;
    attribute_byte_table[99]   = 0;
    attribute_byte_table[100]  = 1;
    attribute_byte_table[101]  = 1;
    attribute_byte_table[102]  = 1;
    attribute_byte_table[103]  = 1;
    attribute_byte_table[104]  = 2;
    attribute_byte_table[105]  = 2;
    attribute_byte_table[106]  = 2;
    attribute_byte_table[107]  = 2;
    attribute_byte_table[108]  = 3;
    attribute_byte_table[109]  = 3;
    attribute_byte_table[110]  = 3;
    attribute_byte_table[111]  = 3;
    attribute_byte_table[112]  = 4;
    attribute_byte_table[113]  = 4;
    attribute_byte_table[114]  = 4;
    attribute_byte_table[115]  = 4;
    attribute_byte_table[116]  = 5;
    attribute_byte_table[117]  = 5;
    attribute_byte_table[118]  = 5;
    attribute_byte_table[119]  = 5;
    attribute_byte_table[120]  = 6;
    attribute_byte_table[121]  = 6;
    attribute_byte_table[122]  = 6;
    attribute_byte_table[123]  = 6;
    attribute_byte_table[124]  = 7;
    attribute_byte_table[125]  = 7;
    attribute_byte_table[126]  = 7;
    attribute_byte_table[127]  = 7;
    attribute_byte_table[128]  = 8;
    attribute_byte_table[129]  = 8;
    attribute_byte_table[130]  = 8;
    attribute_byte_table[131]  = 8;
    attribute_byte_table[132]  = 9;
    attribute_byte_table[133]  = 9;
    attribute_byte_table[134]  = 9;
    attribute_byte_table[135]  = 9;
    attribute_byte_table[136]  = 10;
    attribute_byte_table[137]  = 10;
    attribute_byte_table[138]  = 10;
    attribute_byte_table[139]  = 10;
    attribute_byte_table[140]  = 11;
    attribute_byte_table[141]  = 11;
    attribute_byte_table[142]  = 11;
    attribute_byte_table[143]  = 11;
    attribute_byte_table[144]  = 12;
    attribute_byte_table[145]  = 12;
    attribute_byte_table[146]  = 12;
    attribute_byte_table[147]  = 12;
    attribute_byte_table[148]  = 13;
    attribute_byte_table[149]  = 13;
    attribute_byte_table[150]  = 13;
    attribute_byte_table[151]  = 13;
    attribute_byte_table[152]  = 14;
    attribute_byte_table[153]  = 14;
    attribute_byte_table[154]  = 14;
    attribute_byte_table[155]  = 14;
    attribute_byte_table[156]  = 15;
    attribute_byte_table[157]  = 15;
    attribute_byte_table[158]  = 15;
    attribute_byte_table[159]  = 15;
    attribute_byte_table[160]  = 8;
    attribute_byte_table[161]  = 8;
    attribute_byte_table[162]  = 8;
    attribute_byte_table[163]  = 8;
    attribute_byte_table[164]  = 9;
    attribute_byte_table[165]  = 9;
    attribute_byte_table[166]  = 9;
    attribute_byte_table[167]  = 9;
    attribute_byte_table[168]  = 10;
    attribute_byte_table[169]  = 10;
    attribute_byte_table[170]  = 10;
    attribute_byte_table[171]  = 10;
    attribute_byte_table[172]  = 11;
    attribute_byte_table[173]  = 11;
    attribute_byte_table[174]  = 11;
    attribute_byte_table[175]  = 11;
    attribute_byte_table[176]  = 12;
    attribute_byte_table[177]  = 12;
    attribute_byte_table[178]  = 12;
    attribute_byte_table[179]  = 12;
    attribute_byte_table[180]  = 13;
    attribute_byte_table[181]  = 13;
    attribute_byte_table[182]  = 13;
    attribute_byte_table[183]  = 13;
    attribute_byte_table[184]  = 14;
    attribute_byte_table[185]  = 14;
    attribute_byte_table[186]  = 14;
    attribute_byte_table[187]  = 14;
    attribute_byte_table[188]  = 15;
    attribute_byte_table[189]  = 15;
    attribute_byte_table[190]  = 15;
    attribute_byte_table[191]  = 15;
    attribute_byte_table[192]  = 8;
    attribute_byte_table[193]  = 8;
    attribute_byte_table[194]  = 8;
    attribute_byte_table[195]  = 8;
    attribute_byte_table[196]  = 9;
    attribute_byte_table[197]  = 9;
    attribute_byte_table[198]  = 9;
    attribute_byte_table[199]  = 9;
    attribute_byte_table[200]  = 10;
    attribute_byte_table[201]  = 10;
    attribute_byte_table[202]  = 10;
    attribute_byte_table[203]  = 10;
    attribute_byte_table[204]  = 11;
    attribute_byte_table[205]  = 11;
    attribute_byte_table[206]  = 11;
    attribute_byte_table[207]  = 11;
    attribute_byte_table[208]  = 12;
    attribute_byte_table[209]  = 12;
    attribute_byte_table[210]  = 12;
    attribute_byte_table[211]  = 12;
    attribute_byte_table[212]  = 13;
    attribute_byte_table[213]  = 13;
    attribute_byte_table[214]  = 13;
    attribute_byte_table[215]  = 13;
    attribute_byte_table[216]  = 14;
    attribute_byte_table[217]  = 14;
    attribute_byte_table[218]  = 14;
    attribute_byte_table[219]  = 14;
    attribute_byte_table[220]  = 15;
    attribute_byte_table[221]  = 15;
    attribute_byte_table[222]  = 15;
    attribute_byte_table[223]  = 15;
    attribute_byte_table[224]  = 8;
    attribute_byte_table[225]  = 8;
    attribute_byte_table[226]  = 8;
    attribute_byte_table[227]  = 8;
    attribute_byte_table[228]  = 9;
    attribute_byte_table[229]  = 9;
    attribute_byte_table[230]  = 9;
    attribute_byte_table[231]  = 9;
    attribute_byte_table[232]  = 10;
    attribute_byte_table[233]  = 10;
    attribute_byte_table[234]  = 10;
    attribute_byte_table[235]  = 10;
    attribute_byte_table[236]  = 11;
    attribute_byte_table[237]  = 11;
    attribute_byte_table[238]  = 11;
    attribute_byte_table[239]  = 11;
    attribute_byte_table[240]  = 12;
    attribute_byte_table[241]  = 12;
    attribute_byte_table[242]  = 12;
    attribute_byte_table[243]  = 12;
    attribute_byte_table[244]  = 13;
    attribute_byte_table[245]  = 13;
    attribute_byte_table[246]  = 13;
    attribute_byte_table[247]  = 13;
    attribute_byte_table[248]  = 14;
    attribute_byte_table[249]  = 14;
    attribute_byte_table[250]  = 14;
    attribute_byte_table[251]  = 14;
    attribute_byte_table[252]  = 15;
    attribute_byte_table[253]  = 15;
    attribute_byte_table[254]  = 15;
    attribute_byte_table[255]  = 15;
    attribute_byte_table[256]  = 16;
    attribute_byte_table[257]  = 16;
    attribute_byte_table[258]  = 16;
    attribute_byte_table[259]  = 16;
    attribute_byte_table[260]  = 17;
    attribute_byte_table[261]  = 17;
    attribute_byte_table[262]  = 17;
    attribute_byte_table[263]  = 17;
    attribute_byte_table[264]  = 18;
    attribute_byte_table[265]  = 18;
    attribute_byte_table[266]  = 18;
    attribute_byte_table[267]  = 18;
    attribute_byte_table[268]  = 19;
    attribute_byte_table[269]  = 19;
    attribute_byte_table[270]  = 19;
    attribute_byte_table[271]  = 19;
    attribute_byte_table[272]  = 20;
    attribute_byte_table[273]  = 20;
    attribute_byte_table[274]  = 20;
    attribute_byte_table[275]  = 20;
    attribute_byte_table[276]  = 21;
    attribute_byte_table[277]  = 21;
    attribute_byte_table[278]  = 21;
    attribute_byte_table[279]  = 21;
    attribute_byte_table[280]  = 22;
    attribute_byte_table[281]  = 22;
    attribute_byte_table[282]  = 22;
    attribute_byte_table[283]  = 22;
    attribute_byte_table[284]  = 23;
    attribute_byte_table[285]  = 23;
    attribute_byte_table[286]  = 23;
    attribute_byte_table[287]  = 23;
    attribute_byte_table[288]  = 16;
    attribute_byte_table[289]  = 16;
    attribute_byte_table[290]  = 16;
    attribute_byte_table[291]  = 16;
    attribute_byte_table[292]  = 17;
    attribute_byte_table[293]  = 17;
    attribute_byte_table[294]  = 17;
    attribute_byte_table[295]  = 17;
    attribute_byte_table[296]  = 18;
    attribute_byte_table[297]  = 18;
    attribute_byte_table[298]  = 18;
    attribute_byte_table[299]  = 18;
    attribute_byte_table[300]  = 19;
    attribute_byte_table[301]  = 19;
    attribute_byte_table[302]  = 19;
    attribute_byte_table[303]  = 19;
    attribute_byte_table[304]  = 20;
    attribute_byte_table[305]  = 20;
    attribute_byte_table[306]  = 20;
    attribute_byte_table[307]  = 20;
    attribute_byte_table[308]  = 21;
    attribute_byte_table[309]  = 21;
    attribute_byte_table[310]  = 21;
    attribute_byte_table[311]  = 21;
    attribute_byte_table[312]  = 22;
    attribute_byte_table[313]  = 22;
    attribute_byte_table[314]  = 22;
    attribute_byte_table[315]  = 22;
    attribute_byte_table[316]  = 23;
    attribute_byte_table[317]  = 23;
    attribute_byte_table[318]  = 23;
    attribute_byte_table[319]  = 23;
    attribute_byte_table[320]  = 16;
    attribute_byte_table[321]  = 16;
    attribute_byte_table[322]  = 16;
    attribute_byte_table[323]  = 16;
    attribute_byte_table[324]  = 17;
    attribute_byte_table[325]  = 17;
    attribute_byte_table[326]  = 17;
    attribute_byte_table[327]  = 17;
    attribute_byte_table[328]  = 18;
    attribute_byte_table[329]  = 18;
    attribute_byte_table[330]  = 18;
    attribute_byte_table[331]  = 18;
    attribute_byte_table[332]  = 19;
    attribute_byte_table[333]  = 19;
    attribute_byte_table[334]  = 19;
    attribute_byte_table[335]  = 19;
    attribute_byte_table[336]  = 20;
    attribute_byte_table[337]  = 20;
    attribute_byte_table[338]  = 20;
    attribute_byte_table[339]  = 20;
    attribute_byte_table[340]  = 21;
    attribute_byte_table[341]  = 21;
    attribute_byte_table[342]  = 21;
    attribute_byte_table[343]  = 21;
    attribute_byte_table[344]  = 22;
    attribute_byte_table[345]  = 22;
    attribute_byte_table[346]  = 22;
    attribute_byte_table[347]  = 22;
    attribute_byte_table[348]  = 23;
    attribute_byte_table[349]  = 23;
    attribute_byte_table[350]  = 23;
    attribute_byte_table[351]  = 23;
    attribute_byte_table[352]  = 16;
    attribute_byte_table[353]  = 16;
    attribute_byte_table[354]  = 16;
    attribute_byte_table[355]  = 16;
    attribute_byte_table[356]  = 17;
    attribute_byte_table[357]  = 17;
    attribute_byte_table[358]  = 17;
    attribute_byte_table[359]  = 17;
    attribute_byte_table[360]  = 18;
    attribute_byte_table[361]  = 18;
    attribute_byte_table[362]  = 18;
    attribute_byte_table[363]  = 18;
    attribute_byte_table[364]  = 19;
    attribute_byte_table[365]  = 19;
    attribute_byte_table[366]  = 19;
    attribute_byte_table[367]  = 19;
    attribute_byte_table[368]  = 20;
    attribute_byte_table[369]  = 20;
    attribute_byte_table[370]  = 20;
    attribute_byte_table[371]  = 20;
    attribute_byte_table[372]  = 21;
    attribute_byte_table[373]  = 21;
    attribute_byte_table[374]  = 21;
    attribute_byte_table[375]  = 21;
    attribute_byte_table[376]  = 22;
    attribute_byte_table[377]  = 22;
    attribute_byte_table[378]  = 22;
    attribute_byte_table[379]  = 22;
    attribute_byte_table[380]  = 23;
    attribute_byte_table[381]  = 23;
    attribute_byte_table[382]  = 23;
    attribute_byte_table[383]  = 23;
    attribute_byte_table[384]  = 24;
    attribute_byte_table[385]  = 24;
    attribute_byte_table[386]  = 24;
    attribute_byte_table[387]  = 24;
    attribute_byte_table[388]  = 25;
    attribute_byte_table[389]  = 25;
    attribute_byte_table[390]  = 25;
    attribute_byte_table[391]  = 25;
    attribute_byte_table[392]  = 26;
    attribute_byte_table[393]  = 26;
    attribute_byte_table[394]  = 26;
    attribute_byte_table[395]  = 26;
    attribute_byte_table[396]  = 27;
    attribute_byte_table[397]  = 27;
    attribute_byte_table[398]  = 27;
    attribute_byte_table[399]  = 27;
    attribute_byte_table[400]  = 28;
    attribute_byte_table[401]  = 28;
    attribute_byte_table[402]  = 28;
    attribute_byte_table[403]  = 28;
    attribute_byte_table[404]  = 29;
    attribute_byte_table[405]  = 29;
    attribute_byte_table[406]  = 29;
    attribute_byte_table[407]  = 29;
    attribute_byte_table[408]  = 30;
    attribute_byte_table[409]  = 30;
    attribute_byte_table[410]  = 30;
    attribute_byte_table[411]  = 30;
    attribute_byte_table[412]  = 31;
    attribute_byte_table[413]  = 31;
    attribute_byte_table[414]  = 31;
    attribute_byte_table[415]  = 31;
    attribute_byte_table[416]  = 24;
    attribute_byte_table[417]  = 24;
    attribute_byte_table[418]  = 24;
    attribute_byte_table[419]  = 24;
    attribute_byte_table[420]  = 25;
    attribute_byte_table[421]  = 25;
    attribute_byte_table[422]  = 25;
    attribute_byte_table[423]  = 25;
    attribute_byte_table[424]  = 26;
    attribute_byte_table[425]  = 26;
    attribute_byte_table[426]  = 26;
    attribute_byte_table[427]  = 26;
    attribute_byte_table[428]  = 27;
    attribute_byte_table[429]  = 27;
    attribute_byte_table[430]  = 27;
    attribute_byte_table[431]  = 27;
    attribute_byte_table[432]  = 28;
    attribute_byte_table[433]  = 28;
    attribute_byte_table[434]  = 28;
    attribute_byte_table[435]  = 28;
    attribute_byte_table[436]  = 29;
    attribute_byte_table[437]  = 29;
    attribute_byte_table[438]  = 29;
    attribute_byte_table[439]  = 29;
    attribute_byte_table[440]  = 30;
    attribute_byte_table[441]  = 30;
    attribute_byte_table[442]  = 30;
    attribute_byte_table[443]  = 30;
    attribute_byte_table[444]  = 31;
    attribute_byte_table[445]  = 31;
    attribute_byte_table[446]  = 31;
    attribute_byte_table[447]  = 31;
    attribute_byte_table[448]  = 24;
    attribute_byte_table[449]  = 24;
    attribute_byte_table[450]  = 24;
    attribute_byte_table[451]  = 24;
    attribute_byte_table[452]  = 25;
    attribute_byte_table[453]  = 25;
    attribute_byte_table[454]  = 25;
    attribute_byte_table[455]  = 25;
    attribute_byte_table[456]  = 26;
    attribute_byte_table[457]  = 26;
    attribute_byte_table[458]  = 26;
    attribute_byte_table[459]  = 26;
    attribute_byte_table[460]  = 27;
    attribute_byte_table[461]  = 27;
    attribute_byte_table[462]  = 27;
    attribute_byte_table[463]  = 27;
    attribute_byte_table[464]  = 28;
    attribute_byte_table[465]  = 28;
    attribute_byte_table[466]  = 28;
    attribute_byte_table[467]  = 28;
    attribute_byte_table[468]  = 29;
    attribute_byte_table[469]  = 29;
    attribute_byte_table[470]  = 29;
    attribute_byte_table[471]  = 29;
    attribute_byte_table[472]  = 30;
    attribute_byte_table[473]  = 30;
    attribute_byte_table[474]  = 30;
    attribute_byte_table[475]  = 30;
    attribute_byte_table[476]  = 31;
    attribute_byte_table[477]  = 31;
    attribute_byte_table[478]  = 31;
    attribute_byte_table[479]  = 31;
    attribute_byte_table[480]  = 24;
    attribute_byte_table[481]  = 24;
    attribute_byte_table[482]  = 24;
    attribute_byte_table[483]  = 24;
    attribute_byte_table[484]  = 25;
    attribute_byte_table[485]  = 25;
    attribute_byte_table[486]  = 25;
    attribute_byte_table[487]  = 25;
    attribute_byte_table[488]  = 26;
    attribute_byte_table[489]  = 26;
    attribute_byte_table[490]  = 26;
    attribute_byte_table[491]  = 26;
    attribute_byte_table[492]  = 27;
    attribute_byte_table[493]  = 27;
    attribute_byte_table[494]  = 27;
    attribute_byte_table[495]  = 27;
    attribute_byte_table[496]  = 28;
    attribute_byte_table[497]  = 28;
    attribute_byte_table[498]  = 28;
    attribute_byte_table[499]  = 28;
    attribute_byte_table[500]  = 29;
    attribute_byte_table[501]  = 29;
    attribute_byte_table[502]  = 29;
    attribute_byte_table[503]  = 29;
    attribute_byte_table[504]  = 30;
    attribute_byte_table[505]  = 30;
    attribute_byte_table[506]  = 30;
    attribute_byte_table[507]  = 30;
    attribute_byte_table[508]  = 31;
    attribute_byte_table[509]  = 31;
    attribute_byte_table[510]  = 31;
    attribute_byte_table[511]  = 31;
    attribute_byte_table[512]  = 32;
    attribute_byte_table[513]  = 32;
    attribute_byte_table[514]  = 32;
    attribute_byte_table[515]  = 32;
    attribute_byte_table[516]  = 33;
    attribute_byte_table[517]  = 33;
    attribute_byte_table[518]  = 33;
    attribute_byte_table[519]  = 33;
    attribute_byte_table[520]  = 34;
    attribute_byte_table[521]  = 34;
    attribute_byte_table[522]  = 34;
    attribute_byte_table[523]  = 34;
    attribute_byte_table[524]  = 35;
    attribute_byte_table[525]  = 35;
    attribute_byte_table[526]  = 35;
    attribute_byte_table[527]  = 35;
    attribute_byte_table[528]  = 36;
    attribute_byte_table[529]  = 36;
    attribute_byte_table[530]  = 36;
    attribute_byte_table[531]  = 36;
    attribute_byte_table[532]  = 37;
    attribute_byte_table[533]  = 37;
    attribute_byte_table[534]  = 37;
    attribute_byte_table[535]  = 37;
    attribute_byte_table[536]  = 38;
    attribute_byte_table[537]  = 38;
    attribute_byte_table[538]  = 38;
    attribute_byte_table[539]  = 38;
    attribute_byte_table[540]  = 39;
    attribute_byte_table[541]  = 39;
    attribute_byte_table[542]  = 39;
    attribute_byte_table[543]  = 39;
    attribute_byte_table[544]  = 32;
    attribute_byte_table[545]  = 32;
    attribute_byte_table[546]  = 32;
    attribute_byte_table[547]  = 32;
    attribute_byte_table[548]  = 33;
    attribute_byte_table[549]  = 33;
    attribute_byte_table[550]  = 33;
    attribute_byte_table[551]  = 33;
    attribute_byte_table[552]  = 34;
    attribute_byte_table[553]  = 34;
    attribute_byte_table[554]  = 34;
    attribute_byte_table[555]  = 34;
    attribute_byte_table[556]  = 35;
    attribute_byte_table[557]  = 35;
    attribute_byte_table[558]  = 35;
    attribute_byte_table[559]  = 35;
    attribute_byte_table[560]  = 36;
    attribute_byte_table[561]  = 36;
    attribute_byte_table[562]  = 36;
    attribute_byte_table[563]  = 36;
    attribute_byte_table[564]  = 37;
    attribute_byte_table[565]  = 37;
    attribute_byte_table[566]  = 37;
    attribute_byte_table[567]  = 37;
    attribute_byte_table[568]  = 38;
    attribute_byte_table[569]  = 38;
    attribute_byte_table[570]  = 38;
    attribute_byte_table[571]  = 38;
    attribute_byte_table[572]  = 39;
    attribute_byte_table[573]  = 39;
    attribute_byte_table[574]  = 39;
    attribute_byte_table[575]  = 39;
    attribute_byte_table[576]  = 32;
    attribute_byte_table[577]  = 32;
    attribute_byte_table[578]  = 32;
    attribute_byte_table[579]  = 32;
    attribute_byte_table[580]  = 33;
    attribute_byte_table[581]  = 33;
    attribute_byte_table[582]  = 33;
    attribute_byte_table[583]  = 33;
    attribute_byte_table[584]  = 34;
    attribute_byte_table[585]  = 34;
    attribute_byte_table[586]  = 34;
    attribute_byte_table[587]  = 34;
    attribute_byte_table[588]  = 35;
    attribute_byte_table[589]  = 35;
    attribute_byte_table[590]  = 35;
    attribute_byte_table[591]  = 35;
    attribute_byte_table[592]  = 36;
    attribute_byte_table[593]  = 36;
    attribute_byte_table[594]  = 36;
    attribute_byte_table[595]  = 36;
    attribute_byte_table[596]  = 37;
    attribute_byte_table[597]  = 37;
    attribute_byte_table[598]  = 37;
    attribute_byte_table[599]  = 37;
    attribute_byte_table[600]  = 38;
    attribute_byte_table[601]  = 38;
    attribute_byte_table[602]  = 38;
    attribute_byte_table[603]  = 38;
    attribute_byte_table[604]  = 39;
    attribute_byte_table[605]  = 39;
    attribute_byte_table[606]  = 39;
    attribute_byte_table[607]  = 39;
    attribute_byte_table[608]  = 32;
    attribute_byte_table[609]  = 32;
    attribute_byte_table[610]  = 32;
    attribute_byte_table[611]  = 32;
    attribute_byte_table[612]  = 33;
    attribute_byte_table[613]  = 33;
    attribute_byte_table[614]  = 33;
    attribute_byte_table[615]  = 33;
    attribute_byte_table[616]  = 34;
    attribute_byte_table[617]  = 34;
    attribute_byte_table[618]  = 34;
    attribute_byte_table[619]  = 34;
    attribute_byte_table[620]  = 35;
    attribute_byte_table[621]  = 35;
    attribute_byte_table[622]  = 35;
    attribute_byte_table[623]  = 35;
    attribute_byte_table[624]  = 36;
    attribute_byte_table[625]  = 36;
    attribute_byte_table[626]  = 36;
    attribute_byte_table[627]  = 36;
    attribute_byte_table[628]  = 37;
    attribute_byte_table[629]  = 37;
    attribute_byte_table[630]  = 37;
    attribute_byte_table[631]  = 37;
    attribute_byte_table[632]  = 38;
    attribute_byte_table[633]  = 38;
    attribute_byte_table[634]  = 38;
    attribute_byte_table[635]  = 38;
    attribute_byte_table[636]  = 39;
    attribute_byte_table[637]  = 39;
    attribute_byte_table[638]  = 39;
    attribute_byte_table[639]  = 39;
    attribute_byte_table[640]  = 40;
    attribute_byte_table[641]  = 40;
    attribute_byte_table[642]  = 40;
    attribute_byte_table[643]  = 40;
    attribute_byte_table[644]  = 41;
    attribute_byte_table[645]  = 41;
    attribute_byte_table[646]  = 41;
    attribute_byte_table[647]  = 41;
    attribute_byte_table[648]  = 42;
    attribute_byte_table[649]  = 42;
    attribute_byte_table[650]  = 42;
    attribute_byte_table[651]  = 42;
    attribute_byte_table[652]  = 43;
    attribute_byte_table[653]  = 43;
    attribute_byte_table[654]  = 43;
    attribute_byte_table[655]  = 43;
    attribute_byte_table[656]  = 44;
    attribute_byte_table[657]  = 44;
    attribute_byte_table[658]  = 44;
    attribute_byte_table[659]  = 44;
    attribute_byte_table[660]  = 45;
    attribute_byte_table[661]  = 45;
    attribute_byte_table[662]  = 45;
    attribute_byte_table[663]  = 45;
    attribute_byte_table[664]  = 46;
    attribute_byte_table[665]  = 46;
    attribute_byte_table[666]  = 46;
    attribute_byte_table[667]  = 46;
    attribute_byte_table[668]  = 47;
    attribute_byte_table[669]  = 47;
    attribute_byte_table[670]  = 47;
    attribute_byte_table[671]  = 47;
    attribute_byte_table[672]  = 40;
    attribute_byte_table[673]  = 40;
    attribute_byte_table[674]  = 40;
    attribute_byte_table[675]  = 40;
    attribute_byte_table[676]  = 41;
    attribute_byte_table[677]  = 41;
    attribute_byte_table[678]  = 41;
    attribute_byte_table[679]  = 41;
    attribute_byte_table[680]  = 42;
    attribute_byte_table[681]  = 42;
    attribute_byte_table[682]  = 42;
    attribute_byte_table[683]  = 42;
    attribute_byte_table[684]  = 43;
    attribute_byte_table[685]  = 43;
    attribute_byte_table[686]  = 43;
    attribute_byte_table[687]  = 43;
    attribute_byte_table[688]  = 44;
    attribute_byte_table[689]  = 44;
    attribute_byte_table[690]  = 44;
    attribute_byte_table[691]  = 44;
    attribute_byte_table[692]  = 45;
    attribute_byte_table[693]  = 45;
    attribute_byte_table[694]  = 45;
    attribute_byte_table[695]  = 45;
    attribute_byte_table[696]  = 46;
    attribute_byte_table[697]  = 46;
    attribute_byte_table[698]  = 46;
    attribute_byte_table[699]  = 46;
    attribute_byte_table[700]  = 47;
    attribute_byte_table[701]  = 47;
    attribute_byte_table[702]  = 47;
    attribute_byte_table[703]  = 47;
    attribute_byte_table[704]  = 40;
    attribute_byte_table[705]  = 40;
    attribute_byte_table[706]  = 40;
    attribute_byte_table[707]  = 40;
    attribute_byte_table[708]  = 41;
    attribute_byte_table[709]  = 41;
    attribute_byte_table[710]  = 41;
    attribute_byte_table[711]  = 41;
    attribute_byte_table[712]  = 42;
    attribute_byte_table[713]  = 42;
    attribute_byte_table[714]  = 42;
    attribute_byte_table[715]  = 42;
    attribute_byte_table[716]  = 43;
    attribute_byte_table[717]  = 43;
    attribute_byte_table[718]  = 43;
    attribute_byte_table[719]  = 43;
    attribute_byte_table[720]  = 44;
    attribute_byte_table[721]  = 44;
    attribute_byte_table[722]  = 44;
    attribute_byte_table[723]  = 44;
    attribute_byte_table[724]  = 45;
    attribute_byte_table[725]  = 45;
    attribute_byte_table[726]  = 45;
    attribute_byte_table[727]  = 45;
    attribute_byte_table[728]  = 46;
    attribute_byte_table[729]  = 46;
    attribute_byte_table[730]  = 46;
    attribute_byte_table[731]  = 46;
    attribute_byte_table[732]  = 47;
    attribute_byte_table[733]  = 47;
    attribute_byte_table[734]  = 47;
    attribute_byte_table[735]  = 47;
    attribute_byte_table[736]  = 40;
    attribute_byte_table[737]  = 40;
    attribute_byte_table[738]  = 40;
    attribute_byte_table[739]  = 40;
    attribute_byte_table[740]  = 41;
    attribute_byte_table[741]  = 41;
    attribute_byte_table[742]  = 41;
    attribute_byte_table[743]  = 41;
    attribute_byte_table[744]  = 42;
    attribute_byte_table[745]  = 42;
    attribute_byte_table[746]  = 42;
    attribute_byte_table[747]  = 42;
    attribute_byte_table[748]  = 43;
    attribute_byte_table[749]  = 43;
    attribute_byte_table[750]  = 43;
    attribute_byte_table[751]  = 43;
    attribute_byte_table[752]  = 44;
    attribute_byte_table[753]  = 44;
    attribute_byte_table[754]  = 44;
    attribute_byte_table[755]  = 44;
    attribute_byte_table[756]  = 45;
    attribute_byte_table[757]  = 45;
    attribute_byte_table[758]  = 45;
    attribute_byte_table[759]  = 45;
    attribute_byte_table[760]  = 46;
    attribute_byte_table[761]  = 46;
    attribute_byte_table[762]  = 46;
    attribute_byte_table[763]  = 46;
    attribute_byte_table[764]  = 47;
    attribute_byte_table[765]  = 47;
    attribute_byte_table[766]  = 47;
    attribute_byte_table[767]  = 47;
    attribute_byte_table[768]  = 48;
    attribute_byte_table[769]  = 48;
    attribute_byte_table[770]  = 48;
    attribute_byte_table[771]  = 48;
    attribute_byte_table[772]  = 49;
    attribute_byte_table[773]  = 49;
    attribute_byte_table[774]  = 49;
    attribute_byte_table[775]  = 49;
    attribute_byte_table[776]  = 50;
    attribute_byte_table[777]  = 50;
    attribute_byte_table[778]  = 50;
    attribute_byte_table[779]  = 50;
    attribute_byte_table[780]  = 51;
    attribute_byte_table[781]  = 51;
    attribute_byte_table[782]  = 51;
    attribute_byte_table[783]  = 51;
    attribute_byte_table[784]  = 52;
    attribute_byte_table[785]  = 52;
    attribute_byte_table[786]  = 52;
    attribute_byte_table[787]  = 52;
    attribute_byte_table[788]  = 53;
    attribute_byte_table[789]  = 53;
    attribute_byte_table[790]  = 53;
    attribute_byte_table[791]  = 53;
    attribute_byte_table[792]  = 54;
    attribute_byte_table[793]  = 54;
    attribute_byte_table[794]  = 54;
    attribute_byte_table[795]  = 54;
    attribute_byte_table[796]  = 55;
    attribute_byte_table[797]  = 55;
    attribute_byte_table[798]  = 55;
    attribute_byte_table[799]  = 55;
    attribute_byte_table[800]  = 48;
    attribute_byte_table[801]  = 48;
    attribute_byte_table[802]  = 48;
    attribute_byte_table[803]  = 48;
    attribute_byte_table[804]  = 49;
    attribute_byte_table[805]  = 49;
    attribute_byte_table[806]  = 49;
    attribute_byte_table[807]  = 49;
    attribute_byte_table[808]  = 50;
    attribute_byte_table[809]  = 50;
    attribute_byte_table[810]  = 50;
    attribute_byte_table[811]  = 50;
    attribute_byte_table[812]  = 51;
    attribute_byte_table[813]  = 51;
    attribute_byte_table[814]  = 51;
    attribute_byte_table[815]  = 51;
    attribute_byte_table[816]  = 52;
    attribute_byte_table[817]  = 52;
    attribute_byte_table[818]  = 52;
    attribute_byte_table[819]  = 52;
    attribute_byte_table[820]  = 53;
    attribute_byte_table[821]  = 53;
    attribute_byte_table[822]  = 53;
    attribute_byte_table[823]  = 53;
    attribute_byte_table[824]  = 54;
    attribute_byte_table[825]  = 54;
    attribute_byte_table[826]  = 54;
    attribute_byte_table[827]  = 54;
    attribute_byte_table[828]  = 55;
    attribute_byte_table[829]  = 55;
    attribute_byte_table[830]  = 55;
    attribute_byte_table[831]  = 55;
    attribute_byte_table[832]  = 48;
    attribute_byte_table[833]  = 48;
    attribute_byte_table[834]  = 48;
    attribute_byte_table[835]  = 48;
    attribute_byte_table[836]  = 49;
    attribute_byte_table[837]  = 49;
    attribute_byte_table[838]  = 49;
    attribute_byte_table[839]  = 49;
    attribute_byte_table[840]  = 50;
    attribute_byte_table[841]  = 50;
    attribute_byte_table[842]  = 50;
    attribute_byte_table[843]  = 50;
    attribute_byte_table[844]  = 51;
    attribute_byte_table[845]  = 51;
    attribute_byte_table[846]  = 51;
    attribute_byte_table[847]  = 51;
    attribute_byte_table[848]  = 52;
    attribute_byte_table[849]  = 52;
    attribute_byte_table[850]  = 52;
    attribute_byte_table[851]  = 52;
    attribute_byte_table[852]  = 53;
    attribute_byte_table[853]  = 53;
    attribute_byte_table[854]  = 53;
    attribute_byte_table[855]  = 53;
    attribute_byte_table[856]  = 54;
    attribute_byte_table[857]  = 54;
    attribute_byte_table[858]  = 54;
    attribute_byte_table[859]  = 54;
    attribute_byte_table[860]  = 55;
    attribute_byte_table[861]  = 55;
    attribute_byte_table[862]  = 55;
    attribute_byte_table[863]  = 55;
    attribute_byte_table[864]  = 48;
    attribute_byte_table[865]  = 48;
    attribute_byte_table[866]  = 48;
    attribute_byte_table[867]  = 48;
    attribute_byte_table[868]  = 49;
    attribute_byte_table[869]  = 49;
    attribute_byte_table[870]  = 49;
    attribute_byte_table[871]  = 49;
    attribute_byte_table[872]  = 50;
    attribute_byte_table[873]  = 50;
    attribute_byte_table[874]  = 50;
    attribute_byte_table[875]  = 50;
    attribute_byte_table[876]  = 51;
    attribute_byte_table[877]  = 51;
    attribute_byte_table[878]  = 51;
    attribute_byte_table[879]  = 51;
    attribute_byte_table[880]  = 52;
    attribute_byte_table[881]  = 52;
    attribute_byte_table[882]  = 52;
    attribute_byte_table[883]  = 52;
    attribute_byte_table[884]  = 53;
    attribute_byte_table[885]  = 53;
    attribute_byte_table[886]  = 53;
    attribute_byte_table[887]  = 53;
    attribute_byte_table[888]  = 54;
    attribute_byte_table[889]  = 54;
    attribute_byte_table[890]  = 54;
    attribute_byte_table[891]  = 54;
    attribute_byte_table[892]  = 55;
    attribute_byte_table[893]  = 55;
    attribute_byte_table[894]  = 55;
    attribute_byte_table[895]  = 55;
    attribute_byte_table[896]  = 56;
    attribute_byte_table[897]  = 56;
    attribute_byte_table[898]  = 56;
    attribute_byte_table[899]  = 56;
    attribute_byte_table[900]  = 57;
    attribute_byte_table[901]  = 57;
    attribute_byte_table[902]  = 57;
    attribute_byte_table[903]  = 57;
    attribute_byte_table[904]  = 58;
    attribute_byte_table[905]  = 58;
    attribute_byte_table[906]  = 58;
    attribute_byte_table[907]  = 58;
    attribute_byte_table[908]  = 59;
    attribute_byte_table[909]  = 59;
    attribute_byte_table[910]  = 59;
    attribute_byte_table[911]  = 59;
    attribute_byte_table[912]  = 60;
    attribute_byte_table[913]  = 60;
    attribute_byte_table[914]  = 60;
    attribute_byte_table[915]  = 60;
    attribute_byte_table[916]  = 61;
    attribute_byte_table[917]  = 61;
    attribute_byte_table[918]  = 61;
    attribute_byte_table[919]  = 61;
    attribute_byte_table[920]  = 62;
    attribute_byte_table[921]  = 62;
    attribute_byte_table[922]  = 62;
    attribute_byte_table[923]  = 62;
    attribute_byte_table[924]  = 63;
    attribute_byte_table[925]  = 63;
    attribute_byte_table[926]  = 63;
    attribute_byte_table[927]  = 63;
    attribute_byte_table[928]  = 56;
    attribute_byte_table[929]  = 56;
    attribute_byte_table[930]  = 56;
    attribute_byte_table[931]  = 56;
    attribute_byte_table[932]  = 57;
    attribute_byte_table[933]  = 57;
    attribute_byte_table[934]  = 57;
    attribute_byte_table[935]  = 57;
    attribute_byte_table[936]  = 58;
    attribute_byte_table[937]  = 58;
    attribute_byte_table[938]  = 58;
    attribute_byte_table[939]  = 58;
    attribute_byte_table[940]  = 59;
    attribute_byte_table[941]  = 59;
    attribute_byte_table[942]  = 59;
    attribute_byte_table[943]  = 59;
    attribute_byte_table[944]  = 60;
    attribute_byte_table[945]  = 60;
    attribute_byte_table[946]  = 60;
    attribute_byte_table[947]  = 60;
    attribute_byte_table[948]  = 61;
    attribute_byte_table[949]  = 61;
    attribute_byte_table[950]  = 61;
    attribute_byte_table[951]  = 61;
    attribute_byte_table[952]  = 62;
    attribute_byte_table[953]  = 62;
    attribute_byte_table[954]  = 62;
    attribute_byte_table[955]  = 62;
    attribute_byte_table[956]  = 63;
    attribute_byte_table[957]  = 63;
    attribute_byte_table[958]  = 63;
    attribute_byte_table[959]  = 63;
}

void PPU::load_attribute_square_table() {
    attribute_square_table[0]   = 0;
    attribute_square_table[1]   = 0;
    attribute_square_table[32]  = 0;
    attribute_square_table[33]  = 0;
    attribute_square_table[128] = 0;
    attribute_square_table[129] = 0;
    attribute_square_table[160] = 0;
    attribute_square_table[161] = 0;
    attribute_square_table[256] = 0;
    attribute_square_table[257] = 0;
    attribute_square_table[288] = 0;
    attribute_square_table[289] = 0;
    attribute_square_table[384] = 0;
    attribute_square_table[385] = 0;
    attribute_square_table[416] = 0;
    attribute_square_table[417] = 0;
    attribute_square_table[512] = 0;
    attribute_square_table[513] = 0;
    attribute_square_table[544] = 0;
    attribute_square_table[545] = 0;
    attribute_square_table[640] = 0;
    attribute_square_table[641] = 0;
    attribute_square_table[672] = 0;
    attribute_square_table[673] = 0;
    attribute_square_table[768] = 0;
    attribute_square_table[769] = 0;
    attribute_square_table[800] = 0;
    attribute_square_table[801] = 0;
    attribute_square_table[896] = 0;
    attribute_square_table[897] = 0;
    attribute_square_table[928] = 0;
    attribute_square_table[929] = 0;
    attribute_square_table[4]   = 0;
    attribute_square_table[5]   = 0;
    attribute_square_table[36]  = 0;
    attribute_square_table[37]  = 0;
    attribute_square_table[132] = 0;
    attribute_square_table[133] = 0;
    attribute_square_table[164] = 0;
    attribute_square_table[165] = 0;
    attribute_square_table[260] = 0;
    attribute_square_table[261] = 0;
    attribute_square_table[292] = 0;
    attribute_square_table[293] = 0;
    attribute_square_table[388] = 0;
    attribute_square_table[389] = 0;
    attribute_square_table[420] = 0;
    attribute_square_table[421] = 0;
    attribute_square_table[516] = 0;
    attribute_square_table[517] = 0;
    attribute_square_table[548] = 0;
    attribute_square_table[549] = 0;
    attribute_square_table[644] = 0;
    attribute_square_table[645] = 0;
    attribute_square_table[676] = 0;
    attribute_square_table[677] = 0;
    attribute_square_table[772] = 0;
    attribute_square_table[773] = 0;
    attribute_square_table[804] = 0;
    attribute_square_table[805] = 0;
    attribute_square_table[900] = 0;
    attribute_square_table[901] = 0;
    attribute_square_table[932] = 0;
    attribute_square_table[933] = 0;
    attribute_square_table[8]   = 0;
    attribute_square_table[9]   = 0;
    attribute_square_table[40]  = 0;
    attribute_square_table[41]  = 0;
    attribute_square_table[136] = 0;
    attribute_square_table[137] = 0;
    attribute_square_table[168] = 0;
    attribute_square_table[169] = 0;
    attribute_square_table[264] = 0;
    attribute_square_table[265] = 0;
    attribute_square_table[296] = 0;
    attribute_square_table[297] = 0;
    attribute_square_table[392] = 0;
    attribute_square_table[393] = 0;
    attribute_square_table[424] = 0;
    attribute_square_table[425] = 0;
    attribute_square_table[520] = 0;
    attribute_square_table[521] = 0;
    attribute_square_table[552] = 0;
    attribute_square_table[553] = 0;
    attribute_square_table[648] = 0;
    attribute_square_table[649] = 0;
    attribute_square_table[680] = 0;
    attribute_square_table[681] = 0;
    attribute_square_table[776] = 0;
    attribute_square_table[777] = 0;
    attribute_square_table[808] = 0;
    attribute_square_table[809] = 0;
    attribute_square_table[904] = 0;
    attribute_square_table[905] = 0;
    attribute_square_table[936] = 0;
    attribute_square_table[937] = 0;
    attribute_square_table[12]  = 0;
    attribute_square_table[13]  = 0;
    attribute_square_table[44]  = 0;
    attribute_square_table[45]  = 0;
    attribute_square_table[140] = 0;
    attribute_square_table[141] = 0;
    attribute_square_table[172] = 0;
    attribute_square_table[173] = 0;
    attribute_square_table[268] = 0;
    attribute_square_table[269] = 0;
    attribute_square_table[300] = 0;
    attribute_square_table[301] = 0;
    attribute_square_table[396] = 0;
    attribute_square_table[397] = 0;
    attribute_square_table[428] = 0;
    attribute_square_table[429] = 0;
    attribute_square_table[524] = 0;
    attribute_square_table[525] = 0;
    attribute_square_table[556] = 0;
    attribute_square_table[557] = 0;
    attribute_square_table[652] = 0;
    attribute_square_table[653] = 0;
    attribute_square_table[684] = 0;
    attribute_square_table[685] = 0;
    attribute_square_table[780] = 0;
    attribute_square_table[781] = 0;
    attribute_square_table[812] = 0;
    attribute_square_table[813] = 0;
    attribute_square_table[908] = 0;
    attribute_square_table[909] = 0;
    attribute_square_table[940] = 0;
    attribute_square_table[941] = 0;
    attribute_square_table[16]  = 0;
    attribute_square_table[17]  = 0;
    attribute_square_table[48]  = 0;
    attribute_square_table[49]  = 0;
    attribute_square_table[144] = 0;
    attribute_square_table[145] = 0;
    attribute_square_table[176] = 0;
    attribute_square_table[177] = 0;
    attribute_square_table[272] = 0;
    attribute_square_table[273] = 0;
    attribute_square_table[304] = 0;
    attribute_square_table[305] = 0;
    attribute_square_table[400] = 0;
    attribute_square_table[401] = 0;
    attribute_square_table[432] = 0;
    attribute_square_table[433] = 0;
    attribute_square_table[528] = 0;
    attribute_square_table[529] = 0;
    attribute_square_table[560] = 0;
    attribute_square_table[561] = 0;
    attribute_square_table[656] = 0;
    attribute_square_table[657] = 0;
    attribute_square_table[688] = 0;
    attribute_square_table[689] = 0;
    attribute_square_table[784] = 0;
    attribute_square_table[785] = 0;
    attribute_square_table[816] = 0;
    attribute_square_table[817] = 0;
    attribute_square_table[912] = 0;
    attribute_square_table[913] = 0;
    attribute_square_table[944] = 0;
    attribute_square_table[945] = 0;
    attribute_square_table[20]  = 0;
    attribute_square_table[21]  = 0;
    attribute_square_table[52]  = 0;
    attribute_square_table[53]  = 0;
    attribute_square_table[148] = 0;
    attribute_square_table[149] = 0;
    attribute_square_table[180] = 0;
    attribute_square_table[181] = 0;
    attribute_square_table[276] = 0;
    attribute_square_table[277] = 0;
    attribute_square_table[308] = 0;
    attribute_square_table[309] = 0;
    attribute_square_table[404] = 0;
    attribute_square_table[405] = 0;
    attribute_square_table[436] = 0;
    attribute_square_table[437] = 0;
    attribute_square_table[532] = 0;
    attribute_square_table[533] = 0;
    attribute_square_table[564] = 0;
    attribute_square_table[565] = 0;
    attribute_square_table[660] = 0;
    attribute_square_table[661] = 0;
    attribute_square_table[692] = 0;
    attribute_square_table[693] = 0;
    attribute_square_table[788] = 0;
    attribute_square_table[789] = 0;
    attribute_square_table[820] = 0;
    attribute_square_table[821] = 0;
    attribute_square_table[916] = 0;
    attribute_square_table[917] = 0;
    attribute_square_table[948] = 0;
    attribute_square_table[949] = 0;
    attribute_square_table[24]  = 0;
    attribute_square_table[25]  = 0;
    attribute_square_table[56]  = 0;
    attribute_square_table[57]  = 0;
    attribute_square_table[152] = 0;
    attribute_square_table[153] = 0;
    attribute_square_table[184] = 0;
    attribute_square_table[185] = 0;
    attribute_square_table[280] = 0;
    attribute_square_table[281] = 0;
    attribute_square_table[312] = 0;
    attribute_square_table[313] = 0;
    attribute_square_table[408] = 0;
    attribute_square_table[409] = 0;
    attribute_square_table[440] = 0;
    attribute_square_table[441] = 0;
    attribute_square_table[536] = 0;
    attribute_square_table[537] = 0;
    attribute_square_table[568] = 0;
    attribute_square_table[569] = 0;
    attribute_square_table[664] = 0;
    attribute_square_table[665] = 0;
    attribute_square_table[696] = 0;
    attribute_square_table[697] = 0;
    attribute_square_table[792] = 0;
    attribute_square_table[793] = 0;
    attribute_square_table[824] = 0;
    attribute_square_table[825] = 0;
    attribute_square_table[920] = 0;
    attribute_square_table[921] = 0;
    attribute_square_table[952] = 0;
    attribute_square_table[953] = 0;
    attribute_square_table[28]  = 0;
    attribute_square_table[29]  = 0;
    attribute_square_table[60]  = 0;
    attribute_square_table[61]  = 0;
    attribute_square_table[156] = 0;
    attribute_square_table[157] = 0;
    attribute_square_table[188] = 0;
    attribute_square_table[189] = 0;
    attribute_square_table[284] = 0;
    attribute_square_table[285] = 0;
    attribute_square_table[316] = 0;
    attribute_square_table[317] = 0;
    attribute_square_table[412] = 0;
    attribute_square_table[413] = 0;
    attribute_square_table[444] = 0;
    attribute_square_table[445] = 0;
    attribute_square_table[540] = 0;
    attribute_square_table[541] = 0;
    attribute_square_table[572] = 0;
    attribute_square_table[573] = 0;
    attribute_square_table[668] = 0;
    attribute_square_table[669] = 0;
    attribute_square_table[700] = 0;
    attribute_square_table[701] = 0;
    attribute_square_table[796] = 0;
    attribute_square_table[797] = 0;
    attribute_square_table[828] = 0;
    attribute_square_table[829] = 0;
    attribute_square_table[924] = 0;
    attribute_square_table[925] = 0;
    attribute_square_table[956] = 0;
    attribute_square_table[957] = 0;
    attribute_square_table[2]   = 1;
    attribute_square_table[3]   = 1;
    attribute_square_table[34]  = 1;
    attribute_square_table[35]  = 1;
    attribute_square_table[130] = 1;
    attribute_square_table[131] = 1;
    attribute_square_table[162] = 1;
    attribute_square_table[163] = 1;
    attribute_square_table[258] = 1;
    attribute_square_table[259] = 1;
    attribute_square_table[290] = 1;
    attribute_square_table[291] = 1;
    attribute_square_table[386] = 1;
    attribute_square_table[387] = 1;
    attribute_square_table[418] = 1;
    attribute_square_table[419] = 1;
    attribute_square_table[514] = 1;
    attribute_square_table[515] = 1;
    attribute_square_table[546] = 1;
    attribute_square_table[547] = 1;
    attribute_square_table[642] = 1;
    attribute_square_table[643] = 1;
    attribute_square_table[674] = 1;
    attribute_square_table[675] = 1;
    attribute_square_table[770] = 1;
    attribute_square_table[771] = 1;
    attribute_square_table[802] = 1;
    attribute_square_table[803] = 1;
    attribute_square_table[898] = 1;
    attribute_square_table[899] = 1;
    attribute_square_table[930] = 1;
    attribute_square_table[931] = 1;
    attribute_square_table[6]   = 1;
    attribute_square_table[7]   = 1;
    attribute_square_table[38]  = 1;
    attribute_square_table[39]  = 1;
    attribute_square_table[134] = 1;
    attribute_square_table[135] = 1;
    attribute_square_table[166] = 1;
    attribute_square_table[167] = 1;
    attribute_square_table[262] = 1;
    attribute_square_table[263] = 1;
    attribute_square_table[294] = 1;
    attribute_square_table[295] = 1;
    attribute_square_table[390] = 1;
    attribute_square_table[391] = 1;
    attribute_square_table[422] = 1;
    attribute_square_table[423] = 1;
    attribute_square_table[518] = 1;
    attribute_square_table[519] = 1;
    attribute_square_table[550] = 1;
    attribute_square_table[551] = 1;
    attribute_square_table[646] = 1;
    attribute_square_table[647] = 1;
    attribute_square_table[678] = 1;
    attribute_square_table[679] = 1;
    attribute_square_table[774] = 1;
    attribute_square_table[775] = 1;
    attribute_square_table[806] = 1;
    attribute_square_table[807] = 1;
    attribute_square_table[902] = 1;
    attribute_square_table[903] = 1;
    attribute_square_table[934] = 1;
    attribute_square_table[935] = 1;
    attribute_square_table[10]  = 1;
    attribute_square_table[11]  = 1;
    attribute_square_table[42]  = 1;
    attribute_square_table[43]  = 1;
    attribute_square_table[138] = 1;
    attribute_square_table[139] = 1;
    attribute_square_table[170] = 1;
    attribute_square_table[171] = 1;
    attribute_square_table[266] = 1;
    attribute_square_table[267] = 1;
    attribute_square_table[298] = 1;
    attribute_square_table[299] = 1;
    attribute_square_table[394] = 1;
    attribute_square_table[395] = 1;
    attribute_square_table[426] = 1;
    attribute_square_table[427] = 1;
    attribute_square_table[522] = 1;
    attribute_square_table[523] = 1;
    attribute_square_table[554] = 1;
    attribute_square_table[555] = 1;
    attribute_square_table[650] = 1;
    attribute_square_table[651] = 1;
    attribute_square_table[682] = 1;
    attribute_square_table[683] = 1;
    attribute_square_table[778] = 1;
    attribute_square_table[779] = 1;
    attribute_square_table[810] = 1;
    attribute_square_table[811] = 1;
    attribute_square_table[906] = 1;
    attribute_square_table[907] = 1;
    attribute_square_table[938] = 1;
    attribute_square_table[939] = 1;
    attribute_square_table[14]  = 1;
    attribute_square_table[15]  = 1;
    attribute_square_table[46]  = 1;
    attribute_square_table[47]  = 1;
    attribute_square_table[142] = 1;
    attribute_square_table[143] = 1;
    attribute_square_table[174] = 1;
    attribute_square_table[175] = 1;
    attribute_square_table[270] = 1;
    attribute_square_table[271] = 1;
    attribute_square_table[302] = 1;
    attribute_square_table[303] = 1;
    attribute_square_table[398] = 1;
    attribute_square_table[399] = 1;
    attribute_square_table[430] = 1;
    attribute_square_table[431] = 1;
    attribute_square_table[526] = 1;
    attribute_square_table[527] = 1;
    attribute_square_table[558] = 1;
    attribute_square_table[559] = 1;
    attribute_square_table[654] = 1;
    attribute_square_table[655] = 1;
    attribute_square_table[686] = 1;
    attribute_square_table[687] = 1;
    attribute_square_table[782] = 1;
    attribute_square_table[783] = 1;
    attribute_square_table[814] = 1;
    attribute_square_table[815] = 1;
    attribute_square_table[910] = 1;
    attribute_square_table[911] = 1;
    attribute_square_table[942] = 1;
    attribute_square_table[943] = 1;
    attribute_square_table[18]  = 1;
    attribute_square_table[19]  = 1;
    attribute_square_table[50]  = 1;
    attribute_square_table[51]  = 1;
    attribute_square_table[146] = 1;
    attribute_square_table[147] = 1;
    attribute_square_table[178] = 1;
    attribute_square_table[179] = 1;
    attribute_square_table[274] = 1;
    attribute_square_table[275] = 1;
    attribute_square_table[306] = 1;
    attribute_square_table[307] = 1;
    attribute_square_table[402] = 1;
    attribute_square_table[403] = 1;
    attribute_square_table[434] = 1;
    attribute_square_table[435] = 1;
    attribute_square_table[530] = 1;
    attribute_square_table[531] = 1;
    attribute_square_table[562] = 1;
    attribute_square_table[563] = 1;
    attribute_square_table[658] = 1;
    attribute_square_table[659] = 1;
    attribute_square_table[690] = 1;
    attribute_square_table[691] = 1;
    attribute_square_table[786] = 1;
    attribute_square_table[787] = 1;
    attribute_square_table[818] = 1;
    attribute_square_table[819] = 1;
    attribute_square_table[914] = 1;
    attribute_square_table[915] = 1;
    attribute_square_table[946] = 1;
    attribute_square_table[947] = 1;
    attribute_square_table[22]  = 1;
    attribute_square_table[23]  = 1;
    attribute_square_table[54]  = 1;
    attribute_square_table[55]  = 1;
    attribute_square_table[150] = 1;
    attribute_square_table[151] = 1;
    attribute_square_table[182] = 1;
    attribute_square_table[183] = 1;
    attribute_square_table[278] = 1;
    attribute_square_table[279] = 1;
    attribute_square_table[310] = 1;
    attribute_square_table[311] = 1;
    attribute_square_table[406] = 1;
    attribute_square_table[407] = 1;
    attribute_square_table[438] = 1;
    attribute_square_table[439] = 1;
    attribute_square_table[534] = 1;
    attribute_square_table[535] = 1;
    attribute_square_table[566] = 1;
    attribute_square_table[567] = 1;
    attribute_square_table[662] = 1;
    attribute_square_table[663] = 1;
    attribute_square_table[694] = 1;
    attribute_square_table[695] = 1;
    attribute_square_table[790] = 1;
    attribute_square_table[791] = 1;
    attribute_square_table[822] = 1;
    attribute_square_table[823] = 1;
    attribute_square_table[918] = 1;
    attribute_square_table[919] = 1;
    attribute_square_table[950] = 1;
    attribute_square_table[951] = 1;
    attribute_square_table[26]  = 1;
    attribute_square_table[27]  = 1;
    attribute_square_table[58]  = 1;
    attribute_square_table[59]  = 1;
    attribute_square_table[154] = 1;
    attribute_square_table[155] = 1;
    attribute_square_table[186] = 1;
    attribute_square_table[187] = 1;
    attribute_square_table[282] = 1;
    attribute_square_table[283] = 1;
    attribute_square_table[314] = 1;
    attribute_square_table[315] = 1;
    attribute_square_table[410] = 1;
    attribute_square_table[411] = 1;
    attribute_square_table[442] = 1;
    attribute_square_table[443] = 1;
    attribute_square_table[538] = 1;
    attribute_square_table[539] = 1;
    attribute_square_table[570] = 1;
    attribute_square_table[571] = 1;
    attribute_square_table[666] = 1;
    attribute_square_table[667] = 1;
    attribute_square_table[698] = 1;
    attribute_square_table[699] = 1;
    attribute_square_table[794] = 1;
    attribute_square_table[795] = 1;
    attribute_square_table[826] = 1;
    attribute_square_table[827] = 1;
    attribute_square_table[922] = 1;
    attribute_square_table[923] = 1;
    attribute_square_table[954] = 1;
    attribute_square_table[955] = 1;
    attribute_square_table[30]  = 1;
    attribute_square_table[31]  = 1;
    attribute_square_table[62]  = 1;
    attribute_square_table[63]  = 1;
    attribute_square_table[158] = 1;
    attribute_square_table[159] = 1;
    attribute_square_table[190] = 1;
    attribute_square_table[191] = 1;
    attribute_square_table[286] = 1;
    attribute_square_table[287] = 1;
    attribute_square_table[318] = 1;
    attribute_square_table[319] = 1;
    attribute_square_table[414] = 1;
    attribute_square_table[415] = 1;
    attribute_square_table[446] = 1;
    attribute_square_table[447] = 1;
    attribute_square_table[542] = 1;
    attribute_square_table[543] = 1;
    attribute_square_table[574] = 1;
    attribute_square_table[575] = 1;
    attribute_square_table[670] = 1;
    attribute_square_table[671] = 1;
    attribute_square_table[702] = 1;
    attribute_square_table[703] = 1;
    attribute_square_table[798] = 1;
    attribute_square_table[799] = 1;
    attribute_square_table[830] = 1;
    attribute_square_table[831] = 1;
    attribute_square_table[926] = 1;
    attribute_square_table[927] = 1;
    attribute_square_table[958] = 1;
    attribute_square_table[959] = 1;
    attribute_square_table[64]  = 2;
    attribute_square_table[65]  = 2;
    attribute_square_table[96]  = 2;
    attribute_square_table[97]  = 2;
    attribute_square_table[192] = 2;
    attribute_square_table[193] = 2;
    attribute_square_table[224] = 2;
    attribute_square_table[225] = 2;
    attribute_square_table[320] = 2;
    attribute_square_table[321] = 2;
    attribute_square_table[352] = 2;
    attribute_square_table[353] = 2;
    attribute_square_table[448] = 2;
    attribute_square_table[449] = 2;
    attribute_square_table[480] = 2;
    attribute_square_table[481] = 2;
    attribute_square_table[576] = 2;
    attribute_square_table[577] = 2;
    attribute_square_table[608] = 2;
    attribute_square_table[609] = 2;
    attribute_square_table[704] = 2;
    attribute_square_table[705] = 2;
    attribute_square_table[736] = 2;
    attribute_square_table[737] = 2;
    attribute_square_table[832] = 2;
    attribute_square_table[833] = 2;
    attribute_square_table[864] = 2;
    attribute_square_table[865] = 2;
    attribute_square_table[68]  = 2;
    attribute_square_table[69]  = 2;
    attribute_square_table[100] = 2;
    attribute_square_table[101] = 2;
    attribute_square_table[196] = 2;
    attribute_square_table[197] = 2;
    attribute_square_table[228] = 2;
    attribute_square_table[229] = 2;
    attribute_square_table[324] = 2;
    attribute_square_table[325] = 2;
    attribute_square_table[356] = 2;
    attribute_square_table[357] = 2;
    attribute_square_table[452] = 2;
    attribute_square_table[453] = 2;
    attribute_square_table[484] = 2;
    attribute_square_table[485] = 2;
    attribute_square_table[580] = 2;
    attribute_square_table[581] = 2;
    attribute_square_table[612] = 2;
    attribute_square_table[613] = 2;
    attribute_square_table[708] = 2;
    attribute_square_table[709] = 2;
    attribute_square_table[740] = 2;
    attribute_square_table[741] = 2;
    attribute_square_table[836] = 2;
    attribute_square_table[837] = 2;
    attribute_square_table[868] = 2;
    attribute_square_table[869] = 2;
    attribute_square_table[72]  = 2;
    attribute_square_table[73]  = 2;
    attribute_square_table[104] = 2;
    attribute_square_table[105] = 2;
    attribute_square_table[200] = 2;
    attribute_square_table[201] = 2;
    attribute_square_table[232] = 2;
    attribute_square_table[233] = 2;
    attribute_square_table[328] = 2;
    attribute_square_table[329] = 2;
    attribute_square_table[360] = 2;
    attribute_square_table[361] = 2;
    attribute_square_table[456] = 2;
    attribute_square_table[457] = 2;
    attribute_square_table[488] = 2;
    attribute_square_table[489] = 2;
    attribute_square_table[584] = 2;
    attribute_square_table[585] = 2;
    attribute_square_table[616] = 2;
    attribute_square_table[617] = 2;
    attribute_square_table[712] = 2;
    attribute_square_table[713] = 2;
    attribute_square_table[744] = 2;
    attribute_square_table[745] = 2;
    attribute_square_table[840] = 2;
    attribute_square_table[841] = 2;
    attribute_square_table[872] = 2;
    attribute_square_table[873] = 2;
    attribute_square_table[76]  = 2;
    attribute_square_table[77]  = 2;
    attribute_square_table[108] = 2;
    attribute_square_table[109] = 2;
    attribute_square_table[204] = 2;
    attribute_square_table[205] = 2;
    attribute_square_table[236] = 2;
    attribute_square_table[237] = 2;
    attribute_square_table[332] = 2;
    attribute_square_table[333] = 2;
    attribute_square_table[364] = 2;
    attribute_square_table[365] = 2;
    attribute_square_table[460] = 2;
    attribute_square_table[461] = 2;
    attribute_square_table[492] = 2;
    attribute_square_table[493] = 2;
    attribute_square_table[588] = 2;
    attribute_square_table[589] = 2;
    attribute_square_table[620] = 2;
    attribute_square_table[621] = 2;
    attribute_square_table[716] = 2;
    attribute_square_table[717] = 2;
    attribute_square_table[748] = 2;
    attribute_square_table[749] = 2;
    attribute_square_table[844] = 2;
    attribute_square_table[845] = 2;
    attribute_square_table[876] = 2;
    attribute_square_table[877] = 2;
    attribute_square_table[80]  = 2;
    attribute_square_table[81]  = 2;
    attribute_square_table[112] = 2;
    attribute_square_table[113] = 2;
    attribute_square_table[208] = 2;
    attribute_square_table[209] = 2;
    attribute_square_table[240] = 2;
    attribute_square_table[241] = 2;
    attribute_square_table[336] = 2;
    attribute_square_table[337] = 2;
    attribute_square_table[368] = 2;
    attribute_square_table[369] = 2;
    attribute_square_table[464] = 2;
    attribute_square_table[465] = 2;
    attribute_square_table[496] = 2;
    attribute_square_table[497] = 2;
    attribute_square_table[592] = 2;
    attribute_square_table[593] = 2;
    attribute_square_table[624] = 2;
    attribute_square_table[625] = 2;
    attribute_square_table[720] = 2;
    attribute_square_table[721] = 2;
    attribute_square_table[752] = 2;
    attribute_square_table[753] = 2;
    attribute_square_table[848] = 2;
    attribute_square_table[849] = 2;
    attribute_square_table[880] = 2;
    attribute_square_table[881] = 2;
    attribute_square_table[84]  = 2;
    attribute_square_table[85]  = 2;
    attribute_square_table[116] = 2;
    attribute_square_table[117] = 2;
    attribute_square_table[212] = 2;
    attribute_square_table[213] = 2;
    attribute_square_table[244] = 2;
    attribute_square_table[245] = 2;
    attribute_square_table[340] = 2;
    attribute_square_table[341] = 2;
    attribute_square_table[372] = 2;
    attribute_square_table[373] = 2;
    attribute_square_table[468] = 2;
    attribute_square_table[469] = 2;
    attribute_square_table[500] = 2;
    attribute_square_table[501] = 2;
    attribute_square_table[596] = 2;
    attribute_square_table[597] = 2;
    attribute_square_table[628] = 2;
    attribute_square_table[629] = 2;
    attribute_square_table[724] = 2;
    attribute_square_table[725] = 2;
    attribute_square_table[756] = 2;
    attribute_square_table[757] = 2;
    attribute_square_table[852] = 2;
    attribute_square_table[853] = 2;
    attribute_square_table[884] = 2;
    attribute_square_table[885] = 2;
    attribute_square_table[88]  = 2;
    attribute_square_table[89]  = 2;
    attribute_square_table[120] = 2;
    attribute_square_table[121] = 2;
    attribute_square_table[216] = 2;
    attribute_square_table[217] = 2;
    attribute_square_table[248] = 2;
    attribute_square_table[249] = 2;
    attribute_square_table[344] = 2;
    attribute_square_table[345] = 2;
    attribute_square_table[376] = 2;
    attribute_square_table[377] = 2;
    attribute_square_table[472] = 2;
    attribute_square_table[473] = 2;
    attribute_square_table[504] = 2;
    attribute_square_table[505] = 2;
    attribute_square_table[600] = 2;
    attribute_square_table[601] = 2;
    attribute_square_table[632] = 2;
    attribute_square_table[633] = 2;
    attribute_square_table[728] = 2;
    attribute_square_table[729] = 2;
    attribute_square_table[760] = 2;
    attribute_square_table[761] = 2;
    attribute_square_table[856] = 2;
    attribute_square_table[857] = 2;
    attribute_square_table[888] = 2;
    attribute_square_table[889] = 2;
    attribute_square_table[92]  = 2;
    attribute_square_table[93]  = 2;
    attribute_square_table[124] = 2;
    attribute_square_table[125] = 2;
    attribute_square_table[220] = 2;
    attribute_square_table[221] = 2;
    attribute_square_table[252] = 2;
    attribute_square_table[253] = 2;
    attribute_square_table[348] = 2;
    attribute_square_table[349] = 2;
    attribute_square_table[380] = 2;
    attribute_square_table[381] = 2;
    attribute_square_table[476] = 2;
    attribute_square_table[477] = 2;
    attribute_square_table[508] = 2;
    attribute_square_table[509] = 2;
    attribute_square_table[604] = 2;
    attribute_square_table[605] = 2;
    attribute_square_table[636] = 2;
    attribute_square_table[637] = 2;
    attribute_square_table[732] = 2;
    attribute_square_table[733] = 2;
    attribute_square_table[764] = 2;
    attribute_square_table[765] = 2;
    attribute_square_table[860] = 2;
    attribute_square_table[861] = 2;
    attribute_square_table[892] = 2;
    attribute_square_table[893] = 2;
    attribute_square_table[66]  = 3;
    attribute_square_table[67]  = 3;
    attribute_square_table[98]  = 3;
    attribute_square_table[99]  = 3;
    attribute_square_table[194] = 3;
    attribute_square_table[195] = 3;
    attribute_square_table[226] = 3;
    attribute_square_table[227] = 3;
    attribute_square_table[322] = 3;
    attribute_square_table[323] = 3;
    attribute_square_table[354] = 3;
    attribute_square_table[355] = 3;
    attribute_square_table[450] = 3;
    attribute_square_table[451] = 3;
    attribute_square_table[482] = 3;
    attribute_square_table[483] = 3;
    attribute_square_table[578] = 3;
    attribute_square_table[579] = 3;
    attribute_square_table[610] = 3;
    attribute_square_table[611] = 3;
    attribute_square_table[706] = 3;
    attribute_square_table[707] = 3;
    attribute_square_table[738] = 3;
    attribute_square_table[739] = 3;
    attribute_square_table[834] = 3;
    attribute_square_table[835] = 3;
    attribute_square_table[866] = 3;
    attribute_square_table[867] = 3;
    attribute_square_table[70]  = 3;
    attribute_square_table[71]  = 3;
    attribute_square_table[102] = 3;
    attribute_square_table[103] = 3;
    attribute_square_table[198] = 3;
    attribute_square_table[199] = 3;
    attribute_square_table[230] = 3;
    attribute_square_table[231] = 3;
    attribute_square_table[326] = 3;
    attribute_square_table[327] = 3;
    attribute_square_table[358] = 3;
    attribute_square_table[359] = 3;
    attribute_square_table[454] = 3;
    attribute_square_table[455] = 3;
    attribute_square_table[486] = 3;
    attribute_square_table[487] = 3;
    attribute_square_table[582] = 3;
    attribute_square_table[583] = 3;
    attribute_square_table[614] = 3;
    attribute_square_table[615] = 3;
    attribute_square_table[710] = 3;
    attribute_square_table[711] = 3;
    attribute_square_table[742] = 3;
    attribute_square_table[743] = 3;
    attribute_square_table[838] = 3;
    attribute_square_table[839] = 3;
    attribute_square_table[870] = 3;
    attribute_square_table[871] = 3;
    attribute_square_table[74]  = 3;
    attribute_square_table[75]  = 3;
    attribute_square_table[106] = 3;
    attribute_square_table[107] = 3;
    attribute_square_table[202] = 3;
    attribute_square_table[203] = 3;
    attribute_square_table[234] = 3;
    attribute_square_table[235] = 3;
    attribute_square_table[330] = 3;
    attribute_square_table[331] = 3;
    attribute_square_table[362] = 3;
    attribute_square_table[363] = 3;
    attribute_square_table[458] = 3;
    attribute_square_table[459] = 3;
    attribute_square_table[490] = 3;
    attribute_square_table[491] = 3;
    attribute_square_table[586] = 3;
    attribute_square_table[587] = 3;
    attribute_square_table[618] = 3;
    attribute_square_table[619] = 3;
    attribute_square_table[714] = 3;
    attribute_square_table[715] = 3;
    attribute_square_table[746] = 3;
    attribute_square_table[747] = 3;
    attribute_square_table[842] = 3;
    attribute_square_table[843] = 3;
    attribute_square_table[874] = 3;
    attribute_square_table[875] = 3;
    attribute_square_table[78]  = 3;
    attribute_square_table[79]  = 3;
    attribute_square_table[110] = 3;
    attribute_square_table[111] = 3;
    attribute_square_table[206] = 3;
    attribute_square_table[207] = 3;
    attribute_square_table[238] = 3;
    attribute_square_table[239] = 3;
    attribute_square_table[334] = 3;
    attribute_square_table[335] = 3;
    attribute_square_table[366] = 3;
    attribute_square_table[367] = 3;
    attribute_square_table[462] = 3;
    attribute_square_table[463] = 3;
    attribute_square_table[494] = 3;
    attribute_square_table[495] = 3;
    attribute_square_table[590] = 3;
    attribute_square_table[591] = 3;
    attribute_square_table[622] = 3;
    attribute_square_table[623] = 3;
    attribute_square_table[718] = 3;
    attribute_square_table[719] = 3;
    attribute_square_table[750] = 3;
    attribute_square_table[751] = 3;
    attribute_square_table[846] = 3;
    attribute_square_table[847] = 3;
    attribute_square_table[878] = 3;
    attribute_square_table[879] = 3;
    attribute_square_table[82]  = 3;
    attribute_square_table[83]  = 3;
    attribute_square_table[114] = 3;
    attribute_square_table[115] = 3;
    attribute_square_table[210] = 3;
    attribute_square_table[211] = 3;
    attribute_square_table[242] = 3;
    attribute_square_table[243] = 3;
    attribute_square_table[338] = 3;
    attribute_square_table[339] = 3;
    attribute_square_table[370] = 3;
    attribute_square_table[371] = 3;
    attribute_square_table[466] = 3;
    attribute_square_table[467] = 3;
    attribute_square_table[498] = 3;
    attribute_square_table[499] = 3;
    attribute_square_table[594] = 3;
    attribute_square_table[595] = 3;
    attribute_square_table[626] = 3;
    attribute_square_table[627] = 3;
    attribute_square_table[722] = 3;
    attribute_square_table[723] = 3;
    attribute_square_table[754] = 3;
    attribute_square_table[755] = 3;
    attribute_square_table[850] = 3;
    attribute_square_table[851] = 3;
    attribute_square_table[882] = 3;
    attribute_square_table[883] = 3;
    attribute_square_table[86]  = 3;
    attribute_square_table[87]  = 3;
    attribute_square_table[118] = 3;
    attribute_square_table[119] = 3;
    attribute_square_table[214] = 3;
    attribute_square_table[215] = 3;
    attribute_square_table[246] = 3;
    attribute_square_table[247] = 3;
    attribute_square_table[342] = 3;
    attribute_square_table[343] = 3;
    attribute_square_table[374] = 3;
    attribute_square_table[375] = 3;
    attribute_square_table[470] = 3;
    attribute_square_table[471] = 3;
    attribute_square_table[502] = 3;
    attribute_square_table[503] = 3;
    attribute_square_table[598] = 3;
    attribute_square_table[599] = 3;
    attribute_square_table[630] = 3;
    attribute_square_table[631] = 3;
    attribute_square_table[726] = 3;
    attribute_square_table[727] = 3;
    attribute_square_table[758] = 3;
    attribute_square_table[759] = 3;
    attribute_square_table[854] = 3;
    attribute_square_table[855] = 3;
    attribute_square_table[886] = 3;
    attribute_square_table[887] = 3;
    attribute_square_table[90]  = 3;
    attribute_square_table[91]  = 3;
    attribute_square_table[122] = 3;
    attribute_square_table[123] = 3;
    attribute_square_table[218] = 3;
    attribute_square_table[219] = 3;
    attribute_square_table[250] = 3;
    attribute_square_table[251] = 3;
    attribute_square_table[346] = 3;
    attribute_square_table[347] = 3;
    attribute_square_table[378] = 3;
    attribute_square_table[379] = 3;
    attribute_square_table[474] = 3;
    attribute_square_table[475] = 3;
    attribute_square_table[506] = 3;
    attribute_square_table[507] = 3;
    attribute_square_table[602] = 3;
    attribute_square_table[603] = 3;
    attribute_square_table[634] = 3;
    attribute_square_table[635] = 3;
    attribute_square_table[730] = 3;
    attribute_square_table[731] = 3;
    attribute_square_table[762] = 3;
    attribute_square_table[763] = 3;
    attribute_square_table[858] = 3;
    attribute_square_table[859] = 3;
    attribute_square_table[890] = 3;
    attribute_square_table[891] = 3;
    attribute_square_table[94]  = 3;
    attribute_square_table[95]  = 3;
    attribute_square_table[126] = 3;
    attribute_square_table[127] = 3;
    attribute_square_table[222] = 3;
    attribute_square_table[223] = 3;
    attribute_square_table[254] = 3;
    attribute_square_table[255] = 3;
    attribute_square_table[350] = 3;
    attribute_square_table[351] = 3;
    attribute_square_table[382] = 3;
    attribute_square_table[383] = 3;
    attribute_square_table[478] = 3;
    attribute_square_table[479] = 3;
    attribute_square_table[510] = 3;
    attribute_square_table[511] = 3;
    attribute_square_table[606] = 3;
    attribute_square_table[607] = 3;
    attribute_square_table[638] = 3;
    attribute_square_table[639] = 3;
    attribute_square_table[734] = 3;
    attribute_square_table[735] = 3;
    attribute_square_table[766] = 3;
    attribute_square_table[767] = 3;
    attribute_square_table[862] = 3;
    attribute_square_table[863] = 3;
    attribute_square_table[894] = 3;
    attribute_square_table[895] = 3;
}
