#ifndef _EMU_PPU_H
#define _EMU_PPU_H

#include "base.h"
#include "emu_types.h"
#include "bus.h"

#include <stdlib.h>

#define NesScreenWidth (256)
#define NesScreenHeight (240)

// NOTE: PPUCTRL Bits
#define NmiEnableMask            (0b10000000)
#define MasterSlaveMask          (0b01000000)
#define SpriteHeightMask         (0b00100000)
#define BackgroundTileSelectMask (0b00010000)
#define SpriteTileSelectMask     (0b00001000)
#define IncrementModeMask        (0b00000100)
#define NametableSelectMask      (0b00000011)

// NOTE: PPUSTATUS bits
#define VBlankOffset         (7)
// #define VBlank               (0b10000000)
#define SpriteZeroHitOffset  (6)
// #define SpriteZeroHit        (0b01000000)
// #define SpriteOverflow       (0b00100000)
#define SpriteOverflowOffest (5)

internal u8
PpuPackStatus(ppu* Ppu) {
    u8 Result = (Ppu->Status.VerticalBlank << VBlankOffset)
              | (Ppu->Status.SpriteZeroHit << SpriteZeroHitOffset)
              | (Ppu->Status.SpriteOverflow << SpriteOverflowOffest);
    return Result;
}

internal ppu_pixel
PpuGetCurrentPixel(ppu* Ppu) {
    ppu_pixel Result;
    Result.Color = 0xFF000000 | (rand() | ((u32)rand() << 16));
    Result.X = Ppu->Dot - 1;
    Result.Y = Ppu->Scanline;
    return Result;
}

internal ppu
PpuInit(void) {
    ppu Result = {0};
    return Result;
}

#define PpuDotPerScanline      (341)
#define PpuScanlineCount       (261)
#define PpuVblankStartScanline (241)

internal void
PpuTick(ppu* Ppu) {
    if (Ppu->Scanline == -1 && Ppu->Dot == 0) {
        Ppu->Status.VerticalBlank = 0;
    }

    if (Ppu->Scanline == PpuVblankStartScanline && Ppu->Dot == 0) {
        Ppu->Status.VerticalBlank = 1;
    }

	Ppu->Dot++;
	if (Ppu->Dot >= PpuDotPerScanline)
	{
		Ppu->Dot = 0;
		Ppu->Scanline++;
		if (Ppu->Scanline >= PpuScanlineCount)
		{
			Ppu->Scanline = -1;
			Ppu->FrameComplete = 1;
		}
	}
}

internal u8
PpuRead(bus* Bus, u16 Address) {
    if (Address >= 0x0000 && Address <= 0x1FFF) {
        Assert(Bus->Rom->MapperId == MapperNROM);
        //TODO: Mapper should work here! For now: NROM only
        return Bus->Rom->Chr[Address];
    }
    Halt("Let's not read here, yet");
    return 0x00;
}

internal void
PpuWrite(bus* Bus, u16 Address, u8 Value) {
    DumpU16HexExpression(Address);
    DumpU8HexExpression(Value);
    Halt("No writing to PPU, yet");
}

#define PatternSizeInBytes       (16)
#define PatternPlaneSizeInBytes  (8)
#define PatternSizeInPixels      (8)
#define PatternsPerColum         (16)
#define LeftPatternTableAddress  (0x0000)
#define RightPatternTableAddress (0x1000)

internal u32
PpuGetTilePixel(bus* Bus,
                pattern_table_half Half,
                i32 Row, i32 Column,
                i32 OffsetX, i32 OffsetY) {

    u16 PatternAddress;
    if (Half == Left) {
        PatternAddress = LeftPatternTableAddress;
    } else {
        PatternAddress = RightPatternTableAddress;
    }

    PatternAddress += (PatternsPerColum * Row * PatternSizeInBytes) + (Column * PatternSizeInBytes);

    u8 PatternPixelBitMask = 0b10000000 >> OffsetX;

    u8 PatternRows[2] = {
        PpuRead(Bus, PatternAddress + OffsetY),
        PpuRead(Bus, PatternAddress + OffsetY + PatternPlaneSizeInBytes),
    };

    PatternRows[0] = PatternRows[0] & PatternPixelBitMask;
    PatternRows[1] = PatternRows[1] & PatternPixelBitMask;

    PatternRows[0] = (PatternRows[0]) ? 0b00000001 : 0b00000000;
    PatternRows[1] = (PatternRows[1]) ? 0b00000010 : 0b00000000;

    u8 PalleteIndex = PatternRows[0] | PatternRows[1];
    Assert(PalleteIndex >= 0 && PalleteIndex <= 3);

    u32 PoorMansPallete[4] = {
        0xFFCC0000,
        0xFF33CC33,
        0xFF7777CC,
        0xFFEEEEEE,
    };

    return PoorMansPallete[PalleteIndex];
}

internal void
PpuRegisterWriteOamAddress(bus* Bus, u8 Value) {
    if (Bus->Ppu->Oam.AddressLach) {
        Bus->Ppu->Oam.TempAddress = (Bus->Ppu->Oam.Address & 0xFF00) | (Value << 0);
        Bus->Ppu->Oam.AddressLach = 0;
    } else {
        Bus->Ppu->Oam.TempAddress = (Bus->Ppu->Oam.Address & 0x00FF) | (Value << 8);
        Bus->Ppu->Oam.Address = Bus->Ppu->Oam.TempAddress;
        Bus->Ppu->Oam.AddressLach = 1;
    }
}

internal void
PpuRegisterWriteOamData(bus* Bus, u8 Value) {
    PpuWrite(Bus, Bus->Ppu->Oam.Address, Value);
}

#endif
