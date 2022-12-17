#ifndef _EMU_BUS_H
#define _EMU_BUS_H

#include "emu_types.h"
#include "rom.h"
#include "ppu.h"

// NOTE: PPUCTRL bits
#define NmiEnable            (0b10000000)
#define PpuMasterSlave       (0b01000000)
#define SpriteHeight         (0b00100000)
#define BackgroundTileSelect (0b00010000)
#define SpriteTileSelect     (0b00001000)
#define IncrementMode        (0b00000100)
#define NametableSelect      (0b00000011)

#define PpuRegisterAddressStart (0x2000)
#define PpuRegisterAddressEnd   (0x3FFF)
#define PpuRegisterCount        (0x0008)


#define PPUCTRL   (0x0000)
#define PPUMASK   (0x0001)
#define PPUSTATUS (0x0002)
#define OAMADDR   (0x0003)
#define OAMDATA   (0x0004)
#define PPUSCROLL (0x0005)
#define PPUADDR   (0x0006)
#define PPUDATA   (0x0007)
#define OAMDMA    (0x4014)

internal u8
MemoryRead(bus* Bus, u16 Address) {
    if (Address >= 0x0000 && Address <= 0x07FF) {
        //TODO: Mirror RAM
        return Bus->Ram[Address];
    } else if (Address >= 0x8000 && Address <= 0xFFFF) {
        //Mapper space
        Assert(Bus->Rom->MapperId == MapperNROM);
        //TODO: Mapper should work here! For now: NROM only

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
    } else if (Address >= PpuRegisterAddressStart && Address <= PpuRegisterAddressEnd) {
        u16 PpuRegister = (Address - PpuRegisterAddressStart) % PpuRegisterCount;
        if (PpuRegister == PPUCTRL) {
            return Bus->Ppu->Control;
        } else if (PpuRegister == PPUSTATUS) {
            return PpuPackStatus(Bus->Ppu);
        } else {
            // DumpU16HexExpression(Address);
            // Halt("No reading from here!");
        }
    }

    return 0x00;
}

internal void
MemoryWrite(bus* Bus, u16 Address, u8 Value) {
    if (Address >= PpuRegisterAddressStart && Address <= PpuRegisterAddressEnd) {
        u16 PpuRegister = (Address - PpuRegisterAddressStart) % PpuRegisterCount;
        // PPU Registers
        if (PpuRegister == PPUCTRL) {
            Bus->Ppu->Control = Value;
        } else if (PpuRegister == PPUSTATUS) {
            // TODO: Check if writing to PPUSTATUS is ever legit
        } else {
            // DumpU16HexExpression(Address);
            // Halt("No writing here!");
        }
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
    if (Address >= PpuRegisterAddressStart && Address <= PpuRegisterAddressEnd) {
        u16 PpuRegister = (Address - PpuRegisterAddressStart) % PpuRegisterCount;
        if (PpuRegister == PPUSTATUS) {
            Bus->Ppu->Status.VerticalBlank = 0;
        }
    }
}

#endif
