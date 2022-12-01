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

int main(int argc, char** argv) {
    Unused(argc);
    Unused(argv);

    m6502_t Cpu;
    m6502_desc_t CpuDesc = {0};
    uint64_t Pins = m6502_init(&Cpu, &CpuDesc);
    u8 MemoryValue = 0xEA;
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
