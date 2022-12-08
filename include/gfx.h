#ifndef GFX_H
#define GFX_H

#include "base.h"

typedef struct pixel_buffer {
    i32 Width;
    i32 Height;
    u32* Memory;
} pixel_buffer;

#endif
