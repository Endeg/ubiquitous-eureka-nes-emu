#ifndef _EMU_TYPES_H
#define _EMU_TYPES_H

#include "base.h"

typedef enum mirroring {
    Horizontal,
    Vertical,
} mirroring;

typedef struct rom {
    u8 PrgRomBankCount;
    u8 ChrRomBankCount;
    u32 MapperId;
    mirroring Mirroring;
    bool32 IgnoreMirroring;
    bool32 HasPrgRam;
    bool32 HasTrainer;
    u8* Prg;
    u8* Chr;
} rom;

typedef struct status_register {
    u8 VerticalBlank;
    u8 SpriteZeroHit;
    u8 SpriteOverflow;
} status_register;

typedef struct ppu {
    i32 Dot;
    i32 Scanline;
    bool32 FrameComplete;
    u8 Control;
    status_register Status;
} ppu;

typedef struct ppu_pixel {
    u32 Color;
    i32 X;
    i32 Y;
} ppu_pixel;

typedef enum pattern_table_half {
    Left,
    Right,
} pattern_table_half;

typedef struct bus {
    u32 TickCount;
    rom* Rom;
    u8* Ram;
    ppu* Ppu;
} bus;

#endif
