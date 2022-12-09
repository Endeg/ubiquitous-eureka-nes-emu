#include "base.h"

#define APP_IMPLEMENTATION
#define APP_WINDOWS
#include "app.h"

#define CHIPS_IMPL
#include "m6502.h"

#include <stdio.h>
#include <stdarg.h>

typedef struct loaded_file {
    u8* Data;
    size_t Size;
} loaded_file;

#include "bus.h"
#include "disassembly.h"
#include "gfx.h"
#include "system_font.h"
#include "dumb_allocator.h"

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

#define ScreenScale (1)
#define ScreenWidth (1280 / ScreenScale)
#define ScreenHeight (720 / ScreenScale)

#define NesScreenWidth (256)
#define NesScreenHeight (240)

// global_variable u8 StringData[Megabytes(10)];
// global_variable u8* DisassemblyDict[0xFFFF];

internal void
DrawRam(bus* Bus, pixel_buffer* Buffer, i32 CellX, i32 CellY, u8* CharBuffer) {
    u16 Address = 0x0000;
    i32 NumberOfColumns = 16;
    for (i32 Row = 0; Row < 16; Row++) {
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

#define PpuCyclePerScanline (341)
#define PpuScanlineCount (261)

typedef struct ppu_pixel {
    u32 Color;
    i32 X;
    i32 Y;
} ppu_pixel;

internal ppu_pixel
PpuGetCurrentPixel(ppu* Ppu) {
    ppu_pixel Result;
    Result.Color = 0xFF000000 | (rand() | ((u32)rand() << 16));
    Result.X = Ppu->Cycle - 1;
    Result.Y = Ppu->Scanline;
    return Result;
}

internal void
PpuTick(ppu* Ppu) {
	Ppu->Cycle++;
	if (Ppu->Cycle >= PpuCyclePerScanline)
	{
		Ppu->Cycle = 0;
		Ppu->Scanline++;
		if (Ppu->Scanline >= PpuScanlineCount)
		{
			Ppu->Scanline = -1;
			Ppu->FrameComplete = 1;
		}
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

int AppProc(app_t* App, void* UserData) {
    dumb_allocator Allocator = InitDumbAllocator(Megabytes(8));
    void* RomBuffer = DumbAllocate(&Allocator, Kilobytes(128));
    instruction_info* Instructions = DumbAllocate(&Allocator, sizeof(instruction_info) * 0x100);
    u8* CharBuffer = DumbAllocate(&Allocator, Kilobytes(1));
        
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

    ppu Ppu = {0};

    //Dissasemble(&Bus, Instructions, DisassemblyDict, StringData);

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

    while(app_yield(App) != APP_STATE_EXIT_REQUESTED) {
        bool32 DoOneTick = 0;
        app_input_t Input = app_input(App);
        for (i32 InputIndex = 0; InputIndex < Input.count; InputIndex++) {
            if (Input.events[InputIndex].type == APP_INPUT_KEY_DOWN) {
                if (Input.events[InputIndex].data.key == APP_KEY_SPACE) {
                    Animate = !Animate;
                }
                if (Input.events[InputIndex].data.key == APP_KEY_S) {
                    DoOneTick = 1;
                    PixelBufferClear(&NesScreen, 0xFF000000);
                }
            }
        }

        if (Animate) {
            do {
                GlobalTick(
                    &Cpu, &Pins, &Bus,
                    &Ppu, &NesScreen);
            } while (!Ppu.FrameComplete);
            Ppu.FrameComplete = 0;
        } else if (DoOneTick) {
            GlobalTick(
                &Cpu, &Pins, &Bus,
                &Ppu, &NesScreen);
        }

        PixelBufferClear(&Screen, 0xFF000000);
        //PrintToPixelBuffer(&Screen, 1, 2, "The quick brown fox jumps over the lazy dog");
        FormatDisassembledInstruction(Cpu.PC,
                                      MemoryRead(&Bus, Cpu.PC),
                                      &Bus,
                                      Instructions,
                                      CharBuffer);
        PrintToPixelBuffer(&Screen, 1, 2, CharBuffer);
        DrawRam(&Bus, &Screen, 1, 10, CharBuffer);

        PixelBufferBlit(&Screen, &NesScreen, 8 * 54, 8 * 1);


        app_present(App, Screen.Memory, ScreenWidth, ScreenHeight, 0xFFFFFF, 0xCC0000);
    }
    return 0;
}

int main(int argc, char** argv) {
    Unused(argc);
    Unused(argv);

    return app_run(AppProc, NULL, NULL, NULL, NULL);
}
