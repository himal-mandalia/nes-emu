// iNES Format (.NES)
//  ---------------------
//    +--------+------+------------------------------------------+
//    | Offset | Size | Content(s)                               |
//    +--------+------+------------------------------------------+
//    |   0    |  3   | 'NES'                                    |
//    |   3    |  1   | $1A                                      |
//    |   4    |  1   | 16K PRG-ROM page count                   |
//    |   5    |  1   | 8K CHR-ROM page count                    |
//    |   6    |  1   | ROM Control Byte #1                      |
//    |        |      |   %####vTsM                              |
//    |        |      |    |  ||||+- 0=Horizontal mirroring      |
//    |        |      |    |  ||||   1=Vertical mirroring        |
//    |        |      |    |  |||+-- 1=SRAM enabled              |
//    |        |      |    |  ||+--- 1=512-byte trainer present  |
//    |        |      |    |  |+---- 1=Four-screen mirroring     |
//    |        |      |    |  |                                  |
//    |        |      |    +--+----- Mapper # (lower 4-bits)     |
//    |   7    |  1   | ROM Control Byte #2                      |
//    |        |      |   %####0000                              |
//    |        |      |    |  |                                  |
//    |        |      |    +--+----- Mapper # (upper 4-bits)     |
//    |  8-15  |  8   | $00                                      |
//    | 16-..  |      | Actual 16K PRG-ROM pages (in linear      |
//    |  ...   |      | order). If a trainer exists, it precedes |
//    |  ...   |      | the first PRG-ROM page.                  |
//    | ..-EOF |      | CHR-ROM pages (in ascending order).      |
//    +--------+------+------------------------------------------+

#ifndef ROM_H
#define ROM_H

#include "Constants.h"
#include <fstream>

class ROM {
private:
    Byte rom[ROM_SIZE];
    unsigned int length;
    
    Byte* PRG_bank_1;
    Byte* PRG_bank_2;
    Byte* CHR_bank;
    
    Byte num_PRG_banks;
    Byte num_CHR_banks;
    
    Byte mapper;
    
    Byte mirroring;
    
public:
    ROM() {};
    
    void load_ROM(const char* file);
    
    Byte* get_PRG_bank_1() const { return PRG_bank_1; }
    
    Byte* get_PRG_bank_2() const { return PRG_bank_2; }
    
    Byte* get_CHR_bank() const { return CHR_bank; }
    
    Byte get_num_PRG_banks() const { return num_PRG_banks; }
    
    Byte get_num_CHR_banks() const { return num_CHR_banks; }
    
    Byte get_mirroring() const { return mirroring; }
};

#endif // ROM_H
