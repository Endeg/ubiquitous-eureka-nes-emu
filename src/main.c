#include "base.h"

// #define APP_IMPLEMENTATION
// #include "app.h"

#define CHIPS_IMPL
#include "m6502.h"

#include <stdio.h>

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

typedef enum addressing_mode {
    Implicit,
    Immediate,
    Accumulator,
    Relative,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    IndirectX,
    IndirectY,
} addressing_mode;

typedef enum instruction_mnemonic {
  ADC, ANC, AND, ANE, ARR, ASL, ASR, BCC,
  BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC,
  BVS, CLC, CLD, CLI, CLV, CMP, CPX, CPY,
  DCP, DEC, DEX, DEY, EOR, INC, INX, INY,
  ISB, JMP, JSR, LAS, LAX, LDA, LDX, LDY,
  LSR, LXA, NOP, ORA, PHA, PHP, PLA, PLP,
  RLA, ROL, ROR, RRA, RTI, RTS, SAX, SBC,
  SBX, SEC, SED, SEI, SHA, SHS, SHX, SHY,
  SLO, SRE, STA, STX, STY, TAX, TAY, TSX,
  TXA, TXS, TYA, UNS,
} instruction_mnemonic;

typedef struct instruction_info {
    instruction_mnemonic Mnemonic;
    addressing_mode AddressingMode;
    bool32 Unofficial;
} instruction_info;

global_variable instruction_info Instructions[0xFF];

// off- ++++++++++ Positive ++++++++++  ---------- Negative ----------
// set  00      20      40      60      80      a0      c0      e0      mode

// +00  BRK     JSR     RTI     RTS     NOP*    LDY     CPY     CPX     Impl/immed
// +01  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     (indir,x)
// +02   t       t       t       t      NOP*t   LDX     NOP*t   NOP*t     ? /immed
// +03  SLO*    RLA*    SRE*    RRA*    SAX*    LAX*    DCP*    ISB*    (indir,x)
// +04  NOP*    BIT     NOP*    NOP*    STY     LDY     CPY     CPX     Zeropage
// +05  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     Zeropage
// +06  ASL     ROL     LSR     ROR     STX     LDX     DEC     INC     Zeropage
// +07  SLO*    RLA*    SRE*    RRA*    SAX*    LAX*    DCP*    ISB*    Zeropage

// +08  PHP     PLP     PHA     PLA     DEY     TAY     INY     INX     Implicit
// +09  ORA     AND     EOR     ADC     NOP*    LDA     CMP     SBC     Immediate
// +0a  ASL     ROL     LSR     ROR     TXA     TAX     DEX     NOP     Accu/impl
// +0b  ANC**   ANC**   ASR**   ARR**   ANE**   LXA**   SBX**   SBC*    Immediate
// +0c  NOP*    BIT     JMP     JMP ()  STY     LDY     CPY     CPX     Absolute
// +0d  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     Absolute
// +0e  ASL     ROL     LSR     ROR     STX     LDX     DEC     INC     Absolute
// +0f  SLO*    RLA*    SRE*    RRA*    SAX*    LAX*    DCP*    ISB*    Absolute

// +10  BPL     BMI     BVC     BVS     BCC     BCS     BNE     BEQ     Relative
// +11  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     (indir),y
// +12   t       t       t       t       t       t       t       t         ?
// +13  SLO*    RLA*    SRE*    RRA*    SHA**   LAX*    DCP*    ISB*    (indir),y
// +14  NOP*    NOP*    NOP*    NOP*    STY     LDY     NOP*    NOP*    Zeropage,x
// +15  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     Zeropage,x
// +16  ASL     ROL     LSR     ROR     STX  y) LDX  y) DEC     INC     Zeropage,x
// +17  SLO*    RLA*    SRE*    RRA*    SAX* y) LAX* y) DCP*    ISB*    Zeropage,x

// +18  CLC     SEC     CLI     SEI     TYA     CLV     CLD     SED     Implicit
// +19  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     Absolute,y
// +1a  NOP*    NOP*    NOP*    NOP*    TXS     TSX     NOP*    NOP*    Implicit
// +1b  SLO*    RLA*    SRE*    RRA*    SHS**   LAS**   DCP*    ISB*    Absolute,y
// +1c  NOP*    NOP*    NOP*    NOP*    SHY**   LDY     NOP*    NOP*    Absolute,x
// +1d  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     Absolute,x
// +1e  ASL     ROL     LSR     ROR     SHX**y) LDX  y) DEC     INC     Absolute,x
// +1f  SLO*    RLA*    SRE*    RRA*    SHA**y) LAX* y) DCP*    ISB*    Absolute,x

