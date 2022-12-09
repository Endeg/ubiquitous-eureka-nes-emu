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
PixelBufferPutPixel(pixel_buffer* Screen, i32 X, i32 Y, u32 Color) {
    if (X < 0 || Y < 0 || X > Screen->Width || Y > Screen->Height) {
        return;
    }

    Screen->Memory[X + (Y * Screen->Width)] = Color;
}

internal void
PixelBufferBlit(pixel_buffer* Dest, pixel_buffer* Src, i32 X, i32 Y) {
    //TODO: Clip Src to draw part of Pixel buffer

    i32 SrcLeft = 0;
    i32 SrcTop = 0;
    i32 SrcRight = SrcLeft + Src->Width;
    i32 SrcBottom = SrcTop + Src->Height;

    if (X < 0) {
        //SrcLeft -= X;
        return;
    }

    if (X > (Dest->Width - Src->Width)) {
        return;
    }

    if (Y > (Dest->Height - Src->Height)) {
        return;
    }

    if (Y < 0) {
        //SrcTop -= Y;
        return;
    }

    i32 SrcPitch = SrcRight - SrcLeft;

    u32* DestRow = Dest->Memory;
    DestRow += Y * Dest->Width;
    DestRow += X;

    u32* SrcRow = Src->Memory;
    SrcRow += SrcTop * Src->Width;
    SrcRow += SrcLeft;

    for (i32 SrcY = SrcTop; SrcY < SrcBottom; SrcY++) {
        u32* DestPixel = DestRow;
        u32* SrcPixel = SrcRow;
        for (i32 SrcX = SrcLeft; SrcX < SrcRight; SrcX++) {
            *DestPixel = *SrcPixel;
            DestPixel++;
            SrcPixel++;
        }
        DestRow += Dest->Width;
        SrcRow += SrcPitch;
    }
}

#endif
