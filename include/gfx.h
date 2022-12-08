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

#endif