internal void
InitInstructionsDictionary(instruction_info* Instructions) {
    // https://www.nesdev.org/6502_cpu.txt
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +00  BRK     JSR     RTI     RTS     NOP*    LDY     CPY     CPX     Impl/immed
    Instructions[0x00].Mnemonic = BRK;
    Instructions[0x00].AddressingMode = Implicit;
    Instructions[0x20].Mnemonic = JSR;
    Instructions[0x20].AddressingMode = Implicit;
    Instructions[0x40].Mnemonic = RTI;
    Instructions[0x40].AddressingMode = Implicit;
    Instructions[0x60].Mnemonic = RTS;
    Instructions[0x60].AddressingMode = Implicit;
    Instructions[0x80].Mnemonic = NOP;
    Instructions[0x80].AddressingMode = Implicit;
    Instructions[0x80].Unofficial = 1;
    Instructions[0xA0].Mnemonic = LDY;
    Instructions[0xA0].AddressingMode = Implicit;
    Instructions[0xC0].Mnemonic = CPY;
    Instructions[0xC0].AddressingMode = Implicit;
    Instructions[0xE0].Mnemonic = CPX;
    Instructions[0xE0].AddressingMode = Implicit;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +01  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     (indir,x)
    Instructions[0x01].Mnemonic = ORA;
    Instructions[0x01].AddressingMode = IndirectX;
    Instructions[0x21].Mnemonic = AND;
    Instructions[0x21].AddressingMode = IndirectX;
    Instructions[0x41].Mnemonic = EOR;
    Instructions[0x41].AddressingMode = IndirectX;
    Instructions[0x61].Mnemonic = ADC;
    Instructions[0x61].AddressingMode = IndirectX;
    Instructions[0x81].Mnemonic = STA;
    Instructions[0x81].AddressingMode = IndirectX;
    Instructions[0xA1].Mnemonic = LDA;
    Instructions[0xA1].AddressingMode = IndirectX;
    Instructions[0xC1].Mnemonic = CMP;
    Instructions[0xC1].AddressingMode = IndirectX;
    Instructions[0xE1].Mnemonic = SBC;
    Instructions[0xE1].AddressingMode = IndirectX;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +02   t       t       t       t      NOP*t   LDX     NOP*t   NOP*t     ? /immed
    Instructions[0x02].Mnemonic = UNS;
    Instructions[0x02].AddressingMode = Implicit;
    Instructions[0x22].Mnemonic = UNS;
    Instructions[0x22].AddressingMode = Implicit;
    Instructions[0x42].Mnemonic = UNS;
    Instructions[0x42].AddressingMode = Implicit;
    Instructions[0x62].Mnemonic = UNS;
    Instructions[0x62].AddressingMode = Implicit;
    Instructions[0x82].Mnemonic = NOP;
    Instructions[0x82].AddressingMode = Implicit;
    Instructions[0x82].Unofficial = 1;
    Instructions[0xA2].Mnemonic = LDX;
    Instructions[0xA2].AddressingMode = Implicit;
    Instructions[0xC2].Mnemonic = NOP;
    Instructions[0xC2].AddressingMode = Implicit;
    Instructions[0xC2].Unofficial = 1;
    Instructions[0xE2].Mnemonic = NOP;
    Instructions[0xE2].AddressingMode = Implicit;
    Instructions[0xE2].Unofficial = 1;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +03  SLO*    RLA*    SRE*    RRA*    SAX*    LAX*    DCP*    ISB*    (indir,x)
    Instructions[0x03].Mnemonic = SLO;
    Instructions[0x03].AddressingMode = IndirectX;
    Instructions[0x03].Unofficial = 1;
    Instructions[0x23].Mnemonic = RLA;
    Instructions[0x23].AddressingMode = IndirectX;
    Instructions[0x23].Unofficial = 1;
    Instructions[0x43].Mnemonic = SRE;
    Instructions[0x43].AddressingMode = IndirectX;
    Instructions[0x43].Unofficial = 1;
    Instructions[0x63].Mnemonic = RRA;
    Instructions[0x63].AddressingMode = IndirectX;
    Instructions[0x63].Unofficial = 1;
    Instructions[0x83].Mnemonic = SAX;
    Instructions[0x83].AddressingMode = IndirectX;
    Instructions[0x83].Unofficial = 1;
    Instructions[0xA3].Mnemonic = LAX;
    Instructions[0xA3].AddressingMode = IndirectX;
    Instructions[0xA3].Unofficial = 1;
    Instructions[0xC3].Mnemonic = DCP;
    Instructions[0xC3].AddressingMode = IndirectX;
    Instructions[0xC3].Unofficial = 1;
    Instructions[0xE3].Mnemonic = ISB;
    Instructions[0xE3].AddressingMode = IndirectX;
    Instructions[0xE3].Unofficial = 1;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +04  NOP*    BIT     NOP*    NOP*    STY     LDY     CPY     CPX     Zeropage
    Instructions[0x04].Mnemonic = NOP;
    Instructions[0x04].AddressingMode = ZeroPageX;
    Instructions[0x04].Unofficial = 1;
    Instructions[0x24].Mnemonic = BIT;
    Instructions[0x24].AddressingMode = ZeroPageX;
    Instructions[0x44].Mnemonic = NOP;
    Instructions[0x44].AddressingMode = ZeroPageX;
    Instructions[0x44].Unofficial = 1;
    Instructions[0x64].Mnemonic = NOP;
    Instructions[0x64].AddressingMode = ZeroPageX;
    Instructions[0x64].Unofficial = 1;
    Instructions[0x84].Mnemonic = STY;
    Instructions[0x84].AddressingMode = ZeroPageX;
    Instructions[0xA4].Mnemonic = LDY;
    Instructions[0xA4].AddressingMode = ZeroPageX;
    Instructions[0xC4].Mnemonic = CPY;
    Instructions[0xC4].AddressingMode = ZeroPageX;
    Instructions[0xE4].Mnemonic = CPX;
    Instructions[0xE4].AddressingMode = ZeroPageX;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +05  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     Zeropage
    Instructions[0x05].Mnemonic = ORA;
    Instructions[0x05].AddressingMode = ZeroPage;
    Instructions[0x25].Mnemonic = AND;
    Instructions[0x25].AddressingMode = ZeroPage;
    Instructions[0x45].Mnemonic = EOR;
    Instructions[0x45].AddressingMode = ZeroPage;
    Instructions[0x65].Mnemonic = ADC;
    Instructions[0x65].AddressingMode = ZeroPage;
    Instructions[0x85].Mnemonic = STA;
    Instructions[0x85].AddressingMode = ZeroPage;
    Instructions[0xA5].Mnemonic = LDA;
    Instructions[0xA5].AddressingMode = ZeroPage;
    Instructions[0xC5].Mnemonic = CMP;
    Instructions[0xC5].AddressingMode = ZeroPage;
    Instructions[0xE5].Mnemonic = SBC;
    Instructions[0xE5].AddressingMode = ZeroPage;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +06  ASL     ROL     LSR     ROR     STX     LDX     DEC     INC     Zeropage
    Instructions[0x06].Mnemonic = ASL;
    Instructions[0x06].AddressingMode = ZeroPage;
    Instructions[0x26].Mnemonic = ROL;
    Instructions[0x26].AddressingMode = ZeroPage;
    Instructions[0x46].Mnemonic = LSR;
    Instructions[0x46].AddressingMode = ZeroPage;
    Instructions[0x66].Mnemonic = ROR;
    Instructions[0x66].AddressingMode = ZeroPage;
    Instructions[0x86].Mnemonic = STX;
    Instructions[0x86].AddressingMode = ZeroPage;
    Instructions[0xA6].Mnemonic = LDX;
    Instructions[0xA6].AddressingMode = ZeroPage;
    Instructions[0xC6].Mnemonic = DEC;
    Instructions[0xC6].AddressingMode = ZeroPage;
    Instructions[0xE6].Mnemonic = INC;
    Instructions[0xE6].AddressingMode = ZeroPage;
}

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

