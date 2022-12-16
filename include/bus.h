#ifndef _EMU_BUS_H
#define _EMU_BUS_H

#include "rom.h"
#include "ppu.h"

typedef struct bus {
    u32 TickCount;
    rom* Rom;
    u8* Ram;
    ppu* Ppu;
} bus;

// NOTE: PPUCTRL bits
#define NmiEnable            (0b10000000)
#define PpuMasterSlave       (0b01000000)
#define SpriteHeight         (0b00100000)
#define BackgroundTileSelect (0b00010000)
#define SpriteTileSelect     (0b00001000)
#define IncrementMode        (0b00000100)
#define NametableSelect      (0b00000011)

#define PPUCTRL   (0x2000)
#define PPUMASK   (0x2001)
#define PPUSTATUS (0x2002)
#define OAMADDR   (0x2003)
#define OAMDATA   (0x2004)
#define PPUSCROLL (0x2005)
#define PPUADDR   (0x2006)
#define PPUDATA   (0x2007)
#define OAMDMA    (0x4014)

internal u8
MemoryRead(bus* Bus, u16 Address) {
    if (Address >= 0x0000 && Address <= 0x07FF) {
        //TODO: Mirror RAM
        return Bus->Ram[Address];
    } else if (Address >= 0x8000 && Address <= 0xFFFF) {
        //Mapper space
        Assert(Bus->Rom->MapperId == MapperNROM);

        u16 BaseAddress = Address - 0x8000;

        if (Bus->Rom->PrgRomBankCount == 2) {
            return Bus->Rom->Prg[BaseAddress];
        } else if (Bus->Rom->PrgRomBankCount == 1) {
            if (BaseAddress > PrgBankSize) {
                BaseAddress -= PrgBankSize;
                return Bus->Rom->Prg[BaseAddress];
            } else {
                return Bus->Rom->Prg[BaseAddress];
            }
        } else {
            Halt("Not sure, need debugging.");
        }
    } else if (Address == PPUSTATUS) {
        //TODO: Mirror PPU registers
        return PpuPackStatus(Bus->Ppu);
    }

    return 0x00;
}

internal void
MemoryWrite(bus* Bus, u16 Address, u8 Value) {
    if ((Address >= 0x2000 && Address <= 0X2007) ||
        (Address >= 0x2008 && Address <= 0X3FFF)) {
        // PPU Registers
        DumpU16HexExpression(Address);
        DumpU8HexExpression(Value);
        //TODO: need to store properly
    } else if (Address >= 0x0000 && Address <= 0X1FFF) {
        // RAM
        // DumpU16HexExpression(Address);
        // DumpU8HexExpression(Value);
        u16 RamBaseAddress = Address % RamSize;
        Bus->Ram[RamBaseAddress] = Value;
    } else if (Address >= 0x4000 && Address <= 0X4017) {
        // APU I/O
    } else {
        DumpU16HexExpression(Address);
        DumpU8HexExpression(Value);
        Halt("TODO: Need writing!");
    }
}

internal void
BusPostRead(bus* Bus, u16 Address) {
    if (Address == PPUSTATUS) {
        //TODO: Mirror PPU registers
        Bus->Ppu->Status.VerticalBlank = 0;
    }
}

#endif
