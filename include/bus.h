#ifndef _EMU_BUS_H
#define _EMU_BUS_H

#include "base.h"
#include "constants.h"
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

internal u8
BusRead(bus* Bus, u16 Address) {
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
            MemoryAccessTrap(Address, 0x00, "Not sure, need debugging.");
        }
    } else if (Address >= PpuRegisterAddressStart && Address <= PpuRegisterAddressEnd) {
        return PpuRegisterRead(Bus, Address);
    }

    return 0x00;
}

internal void
BusWrite(bus* Bus, u16 Address, u8 Value) {
    if (Address >= PpuRegisterAddressStart && Address <= PpuRegisterAddressEnd) {
        PpuRegisterWrite(Bus, Address, Value);
    } else if (Address >= 0x0000 && Address <= 0X1FFF) {
        // RAM
        u16 RamBaseAddress = Address % RamSize;
        Bus->Ram[RamBaseAddress] = Value;
    } else if (Address >= 0x4000 && Address <= 0X4017) {
        // APU I/O
    } else {
        MemoryAccessTrap(Address, Value, "Unexpected writing");
    }
}

internal void
BusPostRead(bus* Bus, u16 Address) {
    if (Address >= PpuRegisterAddressStart && Address <= PpuRegisterAddressEnd) {
        u16 PpuRegister = (Address - PpuRegisterAddressStart) % PpuRegisterCount;
        if (PpuRegister == PPUSTATUS) {
            Bus->Ppu->Status.VerticalBlank = 0;
            Bus->Ppu->AddressLatch = 0;
        } else if (PpuRegister == PPUMASK) {
        } else if (PpuRegister == PPUSTATUS) {
        } else if (PpuRegister == OAMADDR) {
        } else if (PpuRegister == OAMDATA) {
        }
    }
}

#endif