#define MapperNROM (0)

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

#define RamSize (1024 * 2)
#define PrgBankSize (16384)

internal u8
MemoryRead(bus* Bus, u16 Address) {
    if (Address >= 0x4020 && Address <= 0xFFFF) {
        //Mapper space
        Assert(Bus->Rom->MapperId == MapperNROM);
        
        u16 BaseAddress = Address - 0x4020;

        

        if (Bus->Rom->PrgRomBankCount == 2) {
            return Bus->Rom->Prg[BaseAddress];
        } else if (Bus->Rom->PrgRomBankCount == 1) {
            if (BaseAddress > PrgBankSize) {
                BaseAddress -= PrgBankSize;
                return Bus->Rom->Prg[BaseAddress];
            } else {
                return Bus->Rom->Prg[BaseAddress];
            }
        } else {
            Halt("Not sure, need debugging.");
        }
    }

    return 0xEA;
}

internal void
MemoryWrite(bus* Bus, u16 Address, u8 Value) {
    if (Address >= 0x2000 && Address <= 0X2007) {
        // PPU Registers
        //TODO: need to store properly
    } else if (Address >= 0x0000 && Address <= 0X1FFF) {
        // RAM
        DumpU16HexExpression(Address);
        DumpU8HexExpression(Value);
        u16 RamBaseAddress = Address % RamSize;
        Bus->Ram[RamBaseAddress] = Value;
    } else {
        DumpU16HexExpression(Address);
        DumpU8HexExpression(Value);
        Halt("TODO: Need writing!");
    }
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

    InitInstructionsDictionary(Instructions);

    loaded_file RomFile = LoadFile(ROM_PATH, TempMemory);

    Assert(RomFile.Data[0] == 0x4E);
    Assert(RomFile.Data[1] == 0x45);
    Assert(RomFile.Data[2] == 0x53);
    Assert(RomFile.Data[3] == 0x1A);

    u8* Ram[RamSize] = {0};

    rom Rom = ParseRom(RomFile);
    bus Bus = {0};
    Bus.Rom = &Rom;
    Bus.Ram = (u8*)Ram;

    m6502_t Cpu;
    m6502_desc_t CpuDesc = {0};
    uint64_t Pins = m6502_init(&Cpu, &CpuDesc);
    for (i32 TickIndex = 0; TickIndex < 10; TickIndex++) {
    // while (1) {
        Pins = m6502_tick(&Cpu, Pins);
        u16 Address = M6502_GET_ADDR(Pins);
        if (Pins & M6502_RW) {
            u8 MemoryValue = MemoryRead(&Bus, Address);
            M6502_SET_DATA(Pins, MemoryValue);
        } else {
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
