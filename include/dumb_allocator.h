#ifndef _COMMON_DUMB_ALLOCATOR_H
#define _COMMON_DUMB_ALLOCATOR_H

#include "base.h"
#include <stdlib.h>

typedef struct dumb_allocator {
    size_t Size;
    size_t FreeSpace;
    u8* MemoryBase;
    u8* MemoryCurrent;
} dumb_allocator;

internal dumb_allocator
InitDumbAllocator(size_t Size) {
    dumb_allocator Result;
    Result.Size = Size;
    Result.FreeSpace = Result.Size;
    Result.MemoryBase = (u8*)malloc(Result.Size);
    Result.MemoryCurrent = Result.MemoryBase;
    return Result;
}

internal void*
DumbAllocate(dumb_allocator* Allocator, size_t Size) {
    if (Allocator->FreeSpace < Size) {
        Halt("No space left!");
    }

    void* Result = Allocator->MemoryCurrent;
    Allocator->MemoryCurrent += Size;
    Allocator->FreeSpace -= Size;
    DumpIntExpression(Allocator->FreeSpace);
    return Result;
}

#endif
