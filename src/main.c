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

#define RomPath ("Super Mario Bros. (JU) [!].nes")

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
    i32 NumberOfColumns = 16;
    for (i32 Row = 0; Row < 32; Row++) {
        sprintf(CharBuffer, "%04X:\0", Address);
        PrintToPixelBuffer(Buffer,
                           CellX,
                           CellY + Row,
                           CharBuffer);

        for (i32 Column = 0; Column < NumberOfColumns; Column++) {
            u8 MemoryValue = MemoryRead(Bus, Address);

            sprintf(CharBuffer, "%02X\0", MemoryValue);

            PrintToPixelBuffer(Buffer,
                               CellX + (Column * 3) + 5,
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
        u8 MemoryValue = MemoryRead(Bus, Address);
        M6502_SET_DATA(*Pins, MemoryValue);
    } else {
        u8 MemoryValueToWrite = M6502_GET_DATA(*Pins);
        MemoryWrite(Bus, Address, MemoryValueToWrite);
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

    rom Rom = ParseRom(RomFile);
    bus Bus = {0};
    Bus.Rom = &Rom;
    Bus.Ram = Ram;

    ppu Ppu = PpuInit();

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

    //app_interpolation(App, APP_INTERPOLATION_NONE);
    app_screenmode(App, APP_SCREENMODE_WINDOW);

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

        PixelBufferBlit(&Screen, &NesScreen, 8 * 54, 8 * 1);

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
