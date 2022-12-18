#include "base.h"

#include "bus.h"
#include "rom.h"
#include "disassembly.h"
#include "gfx.h"
#include "system_font.h"
#include "dumb_allocator.h"
#include "file_io.h"

#define APP_IMPLEMENTATION
#define APP_WINDOWS
#include "app.h"

#define CHIPS_IMPL
#include "m6502.h"

#include <stdio.h>
#include <stdarg.h>

// #define RomPath ("Super Mario Bros. (JU) [!].nes")
#define RomPath ("Donkey Kong (U) (PRG1) [!p].nes")

void
PlatformPrint(char* FormatString, ...) {
    char FormatBuffer[Kilobytes(1)];
    va_list Arguments;
    va_start(Arguments, FormatString);
    vsprintf(FormatBuffer, FormatString, Arguments);
    va_end(Arguments);
    printf("PLATFORM: %s\n", FormatBuffer);
}

#define ScreenScale (1)
#define ScreenWidth (1280 / ScreenScale)
#define ScreenHeight (720 / ScreenScale)

internal void
DrawRam(bus* Bus, pixel_buffer* Buffer, i32 CellX, i32 CellY, u8* CharBuffer) {
    u16 Address = 0x0000;
    i32 NumberOfColumns = 24;
    for (i32 Row = 0; Row < 85; Row++) {
        sprintf(CharBuffer, "%04X:\0", Address);
        PrintToPixelBuffer(Buffer,
                           CellX,
                           CellY + Row,
                           CharBuffer);

        for (i32 Column = 0; Column < NumberOfColumns; Column++) {
            u8 MemoryValue = BusRead(Bus, Address);

            sprintf(CharBuffer, "%02X\0", MemoryValue);

            PrintToPixelBuffer(Buffer,
                               CellX + (Column * 2) + 5,
                               CellY + Row,
                               CharBuffer);

            Address++;
        }
    }
}

internal void
CpuTick(m6502_t* Cpu, u64* Pins, bus* Bus) {
    *Pins = m6502_tick(Cpu, *Pins);
    u16 Address = M6502_GET_ADDR(*Pins);
    if (*Pins & M6502_RW) {
        u8 MemoryValue = BusRead(Bus, Address);
        BusPostRead(Bus, Address);
        M6502_SET_DATA(*Pins, MemoryValue);
    } else {
        u8 MemoryValueToWrite = M6502_GET_DATA(*Pins);
        BusWrite(Bus, Address, MemoryValueToWrite);
        //TODO: Memory post-write
    }
}

internal void
GlobalTick(m6502_t* Cpu, u64* Pins, bus* Bus,
           ppu* Ppu, pixel_buffer* Screen) {
    ppu_pixel Pixel = PpuGetCurrentPixel(Ppu);
    PixelBufferPutPixel(Screen, Pixel.X, Pixel.Y, Pixel.Color);

    PpuTick(Ppu);

    if (Bus->TickCount % 3 == 0) {
        CpuTick(Cpu, Pins, Bus);
    }

    Bus->TickCount++;
}

internal void
DrawCpuState(pixel_buffer* DestinationPixelBuffer,
             i32 CellX, i32 CellY,
             m6502_t* Cpu,
             bus* Bus,
             u8* CharBuffer) {
    sprintf(CharBuffer, "Tick:%010u", Bus->TickCount);
    PrintToPixelBuffer(DestinationPixelBuffer, CellX, CellY, CharBuffer);
    sprintf(CharBuffer, "PC:%04X A:%02X X:%02X Y:%02X S:%02X P:%02X",
        Cpu->PC, Cpu->A, Cpu->X, Cpu->Y, Cpu->S, Cpu->P);
    PrintToPixelBuffer(DestinationPixelBuffer, CellX, CellY + 1, CharBuffer);
}

