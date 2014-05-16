#include "ROM.h"

void ROM::load_ROM(const char* file) {
    // Open ROM file for reading
    ifstream in_file(file, ios::in | ios::binary);
    
    length = 0;
    // Load ROM into array
    while(in_file.good()) rom[length++] = in_file.get();
    in_file.close();

    if(length == 0) throw "Couldn't load ROM";
    
    Byte identifier[] = { 'N', 'E', 'S', 0x1A };

    for(int i = 0; i < 4; i++)
        if(rom[i] != identifier[i]) throw "Not a valid iNES ROM image";
    
    num_PRG_banks = rom[4];
    num_CHR_banks = rom[5];
    
    PRG_bank_1 = &rom[PRG_START];
    
    if(num_PRG_banks > 1) PRG_bank_2 = &rom[PRG_START + PRG_BANK_SIZE];
    
    CHR_bank = &rom[PRG_START + (PRG_BANK_SIZE * (num_PRG_banks == 1 ? 1: 2))];
    
    // needs testing - SINGLE SCREEN AND FOUR SCREEN NOT REGISTERING
    mirroring = (rom[6] & 1) | (rom[6] & 8);
    
    // needs testing
    mapper = ((rom[6] >> 4) & 0xF) | (rom[7] & 0xF0);
    
    //if(mapper != 0)
    //  throw "Only ROMs using mapper 0 (no mapper) supported at this time";
}
