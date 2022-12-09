#ifndef GFX_H
#define GFX_H

#include "base.h"

typedef struct pixel_buffer {
    i32 Width;
    i32 Height;
    u32* Memory;
} pixel_buffer;

internal void
PixelBufferClear(pixel_buffer* Dest, u32 Color) {
    u32* Row = Dest->Memory;
    for (i32 Y = 0; Y < Dest->Height; Y++) {
        u32* Pixel = Row;
        for (i32 X = 0; X < Dest->Width; X++) {
            *Pixel = Color;
            Pixel++;
        }
        Row += Dest->Width;
    }
}

internal void
PixelBufferBlit(pixel_buffer* Dest, pixel_buffer* Src, i32 X, i32 Y) {
    u32* DestRow = Dest->Memory;
    DestRow += Y * Dest->Width;
    DestRow += X;

    u32* SrcRow = Src->Memory;
    for (i32 SrcY = 0; SrcY < Src->Height; SrcY++) {
        u32* DestPixel = DestRow;
        u32* SrcPixel = SrcRow;
        for (i32 SrcX = 0; SrcX < Src->Width; SrcX++) {
            *DestPixel = *SrcPixel;
            DestPixel++;
            SrcPixel++;
        }
        DestRow += Dest->Width;
        SrcRow += Src->Width;
    }
}

#endif
