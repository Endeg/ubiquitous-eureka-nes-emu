#ifndef _EMU_ROM_H
#define _EMU_ROM_H

#include "base.h"
#include "emu_types.h"
#include "file_io.h"

#define MapperNROM (0)

#define RamSize (1024 * 2)
#define PrgBankSize (16384)
#define ChrBankSize (1024 * 8)

#define INesHeaderSize    (16)
#define INesTrainerSize   (512)
#define INesPrgBanksCount (4)
#define INesChrBanksCount (5)
#define INesFlags6        (6)
#define INesFlags7        (7)

#define INesFlags6Mirroring       (0b00000001)
#define INesFlags6PrgRam          (0b00000010)
#define INesFlags6Trainer         (0b00000100)
#define INesFlags6IgnoreMirroring (0b00001000)
#define INesFlags6MapperIdLow     (0b11110000)

#define INesFlags7MapperIdHigh    (0b11110000)

internal rom ParseRom(loaded_file LoadedFile) {
    rom Result = {0};

    Result.PrgRomBankCount = LoadedFile.Data[INesPrgBanksCount];
    Result.ChrRomBankCount = LoadedFile.Data[INesChrBanksCount];
    Result.MapperId = (LoadedFile.Data[INesFlags7] & INesFlags7MapperIdHigh) | ((LoadedFile.Data[INesFlags6] & INesFlags6MapperIdLow) >> 4);
    Result.Mirroring = (LoadedFile.Data[INesFlags6] & INesFlags6Mirroring) ? Vertical : Horizontal;
    Result.IgnoreMirroring = LoadedFile.Data[INesFlags6] & INesFlags6IgnoreMirroring;
    Result.HasPrgRam = LoadedFile.Data[INesFlags6] & INesFlags6PrgRam;
    Result.HasTrainer = LoadedFile.Data[INesFlags6] & INesFlags6Trainer;

    u8* PrgSectionStart =
        LoadedFile.Data +
        INesHeaderSize + ((Result.HasTrainer) ? INesTrainerSize : 0);

    Result.Prg = PrgSectionStart;
    Result.Chr = PrgSectionStart + (PrgBankSize * Result.PrgRomBankCount);

    Assert(!Result.HasPrgRam);
    Assert(Result.MapperId == MapperNROM);

    return Result;
}

#endif