int AppProc(app_t* App, void* UserData) {
    dumb_allocator Allocator = InitDumbAllocator(Megabytes(8));
    void* RomBuffer = DumbAllocate(&Allocator, Kilobytes(128));
    instruction_info* Instructions = DumbAllocate(&Allocator, sizeof(instruction_info) * 0x100);
    u8* CharBuffer = DumbAllocate(&Allocator, Kilobytes(1));
    u8** DisassemledInstructions = DumbAllocate(&Allocator, sizeof(u8*) * 0xFFFF);
    u8* DissasemblyStringData = DumbAllocate(&Allocator, Megabytes(3));

    InitInstructionsDictionary(Instructions);

    loaded_file RomFile = LoadFile(RomPath, RomBuffer);

    Assert(RomFile.Data[0] == 0x4E);
    Assert(RomFile.Data[1] == 0x45);
    Assert(RomFile.Data[2] == 0x53);
    Assert(RomFile.Data[3] == 0x1A);

    u8* Ram = DumbAllocate(&Allocator, Kilobytes(2));
    ppu Ppu = PpuInit();
    rom Rom = ParseRom(RomFile);
    bus Bus = {0};
    Bus.Rom = &Rom;
    Bus.Ram = Ram;
    Bus.Ppu = &Ppu;

    Dissasemble(&Bus,
                Instructions,
                DisassemledInstructions,
                DissasemblyStringData);

    m6502_t Cpu;
    m6502_desc_t CpuDesc = {0};
    uint64_t Pins = m6502_init(&Cpu, &CpuDesc);

    pixel_buffer Screen = {
        ScreenWidth,
        ScreenHeight,
        (u32*)DumbAllocate(&Allocator, sizeof(u32) * ScreenWidth * ScreenHeight),
    };

    pixel_buffer NesScreen = {
        NesScreenWidth,
        NesScreenHeight,
        (u32*)DumbAllocate(&Allocator, sizeof(u32) * NesScreenWidth * NesScreenHeight),
    };

    pixel_buffer NameTableVisual0 = {
        16,
        16,
        (u32*)Ppu.NameTable[0],
    };

    pixel_buffer NameTableVisual1 = {
        16,
        16,
        (u32*)Ppu.NameTable[1],
    };

    // app_interpolation(App, APP_INTERPOLATION_NONE);
    app_screenmode(App, APP_SCREENMODE_WINDOW);

    //TODO: Fix disassembled code rendering during CPU startup
    bool32 Animate = 1;

    f32 AppTimeFrequency = app_time_freq(App);

    while(app_yield(App) != APP_STATE_EXIT_REQUESTED) {
        u64 AppTimeFrameStart = app_time_count(App);
        bool32 DoOneTick = 0;
        bool32 DoOneInstruction = 0;
        bool32 DoOneFrame = 0;
        app_input_t Input = app_input(App);
        for (i32 InputIndex = 0; InputIndex < Input.count; InputIndex++) {
            if (Input.events[InputIndex].type == APP_INPUT_KEY_DOWN) {
                if (Input.events[InputIndex].data.key == APP_KEY_SPACE) {
                    Animate = !Animate;
                }
                if (Input.events[InputIndex].data.key == APP_KEY_S) {
                    DoOneTick = 1;
                }
                if (Input.events[InputIndex].data.key == APP_KEY_I) {
                    DoOneInstruction = 1;
                }
                if (Input.events[InputIndex].data.key == APP_KEY_F) {
                    DoOneFrame = 1;
                }
            }
        }

        if (Animate) {
            do {
                GlobalTick(&Cpu, &Pins, &Bus,
                           &Ppu, &NesScreen);
            } while (!Ppu.FrameComplete);
            Ppu.FrameComplete = 0;
        } else if (DoOneTick) {
            GlobalTick(&Cpu, &Pins, &Bus,
                       &Ppu, &NesScreen);
        } else if (DoOneInstruction) {
            u16 SavedPC = Cpu.PC;
            do {
                GlobalTick(&Cpu, &Pins, &Bus,
                           &Ppu, &NesScreen);
            } while (Cpu.PC == SavedPC);
            // TODO: Cpu.PC change doesn't mean that Cpu is on the next instruction
            //       We also need to validate that this instruction is inside
            //       DisassemledInstructions. But looks like this aproach doesn't work
            //       properly.
            while (!DisassemledInstructions[Cpu.PC]) {
                GlobalTick(&Cpu, &Pins, &Bus,
                           &Ppu, &NesScreen);
            }

        } else if (DoOneFrame) {
            do {
                GlobalTick(&Cpu, &Pins, &Bus,
                           &Ppu, &NesScreen);
            } while (!Ppu.FrameComplete);
            Ppu.FrameComplete = 0;
        }

        PixelBufferClear(&Screen, 0xFF000000);

        DrawCpuState(&Screen, 1, 1, &Cpu, &Bus, CharBuffer);
        DrawCode(&Screen, 1, 4, Cpu.PC, &Bus, DisassemledInstructions);
        DrawRam(&Bus, &Screen, 1, 12, CharBuffer);

        {
            sprintf(CharBuffer, "S: %04d, D: %03d, CTRL: %02X, STATUS: %02X, OAMADDR: %04X (%04X)",
                Ppu.Scanline, Ppu.Dot,
                Ppu.Control,
                PpuPackStatus(&Ppu),
                Ppu.Oam.Address,
                Ppu.Oam.TempAddress);
            PrintToPixelBuffer(&Screen, 1, 3, CharBuffer);
        }

        // u8 Control;
        // status_register Status;
        // oam Oam;

        PixelBufferBlit(&Screen, &NesScreen, 8 * 54, 8 * 1);

        PixelBufferBlit(&Screen, &NameTableVisual0, 8 * 80, 8 * 1);
        PixelBufferBlit(&Screen, &NameTableVisual1, 8 * 80, 8 * 4);

        {
            i32 PatternTablesX = 8 * 54;
            i32 PatternTablesY = 8 * 40;

            //TODO: Better name for PatternsPerColum
            for (i32 Row = 0; Row < PatternsPerColum; Row++) {
                for (i32 Column = 0; Column < PatternsPerColum; Column++) {
                    for (i32 PixelOffsetY = 0; PixelOffsetY < PatternSizeInPixels; PixelOffsetY++) {
                        for (i32 PixelOffsetX = 0; PixelOffsetX < PatternSizeInPixels; PixelOffsetX++) {
                            {
                                i32 DestX = PatternTablesX + (Column * PatternSizeInPixels) + PixelOffsetX;
                                i32 DestY = PatternTablesY + (Row * PatternSizeInPixels) + PixelOffsetY;
                                u32 DestColor = PpuGetTilePixel(&Bus, Left, Row, Column, PixelOffsetX, PixelOffsetY);
                                PixelBufferPutPixel(&Screen, DestX, DestY, DestColor);
                            }
                            {
                                i32 DestX = PatternTablesX + (Column * PatternSizeInPixels) + PixelOffsetX + 128;
                                i32 DestY = PatternTablesY + (Row * PatternSizeInPixels) + PixelOffsetY;
                                u32 DestColor = PpuGetTilePixel(&Bus, Right, Row, Column, PixelOffsetX, PixelOffsetY);
                                PixelBufferPutPixel(&Screen, DestX, DestY, DestColor);
                            }
                        }
                    }
                }
            }

        }

        app_present(App, Screen.Memory, ScreenWidth, ScreenHeight, 0xFFFFFF, 0x220000);
        u64 AppTimeFrameEnd = app_time_count(App);
        f32 FrameDelta = (f32)(AppTimeFrameEnd - AppTimeFrameStart) / AppTimeFrequency;
        //DumpFloatExpression(FrameDelta);
    }
    return 0;
}

int main(int argc, char** argv) {
    Unused(argc);
    Unused(argv);

    return app_run(AppProc, NULL, NULL, NULL, NULL);
}
