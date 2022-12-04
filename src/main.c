#include "base.h"

// #define APP_IMPLEMENTATION
// #include "app.h"

#define CHIPS_IMPL
#include "m6502.h"

#include <stdio.h>

typedef struct rom {
    u8* Prg;
} rom;

typedef struct ppu {
    u32 cycle;
    i32 scanline;
} ppu;

typedef struct bus {
    rom* Rom;
    u8* Ram;
    ppu* Ppu;
} bus;

typedef struct loaded_file {
    u8* Data;
    size_t Size;
} loaded_file;

internal loaded_file
LoadFile(char* FileName, void* DestinationMemory) {
    FILE* File = fopen(FileName, "rb");

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

internal u8
MemoryRead(bus* Bus, u16 Address) {
    return 0xEA;
}

internal void
MemoryWrite(bus* Bus, u16 Address, u8 Value) {
    Halt("TODO: Need writing!");
}

void
PlatformPrint(char* FormatString, ...) {
    char FormatBuffer[Kilobytes(1)];
    va_list Arguments;
    va_start(Arguments, FormatString);
    vsprintf(FormatBuffer, FormatString, Arguments);
    va_end(Arguments);
    printf("PLATFORM: %s\n", FormatBuffer);
}

global_variable u8 TempMemory[Megabytes(1)];

#define ROM_PATH ("Super Mario Bros. (JU) [!].nes")

int main(int argc, char** argv) {
    Unused(argc);
    Unused(argv);

    loaded_file RomFile = LoadFile(ROM_PATH, TempMemory);

    Assert(RomFile.Size == 40976);
    Assert(RomFile.Data[0] == 0x4E);
    Assert(RomFile.Data[1] == 0x45);
    Assert(RomFile.Data[2] == 0x53);
    Assert(RomFile.Data[3] == 0x1A);

    m6502_t Cpu;
    m6502_desc_t CpuDesc = {0};
    uint64_t Pins = m6502_init(&Cpu, &CpuDesc);
    for (i32 TickIndex = 0; TickIndex < 20; TickIndex++) {
        Pins = m6502_tick(&Cpu, Pins);
        u16 Address = M6502_GET_ADDR(Pins);
        if (Pins & M6502_RW) {
            bus Bus = {0};
            u8 MemoryValue = MemoryRead(&Bus, Address);
            M6502_SET_DATA(Pins, MemoryValue);
        } else {
            bus Bus = {0};
            u8 MemoryValueToWrite = M6502_GET_DATA(Pins);
            MemoryWrite(&Bus, Address, MemoryValueToWrite);
        }

        // DumpIntExpression(TickIndex);
        DumpU16HexExpression(Cpu.PC);
        // DumpU8HexExpression(Cpu.A);
        // DumpU8HexExpression(Cpu.X);
        // DumpU8HexExpression(Cpu.Y);
    }


    return 0;
}
