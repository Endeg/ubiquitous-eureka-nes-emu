#ifndef _EMU_FILE_IO_H
#define _EMU_FILE_IO_H

#include "base.h"

#include <stdio.h>

typedef struct loaded_file {
    u8* Data;
    size_t Size;
} loaded_file;

internal loaded_file
LoadFile(char* FileName, void* DestinationMemory) {
    FILE* File = fopen(FileName, "rb");
    Assert(File);

    i32 CurrentByte;

    loaded_file Result;

    Result.Data = (u8*)DestinationMemory;
    Result.Size = 0;

    while ((CurrentByte = fgetc(File)) != EOF) {
        Result.Data[Result.Size++] = (u8)CurrentByte;
    }

    fclose(File);

    return Result;
}

#endif
