#include "base.h"

// #define APP_IMPLEMENTATION
// #include "app.h"

#define CHIPS_IMPL
#include "m6502.h"

#include <stdio.h>
#include <stdarg.h>

typedef enum mirroring {
    Horizontal,
    Vertical,
} mirroring;

typedef struct rom {
    u8 PrgRomBankCount;
    u32 MapperId;
    mirroring Mirroring;
    bool32 IgnoreMirroring;
    bool32 HasPrgRam;
    bool32 HasTrainer;
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

#include "bus.h"
#include "disassembly.h"

global_variable instruction_info Instructions[0x100] = {0};
global_variable u8 TempMemory[Megabytes(1)];
global_variable u8 StringData[Megabytes(10)];
global_variable u8* DisassemblyDict[0xFFFF];

#define RomPath ("Super Mario Bros. (JU) [!].nes")

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

#define INesHeaderSize (16)
#define INesTrainerSize (512)
#define INesPrgBanksCount (4)
#define INesFlags6 (6)
#define INesFlags7 (7)

#define INesFlags6Mirroring       (0b00000001)
#define INesFlags6PrgRam          (0b00000010)
#define INesFlags6Trainer         (0b00000100)
#define INesFlags6IgnoreMirroring (0b00001000)
#define INesFlags6MapperIdLow     (0b11110000)

#define INesFlags7MapperIdHigh    (0b11110000)

internal rom ParseRom(loaded_file LoadedFile) {
    rom Result = {0};

    Result.PrgRomBankCount = LoadedFile.Data[INesPrgBanksCount];
    Result.MapperId = (LoadedFile.Data[INesFlags7] & INesFlags7MapperIdHigh) | ((LoadedFile.Data[INesFlags6] & INesFlags6MapperIdLow) >> 4);
    Result.Mirroring = (LoadedFile.Data[INesFlags6] & INesFlags6Mirroring) ? Vertical : Horizontal;
    Result.IgnoreMirroring = LoadedFile.Data[INesFlags6] & INesFlags6IgnoreMirroring;
    Result.HasPrgRam = LoadedFile.Data[INesFlags6] & INesFlags6PrgRam;
    Result.HasTrainer = LoadedFile.Data[INesFlags6] & INesFlags6Trainer;

    u8* PrgSectionStart = LoadedFile.Data +
                          INesHeaderSize +
                          ((Result.HasTrainer) ? INesTrainerSize : 0);

    Result.Prg = PrgSectionStart;

    Assert(!Result.HasPrgRam);
    Assert(Result.MapperId == MapperNROM);    

    return Result;
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

int main(int argc, char** argv) {
    Unused(argc);
    Unused(argv);

    InitInstructionsDictionary(Instructions);

    loaded_file RomFile = LoadFile(RomPath, TempMemory);

    Assert(RomFile.Data[0] == 0x4E);
    Assert(RomFile.Data[1] == 0x45);
    Assert(RomFile.Data[2] == 0x53);
    Assert(RomFile.Data[3] == 0x1A);

    u8* Ram[RamSize] = {0};

    rom Rom = ParseRom(RomFile);
    bus Bus = {0};
    Bus.Rom = &Rom;
    Bus.Ram = (u8*)Ram;

    //Dissasemble(&Bus, Instructions, DisassemblyDict, StringData);

    m6502_t Cpu;
    m6502_desc_t CpuDesc = {0};
    uint64_t Pins = m6502_init(&Cpu, &CpuDesc);
    for (i32 TickIndex = 0; TickIndex < 2000; TickIndex++) {
        
        PrintDisassembledInstruction(
            Cpu.PC, MemoryRead(&Bus, Cpu.PC), &Bus, Instructions);

        Pins = m6502_tick(&Cpu, Pins);
        u16 Address = M6502_GET_ADDR(Pins);
        if (Pins & M6502_RW) {
            u8 MemoryValue = MemoryRead(&Bus, Address);
            M6502_SET_DATA(Pins, MemoryValue);
        } else {
            u8 MemoryValueToWrite = M6502_GET_DATA(Pins);
            MemoryWrite(&Bus, Address, MemoryValueToWrite);
        }
    }

    return 0;
}
