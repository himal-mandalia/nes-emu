#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>

// Remove when this becomes unnecessary
using namespace std;

typedef unsigned char Byte;
typedef unsigned short Word;
typedef bool Flag;

const int ROM_SIZE                  = 0x10000;
const int PRG_START                 = 0x10;
const int PRG_BANK_SIZE             = 0x4000;
const int CHR_BANK_SIZE             = 0x2000;

// check these
const int HORIZONTAL_MIRRORING      = 0;
const int VERTICAL_MIRRORING        = 1;
const int SINGLE_SCREEN_MIRRORING   = 2;
const int FOUR_SCREEN_MIRRORING     = 3;

const int CPU_MEM_SIZE              = 0x10000;

enum BUTTONS {
    BUTTON_A, BUTTON_B,
    BUTTON_SELECT, BUTTON_START,
    BUTTON_UP, BUTTON_DOWN,
    BUTTON_LEFT, BUTTON_RIGHT
};

enum BUTTON_STATE {
    RELEASED, PRESSED
};

// Colour palettes

// Taken from:
// http://www.thealmightyguru.com/Games/Hacking/Docs/NES-NTSC-Palette.txt
const int NTSC_HEX_PALETTE[] = {
    0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC,
    0x940084, 0xA80020, 0xA81000, 0x881400,
    0x503000, 0x007800, 0x006800, 0x005800,
    0x004058, 0x000000, 0x000000, 0x000000,
    0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC,
    0xD800CC, 0xE40058, 0xF83800, 0xE45C10,
    0xAC7C00, 0x00B800, 0x00A800, 0x00A844,
    0x008888, 0x000000, 0x000000, 0x000000,
    0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8,
    0xF878F8, 0xF85898, 0xF87858, 0xFCA044,
    0xF8B800, 0xB8F818, 0x58D854, 0x58F898,
    0x00E8D8, 0x787878, 0x000000, 0x000000,
    0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8,
    0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,
    0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8,
    0x00FCFC, 0xF8D8F8, 0x000000, 0x000000
};

enum RGB {
    RED, GREEN, BLUE
};

const int NTSC_RGB_PALETTE[][3] = {
    {124, 124, 124},
    {0, 0, 252},
    {0, 0, 188},
    {68, 40, 188},
    {148, 0, 132},
    {168, 0, 32},
    {168, 16, 0},
    {136, 20, 0},
    {80, 48, 0},
    {0, 120, 0},
    {0, 104, 0},
    {0, 88, 0},
    {0, 64, 88},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {188, 188, 188},
    {0, 120, 248},
    {0, 88, 248},
    {104, 68, 252},
    {216, 0, 204},
    {228, 0, 88},
    {248, 56, 0},
    {228,92,16},
    {172, 124, 0},
    {0, 184, 0},
    {0, 168, 0},
    {0, 168, 68},
    {0, 136, 136},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {248, 248, 248},
    {60, 188, 252},
    {104, 136, 252},
    {152, 120, 248},
    {248, 120, 248},
    {248, 88, 152},
    {248, 120, 88},
    {252, 160, 68},
    {248, 184, 0},
    {184, 248, 24},
    {88, 216, 84},
    {88, 248, 152},
    {0, 232, 216},
    {120, 120, 120},
    {0, 0, 0},
    {0, 0, 0},
    {252, 252, 252},
    {164, 228, 252},
    {184, 184, 248},
    {216, 184, 248},
    {248, 184, 248},
    {248, 164, 192},
    {240, 208, 176},
    {252, 224, 168},
    {248, 216, 120},
    {216, 248, 120},
    {184, 248, 184},
    {184, 248, 216},
    {0, 252, 252},
    {248, 216, 248},
    {0, 0, 0},
    {0, 0, 0}
};

// Alternative NTSC palette from -merp (merp@captainn.net http://qbob.free.fr)
const int ALT_NTSC_HEX_PALETTE[] = {
   0x525252, 0x000080, 0x08008A, 0x2C007E,
   0x4A004E, 0x500006, 0x440000, 0x260800,
   0x0A2000, 0x002E00, 0x003200, 0x00260A,
   0x001C48, 0x000000, 0x000000, 0x000000,
   0xA4A4A4, 0x0038CE, 0x3416EC, 0x5E04DC,
   0x8C00B0, 0x9A004C, 0x901800, 0x703600,
   0x4C5400, 0x0E6C00, 0x007400, 0x006C2C,
   0x005E84, 0x000000, 0x000000, 0x000000,
   0xFFFFFF, 0x4C9CFF, 0x7C78FF, 0xA664FF,
   0xDA5AFF, 0xF054C0, 0xF06A56, 0xD68610,
   0xBAA400, 0x76C000, 0x46CC1A, 0x2EC866,
   0x34C2BE, 0x3A3A3A, 0x000000, 0x000000,
   0xFFFFFF, 0xB6DAFF, 0xC8CAFF, 0xDAC2FF,
   0xF0BEFF, 0xFCBCEE, 0xFAC2C0, 0xF2CCA2,
   0xE6DA92, 0xCCE68E, 0xB8EEA2, 0xAEEABE,
   0xAEE8E2, 0xB0B0B0, 0x000000, 0x000000,
};

#endif // CONSTANTS_H
