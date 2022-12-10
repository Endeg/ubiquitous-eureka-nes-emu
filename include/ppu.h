#ifndef _EMU_PPU_H
#define _EMU_PPU_H

#include "base.h"

#include <stdlib.h>

#define PpuCyclePerScanline (341)
#define PpuScanlineCount (261)

#define NesScreenWidth (256)
#define NesScreenHeight (240)

typedef struct ppu {
    u32 Cycle;
    i32 Scanline;
    bool32 FrameComplete;
} ppu;

typedef struct ppu_pixel {
    u32 Color;
    i32 X;
    i32 Y;
} ppu_pixel;

internal ppu_pixel
PpuGetCurrentPixel(ppu* Ppu) {
    ppu_pixel Result;
    Result.Color = 0xFF000000 | (rand() | ((u32)rand() << 16));
    Result.X = Ppu->Cycle - 1;
    Result.Y = Ppu->Scanline;
    return Result;
}

internal ppu
PpuInit(void) {
    ppu Result = {0};
    return Result;
}

internal void
PpuTick(ppu* Ppu) {
	Ppu->Cycle++;
	if (Ppu->Cycle >= PpuCyclePerScanline)
	{
		Ppu->Cycle = 0;
		Ppu->Scanline++;
		if (Ppu->Scanline >= PpuScanlineCount)
		{
			Ppu->Scanline = -1;
			Ppu->FrameComplete = 1;
		}
	}
}

#endif
