#ifndef BUS_H
#define BUS_H

#define MapperNROM (0)

#define RamSize (1024 * 2)
#define PrgBankSize (16384)

internal u8
MemoryRead(bus* Bus, u16 Address) {
    if (Address >= 0x8000 && Address <= 0xFFFF) {
        //TODO: NROM memory mapping need fixing, Mario starts at $0000 instead of $8000
        //Mapper space
        Assert(Bus->Rom->MapperId == MapperNROM);
        
        u16 BaseAddress = Address - 0x8000;

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

#endif
