#ifndef _EMU_PPU_H
#define _EMU_PPU_H

#include "base.h"

#include <stdlib.h>

#define NesScreenWidth (256)
#define NesScreenHeight (240)

typedef struct status_register {
    u8 VerticalBlank;
    u8 SpriteZeroHit;
    u8 SpriteOverflow;
} status_register;

typedef struct ppu {
    i32 Dot;
    i32 Scanline;
    bool32 FrameComplete;
    status_register Status;
} ppu;

typedef struct ppu_pixel {
    u32 Color;
    i32 X;
    i32 Y;
} ppu_pixel;

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

#endif
