#ifndef _EMU_DISASSEMBLY_H
#define _EMU_DISASSEMBLY_H

#include "base.h"

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
    Indirect,
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
  TXA, TXS, TYA, HLT,
} instruction_mnemonic;

typedef struct instruction_info {
    instruction_mnemonic Mnemonic;
    addressing_mode AddressingMode;
    bool32 Unofficial;
} instruction_info;

internal u8*
MnemonicToString(instruction_mnemonic Mnemonic) {
    switch (Mnemonic) {
        case ADC: return "ADC";
        case ANC: return "ANC";
        case AND: return "AND";
        case ANE: return "ANE";
        case ARR: return "ARR";
        case ASL: return "ASL";
        case ASR: return "ASR";
        case BCC: return "BCC";
        case BCS: return "BCS";
        case BEQ: return "BEQ";
        case BIT: return "BIT";
        case BMI: return "BMI";
        case BNE: return "BNE";
        case BPL: return "BPL";
        case BRK: return "BRK";
        case BVC: return "BVC";
        case BVS: return "BVS";
        case CLC: return "CLC";
        case CLD: return "CLD";
        case CLI: return "CLI";
        case CLV: return "CLV";
        case CMP: return "CMP";
        case CPX: return "CPX";
        case CPY: return "CPY";
        case DCP: return "DCP";
        case DEC: return "DEC";
        case DEX: return "DEX";
        case DEY: return "DEY";
        case EOR: return "EOR";
        case INC: return "INC";
        case INX: return "INX";
        case INY: return "INY";
        case ISB: return "ISB";
        case JMP: return "JMP";
        case JSR: return "JSR";
        case LAS: return "LAS";
        case LAX: return "LAX";
        case LDA: return "LDA";
        case LDX: return "LDX";
        case LDY: return "LDY";
        case LSR: return "LSR";
        case LXA: return "LXA";
        case NOP: return "NOP";
        case ORA: return "ORA";
        case PHA: return "PHA";
        case PHP: return "PHP";
        case PLA: return "PLA";
        case PLP: return "PLP";
        case RLA: return "RLA";
        case ROL: return "ROL";
        case ROR: return "ROR";
        case RRA: return "RRA";
        case RTI: return "RTI";
        case RTS: return "RTS";
        case SAX: return "SAX";
        case SBC: return "SBC";
        case SBX: return "SBX";
        case SEC: return "SEC";
        case SED: return "SED";
        case SEI: return "SEI";
        case SHA: return "SHA";
        case SHS: return "SHS";
        case SHX: return "SHX";
        case SHY: return "SHY";
        case SLO: return "SLO";
        case SRE: return "SRE";
        case STA: return "STA";
        case STX: return "STX";
        case STY: return "STY";
        case TAX: return "TAX";
        case TAY: return "TAY";
        case TSX: return "TSX";
        case TXA: return "TXA";
        case TXS: return "TXS";
        case TYA: return "TYA";
        case HLT: return "HLT";
        default : return "???";
    }
}

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
    Instructions[0x80].AddressingMode = Immediate;
    Instructions[0x80].Unofficial = 1;
    Instructions[0xA0].Mnemonic = LDY;
    Instructions[0xA0].AddressingMode = Immediate;
    Instructions[0xC0].Mnemonic = CPY;
    Instructions[0xC0].AddressingMode = Immediate;
    Instructions[0xE0].Mnemonic = CPX;
    Instructions[0xE0].AddressingMode = Immediate;
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
    Instructions[0x02].Mnemonic = HLT;
    Instructions[0x02].AddressingMode = Implicit;
    Instructions[0x22].Mnemonic = HLT;
    Instructions[0x22].AddressingMode = Implicit;
    Instructions[0x42].Mnemonic = HLT;
    Instructions[0x42].AddressingMode = Implicit;
    Instructions[0x62].Mnemonic = HLT;
    Instructions[0x62].AddressingMode = Implicit;
    Instructions[0x82].Mnemonic = NOP;
    Instructions[0x82].AddressingMode = Immediate;
    Instructions[0x82].Unofficial = 1;
    Instructions[0xA2].Mnemonic = LDX;
    Instructions[0xA2].AddressingMode = Immediate;
    Instructions[0xC2].Mnemonic = NOP;
    Instructions[0xC2].AddressingMode = Immediate;
    Instructions[0xC2].Unofficial = 1;
    Instructions[0xE2].Mnemonic = NOP;
    Instructions[0xE2].AddressingMode = Immediate;
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
    Instructions[0x04].AddressingMode = ZeroPage;
    Instructions[0x04].Unofficial = 1;
    Instructions[0x24].Mnemonic = BIT;
    Instructions[0x24].AddressingMode = ZeroPageX;
    Instructions[0x44].Mnemonic = NOP;
    Instructions[0x44].AddressingMode = ZeroPage;
    Instructions[0x44].Unofficial = 1;
    Instructions[0x64].Mnemonic = NOP;
    Instructions[0x64].AddressingMode = ZeroPage;
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
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +07  SLO*    RLA*    SRE*    RRA*    SAX*    LAX*    DCP*    ISB*    Zeropage
    Instructions[0x07].Mnemonic = SLO;
    Instructions[0x07].AddressingMode = ZeroPage;
    Instructions[0x07].Unofficial = 1;
    Instructions[0x27].Mnemonic = RLA;
    Instructions[0x27].AddressingMode = ZeroPage;
    Instructions[0x27].Unofficial = 1;
    Instructions[0x47].Mnemonic = SRE;
    Instructions[0x47].AddressingMode = ZeroPage;
    Instructions[0x47].Unofficial = 1;
    Instructions[0x67].Mnemonic = RRA;
    Instructions[0x67].AddressingMode = ZeroPage;
    Instructions[0x67].Unofficial = 1;
    Instructions[0x87].Mnemonic = SAX;
    Instructions[0x87].AddressingMode = ZeroPage;
    Instructions[0x87].Unofficial = 1;
    Instructions[0xA7].Mnemonic = LAX;
    Instructions[0xA7].AddressingMode = ZeroPage;
    Instructions[0xA7].Unofficial = 1;
    Instructions[0xC7].Mnemonic = DCP;
    Instructions[0xC7].AddressingMode = ZeroPage;
    Instructions[0xC7].Unofficial = 1;
    Instructions[0xE7].Mnemonic = ISB;
    Instructions[0xE7].AddressingMode = ZeroPage;
    Instructions[0xE7].Unofficial = 1;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +08  PHP     PLP     PHA     PLA     DEY     TAY     INY     INX     Implicit
    Instructions[0x08].Mnemonic = PHP;
    Instructions[0x08].AddressingMode = Implicit;
    Instructions[0x28].Mnemonic = PLP;
    Instructions[0x28].AddressingMode = Implicit;
    Instructions[0x48].Mnemonic = PHA;
    Instructions[0x48].AddressingMode = Implicit;
    Instructions[0x68].Mnemonic = PLA;
    Instructions[0x68].AddressingMode = Implicit;
    Instructions[0x88].Mnemonic = DEY;
    Instructions[0x88].AddressingMode = Implicit;
    Instructions[0xA8].Mnemonic = TAY;
    Instructions[0xA8].AddressingMode = Implicit;
    Instructions[0xC8].Mnemonic = INY;
    Instructions[0xC8].AddressingMode = Implicit;
    Instructions[0xE8].Mnemonic = INX;
    Instructions[0xE8].AddressingMode = Implicit;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +09  ORA     AND     EOR     ADC     NOP*    LDA     CMP     SBC     Immediate
    Instructions[0x09].Mnemonic = ORA;
    Instructions[0x09].AddressingMode = Immediate;
    Instructions[0x29].Mnemonic = AND;
    Instructions[0x29].AddressingMode = Immediate;
    Instructions[0x49].Mnemonic = EOR;
    Instructions[0x49].AddressingMode = Immediate;
    Instructions[0x69].Mnemonic = ADC;
    Instructions[0x69].AddressingMode = Immediate;
    Instructions[0x89].Mnemonic = NOP;
    Instructions[0x89].AddressingMode = Immediate;
    Instructions[0x89].Unofficial = 1;
    Instructions[0xA9].Mnemonic = LDA;
    Instructions[0xA9].AddressingMode = Immediate;
    Instructions[0xC9].Mnemonic = CMP;
    Instructions[0xC9].AddressingMode = Immediate;
    Instructions[0xE9].Mnemonic = SBC;
    Instructions[0xE9].AddressingMode = Immediate;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +0a  ASL     ROL     LSR     ROR     TXA     TAX     DEX     NOP     Accu/impl
    Instructions[0x0A].Mnemonic = ASL;
    Instructions[0x0A].AddressingMode = Accumulator;
    Instructions[0x2A].Mnemonic = ROL;
    Instructions[0x2A].AddressingMode = Accumulator;
    Instructions[0x4A].Mnemonic = LSR;
    Instructions[0x4A].AddressingMode = Accumulator;
    Instructions[0x6A].Mnemonic = ROR;
    Instructions[0x6A].AddressingMode = Accumulator;
    Instructions[0x8A].Mnemonic = TXA;
    Instructions[0x8A].AddressingMode = Implicit;
    Instructions[0xAA].Mnemonic = TAX;
    Instructions[0xAA].AddressingMode = Implicit;
    Instructions[0xCA].Mnemonic = DEX;
    Instructions[0xCA].AddressingMode = Implicit;
    Instructions[0xEA].Mnemonic = NOP;
    Instructions[0xEA].AddressingMode = Implicit;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +0b  ANC**   ANC**   ASR**   ARR**   ANE**   LXA**   SBX**   SBC*    Immediate
    Instructions[0x0B].Mnemonic = ANC;
    Instructions[0x0B].AddressingMode = Immediate;
    Instructions[0x0B].Unofficial = 1;
    Instructions[0x2B].Mnemonic = ANC;
    Instructions[0x2B].AddressingMode = Immediate;
    Instructions[0x2B].Unofficial = 1;
    Instructions[0x4B].Mnemonic = ASR;
    Instructions[0x4B].AddressingMode = Immediate;
    Instructions[0x4B].Unofficial = 1;
    Instructions[0x6B].Mnemonic = ARR;
    Instructions[0x6B].AddressingMode = Immediate;
    Instructions[0x6B].Unofficial = 1;
    Instructions[0x8B].Mnemonic = ANE;
    Instructions[0x8B].AddressingMode = Immediate;
    Instructions[0x8B].Unofficial = 1;
    Instructions[0xAB].Mnemonic = LXA;
    Instructions[0xAB].AddressingMode = Immediate;
    Instructions[0xAB].Unofficial = 1;
    Instructions[0xCB].Mnemonic = SBX;
    Instructions[0xCB].AddressingMode = Immediate;
    Instructions[0xCB].Unofficial = 1;
    Instructions[0xEB].Mnemonic = SBC;
    Instructions[0xEB].AddressingMode = Immediate;
    Instructions[0xEB].Unofficial = 1;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +0c  NOP*    BIT     JMP     JMP ()  STY     LDY     CPY     CPX     Absolute
    Instructions[0x0C].Mnemonic = NOP;
    Instructions[0x0C].AddressingMode = Absolute;
    Instructions[0x0C].Unofficial = 1;
    Instructions[0x2C].Mnemonic = BIT;
    Instructions[0x2C].AddressingMode = Absolute;
    Instructions[0x4C].Mnemonic = JMP;
    Instructions[0x4C].AddressingMode = Absolute;
    Instructions[0x6C].Mnemonic = JMP;
    Instructions[0x6C].AddressingMode = Indirect;
    Instructions[0x8C].Mnemonic = STY;
    Instructions[0x8C].AddressingMode = Absolute;
    Instructions[0xAC].Mnemonic = LDY;
    Instructions[0xAC].AddressingMode = Absolute;
    Instructions[0xCC].Mnemonic = CPY;
    Instructions[0xCC].AddressingMode = Absolute;
    Instructions[0xEC].Mnemonic = CPX;
    Instructions[0xEC].AddressingMode = Absolute;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +0d  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     Absolute
    Instructions[0x0D].Mnemonic = ORA;
    Instructions[0x0D].AddressingMode = Absolute;
    Instructions[0x2D].Mnemonic = AND;
    Instructions[0x2D].AddressingMode = Absolute;
    Instructions[0x4D].Mnemonic = EOR;
    Instructions[0x4D].AddressingMode = Absolute;
    Instructions[0x6D].Mnemonic = ADC;
    Instructions[0x6D].AddressingMode = Absolute;
    Instructions[0x8D].Mnemonic = STA;
    Instructions[0x8D].AddressingMode = Absolute;
    Instructions[0xAD].Mnemonic = LDA;
    Instructions[0xAD].AddressingMode = Absolute;
    Instructions[0xCD].Mnemonic = CMP;
    Instructions[0xCD].AddressingMode = Absolute;
    Instructions[0xED].Mnemonic = SBC;
    Instructions[0xED].AddressingMode = Absolute;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +0e  ASL     ROL     LSR     ROR     STX     LDX     DEC     INC     Absolute
    Instructions[0x0E].Mnemonic = ASL;
    Instructions[0x0E].AddressingMode = Absolute;
    Instructions[0x2E].Mnemonic = ROL;
    Instructions[0x2E].AddressingMode = Absolute;
    Instructions[0x4E].Mnemonic = LSR;
    Instructions[0x4E].AddressingMode = Absolute;
    Instructions[0x6E].Mnemonic = ROR;
    Instructions[0x6E].AddressingMode = Absolute;
    Instructions[0x8E].Mnemonic = STX;
    Instructions[0x8E].AddressingMode = Absolute;
    Instructions[0xAE].Mnemonic = LDX;
    Instructions[0xAE].AddressingMode = Absolute;
    Instructions[0xCE].Mnemonic = DEC;
    Instructions[0xCE].AddressingMode = Absolute;
    Instructions[0xEE].Mnemonic = INC;
    Instructions[0xEE].AddressingMode = Absolute;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +0f  SLO*    RLA*    SRE*    RRA*    SAX*    LAX*    DCP*    ISB*    Absolute
    Instructions[0x0F].Mnemonic = SLO;
    Instructions[0x0F].AddressingMode = Absolute;
    Instructions[0x0F].Unofficial = 1;
    Instructions[0x2F].Mnemonic = RLA;
    Instructions[0x2F].AddressingMode = Absolute;
    Instructions[0x2F].Unofficial = 1;
    Instructions[0x4F].Mnemonic = SRE;
    Instructions[0x4F].AddressingMode = Absolute;
    Instructions[0x4F].Unofficial = 1;
    Instructions[0x6F].Mnemonic = RRA;
    Instructions[0x6F].AddressingMode = Absolute;
    Instructions[0x6F].Unofficial = 1;
    Instructions[0x8F].Mnemonic = SAX;
    Instructions[0x8F].AddressingMode = Absolute;
    Instructions[0x8F].Unofficial = 1;
    Instructions[0xAF].Mnemonic = LAX;
    Instructions[0xAF].AddressingMode = Absolute;
    Instructions[0xAF].Unofficial = 1;
    Instructions[0xCF].Mnemonic = DCP;
    Instructions[0xCF].AddressingMode = Absolute;
    Instructions[0xCF].Unofficial = 1;
    Instructions[0xEF].Mnemonic = ISB;
    Instructions[0xEF].AddressingMode = Absolute;
    Instructions[0xEF].Unofficial = 1;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +10  BPL     BMI     BVC     BVS     BCC     BCS     BNE     BEQ     Relative
    Instructions[0x10].Mnemonic = BPL;
    Instructions[0x10].AddressingMode = Relative;
    Instructions[0x30].Mnemonic = BMI;
    Instructions[0x30].AddressingMode = Relative;
    Instructions[0x50].Mnemonic = BVC;
    Instructions[0x50].AddressingMode = Relative;
    Instructions[0x70].Mnemonic = BVS;
    Instructions[0x70].AddressingMode = Relative;
    Instructions[0x90].Mnemonic = BCC;
    Instructions[0x90].AddressingMode = Relative;
    Instructions[0xB0].Mnemonic = BCS;
    Instructions[0xB0].AddressingMode = Relative;
    Instructions[0xD0].Mnemonic = BNE;
    Instructions[0xD0].AddressingMode = Relative;
    Instructions[0xF0].Mnemonic = BEQ;
    Instructions[0xF0].AddressingMode = Relative;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +11  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     (indir),y
    Instructions[0x11].Mnemonic = ORA;
    Instructions[0x11].AddressingMode = IndirectY;
    Instructions[0x31].Mnemonic = AND;
    Instructions[0x31].AddressingMode = IndirectY;
    Instructions[0x51].Mnemonic = EOR;
    Instructions[0x51].AddressingMode = IndirectY;
    Instructions[0x71].Mnemonic = ADC;
    Instructions[0x71].AddressingMode = IndirectY;
    Instructions[0x91].Mnemonic = STA;
    Instructions[0x91].AddressingMode = IndirectY;
    Instructions[0xB1].Mnemonic = LDA;
    Instructions[0xB1].AddressingMode = IndirectY;
    Instructions[0xD1].Mnemonic = CMP;
    Instructions[0xD1].AddressingMode = IndirectY;
    Instructions[0xF1].Mnemonic = SBC;
    Instructions[0xF1].AddressingMode = IndirectY;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +12   t       t       t       t       t       t       t       t         ?
    Instructions[0x12].Mnemonic = HLT;
    Instructions[0x12].AddressingMode = Implicit;
    Instructions[0x32].Mnemonic = HLT;
    Instructions[0x32].AddressingMode = Implicit;
    Instructions[0x52].Mnemonic = HLT;
    Instructions[0x52].AddressingMode = Implicit;
    Instructions[0x72].Mnemonic = HLT;
    Instructions[0x72].AddressingMode = Implicit;
    Instructions[0x92].Mnemonic = HLT;
    Instructions[0x92].AddressingMode = Implicit;
    Instructions[0xB2].Mnemonic = HLT;
    Instructions[0xB2].AddressingMode = Implicit;
    Instructions[0xD2].Mnemonic = HLT;
    Instructions[0xD2].AddressingMode = Implicit;
    Instructions[0xF2].Mnemonic = HLT;
    Instructions[0xF2].AddressingMode = Implicit;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +13  SLO*    RLA*    SRE*    RRA*    SHA**   LAX*    DCP*    ISB*    (indir),y
    Instructions[0x13].Mnemonic = SLO;
    Instructions[0x13].AddressingMode = IndirectY;
    Instructions[0x13].Unofficial = 1;
    Instructions[0x33].Mnemonic = RLA;
    Instructions[0x33].AddressingMode = IndirectY;
    Instructions[0x33].Unofficial = 1;
    Instructions[0x53].Mnemonic = SRE;
    Instructions[0x53].AddressingMode = IndirectY;
    Instructions[0x53].Unofficial = 1;
    Instructions[0x73].Mnemonic = RRA;
    Instructions[0x73].AddressingMode = IndirectY;
    Instructions[0x73].Unofficial = 1;
    Instructions[0x93].Mnemonic = SHA;
    Instructions[0x93].AddressingMode = IndirectY;
    Instructions[0x93].Unofficial = 1;
    Instructions[0xB3].Mnemonic = LAX;
    Instructions[0xB3].AddressingMode = IndirectY;
    Instructions[0xB3].Unofficial = 1;
    Instructions[0xD3].Mnemonic = DCP;
    Instructions[0xD3].AddressingMode = IndirectY;
    Instructions[0xD3].Unofficial = 1;
    Instructions[0xF3].Mnemonic = ISB;
    Instructions[0xF3].AddressingMode = IndirectY;
    Instructions[0xF3].Unofficial = 1;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +14  NOP*    NOP*    NOP*    NOP*    STY     LDY     NOP*    NOP*    Zeropage,x
    Instructions[0x14].Mnemonic = NOP;
    Instructions[0x14].AddressingMode = ZeroPageX;
    Instructions[0x14].Unofficial = 1;
    Instructions[0x34].Mnemonic = NOP;
    Instructions[0x34].AddressingMode = ZeroPageX;
    Instructions[0x34].Unofficial = 1;
    Instructions[0x54].Mnemonic = NOP;
    Instructions[0x54].AddressingMode = ZeroPageX;
    Instructions[0x54].Unofficial = 1;
    Instructions[0x74].Mnemonic = NOP;
    Instructions[0x74].AddressingMode = ZeroPageX;
    Instructions[0x74].Unofficial = 1;
    Instructions[0x94].Mnemonic = STY;
    Instructions[0x94].AddressingMode = ZeroPageX;
    Instructions[0xB4].Mnemonic = LDY;
    Instructions[0xB4].AddressingMode = ZeroPageX;
    Instructions[0xD4].Mnemonic = NOP;
    Instructions[0xD4].AddressingMode = ZeroPageX;
    Instructions[0xD4].Unofficial = 1;
    Instructions[0xF4].Mnemonic = NOP;
    Instructions[0xF4].AddressingMode = ZeroPageX;
    Instructions[0xF4].Unofficial = 1;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +15  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     Zeropage,x
    Instructions[0x15].Mnemonic = ORA;
    Instructions[0x15].AddressingMode = ZeroPageX;
    Instructions[0x35].Mnemonic = AND;
    Instructions[0x35].AddressingMode = ZeroPageX;
    Instructions[0x55].Mnemonic = EOR;
    Instructions[0x55].AddressingMode = ZeroPageX;
    Instructions[0x75].Mnemonic = ADC;
    Instructions[0x75].AddressingMode = ZeroPageX;
    Instructions[0x95].Mnemonic = STA;
    Instructions[0x95].AddressingMode = ZeroPageX;
    Instructions[0xB5].Mnemonic = LDA;
    Instructions[0xB5].AddressingMode = ZeroPageX;
    Instructions[0xD5].Mnemonic = CMP;
    Instructions[0xD5].AddressingMode = ZeroPageX;
    Instructions[0xF5].Mnemonic = SBC;
    Instructions[0xF5].AddressingMode = ZeroPageX;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +16  ASL     ROL     LSR     ROR     STX  y) LDX  y) DEC     INC     Zeropage,x
    Instructions[0x16].Mnemonic = ASL;
    Instructions[0x16].AddressingMode = ZeroPageX;
    Instructions[0x36].Mnemonic = ROL;
    Instructions[0x36].AddressingMode = ZeroPageX;
    Instructions[0x56].Mnemonic = LSR;
    Instructions[0x56].AddressingMode = ZeroPageX;
    Instructions[0x76].Mnemonic = ROR;
    Instructions[0x76].AddressingMode = ZeroPageX;
    Instructions[0x96].Mnemonic = STX;
    Instructions[0x96].AddressingMode = ZeroPageY;
    Instructions[0xB6].Mnemonic = LDX;
    Instructions[0xB6].AddressingMode = ZeroPageY;
    Instructions[0xD6].Mnemonic = DEC;
    Instructions[0xD6].AddressingMode = ZeroPageX;
    Instructions[0xF6].Mnemonic = INC;
    Instructions[0xF6].AddressingMode = ZeroPageX;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +17  SLO*    RLA*    SRE*    RRA*    SAX* y) LAX* y) DCP*    ISB*    Zeropage,x
    Instructions[0x17].Mnemonic = SLO;
    Instructions[0x17].AddressingMode = ZeroPageX;
    Instructions[0x17].Unofficial = 1;
    Instructions[0x37].Mnemonic = RLA;
    Instructions[0x37].AddressingMode = ZeroPageX;
    Instructions[0x37].Unofficial = 1;
    Instructions[0x57].Mnemonic = SRE;
    Instructions[0x57].AddressingMode = ZeroPageX;
    Instructions[0x57].Unofficial = 1;
    Instructions[0x77].Mnemonic = RRA;
    Instructions[0x77].AddressingMode = ZeroPageX;
    Instructions[0x77].Unofficial = 1;
    Instructions[0x97].Mnemonic = SAX;
    Instructions[0x97].AddressingMode = ZeroPageY;
    Instructions[0x97].Unofficial = 1;
    Instructions[0xB7].Mnemonic = LAX;
    Instructions[0xB7].AddressingMode = ZeroPageY;
    Instructions[0xB7].Unofficial = 1;
    Instructions[0xD7].Mnemonic = DCP;
    Instructions[0xD7].AddressingMode = ZeroPageX;
    Instructions[0xD7].Unofficial = 1;
    Instructions[0xF7].Mnemonic = ISB;
    Instructions[0xF7].AddressingMode = ZeroPageX;
    Instructions[0xF7].Unofficial = 1;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +18  CLC     SEC     CLI     SEI     TYA     CLV     CLD     SED     Implicit
    Instructions[0x18].Mnemonic = CLC;
    Instructions[0x18].AddressingMode = Implicit;
    Instructions[0x38].Mnemonic = SEC;
    Instructions[0x38].AddressingMode = Implicit;
    Instructions[0x58].Mnemonic = CLI;
    Instructions[0x58].AddressingMode = Implicit;
    Instructions[0x78].Mnemonic = SEI;
    Instructions[0x78].AddressingMode = Implicit;
    Instructions[0x98].Mnemonic = TYA;
    Instructions[0x98].AddressingMode = Implicit;
    Instructions[0xB8].Mnemonic = CLV;
    Instructions[0xB8].AddressingMode = Implicit;
    Instructions[0xD8].Mnemonic = CLD;
    Instructions[0xD8].AddressingMode = Implicit;
    Instructions[0xF8].Mnemonic = SED;
    Instructions[0xF8].AddressingMode = Implicit;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +19  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     Absolute,y
    Instructions[0x19].Mnemonic = ORA;
    Instructions[0x19].AddressingMode = AbsoluteY;
    Instructions[0x39].Mnemonic = AND;
    Instructions[0x39].AddressingMode = AbsoluteY;
    Instructions[0x59].Mnemonic = EOR;
    Instructions[0x59].AddressingMode = AbsoluteY;
    Instructions[0x79].Mnemonic = ADC;
    Instructions[0x79].AddressingMode = AbsoluteY;
    Instructions[0x99].Mnemonic = STA;
    Instructions[0x99].AddressingMode = AbsoluteY;
    Instructions[0xB9].Mnemonic = LDA;
    Instructions[0xB9].AddressingMode = AbsoluteY;
    Instructions[0xD9].Mnemonic = CMP;
    Instructions[0xD9].AddressingMode = AbsoluteY;
    Instructions[0xF9].Mnemonic = SBC;
    Instructions[0xF9].AddressingMode = AbsoluteY;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +1a  NOP*    NOP*    NOP*    NOP*    TXS     TSX     NOP*    NOP*    Implicit
    Instructions[0x1A].Mnemonic = NOP;
    Instructions[0x1A].AddressingMode = Implicit;
    Instructions[0x1A].Unofficial = 1;
    Instructions[0x3A].Mnemonic = NOP;
    Instructions[0x3A].AddressingMode = Implicit;
    Instructions[0x3A].Unofficial = 1;
    Instructions[0x5A].Mnemonic = NOP;
    Instructions[0x5A].AddressingMode = Implicit;
    Instructions[0x5A].Unofficial = 1;
    Instructions[0x7A].Mnemonic = NOP;
    Instructions[0x7A].AddressingMode = Implicit;
    Instructions[0x7A].Unofficial = 1;
    Instructions[0x9A].Mnemonic = TXS;
    Instructions[0x9A].AddressingMode = Implicit;
    Instructions[0xBA].Mnemonic = TSX;
    Instructions[0xBA].AddressingMode = Implicit;
    Instructions[0xDA].Mnemonic = NOP;
    Instructions[0xDA].AddressingMode = Implicit;
    Instructions[0xDA].Unofficial = 1;
    Instructions[0xFA].Mnemonic = NOP;
    Instructions[0xFA].AddressingMode = Implicit;
    Instructions[0xFA].Unofficial = 1;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +1b  SLO*    RLA*    SRE*    RRA*    SHS**   LAS**   DCP*    ISB*    Absolute,y
    Instructions[0x1B].Mnemonic = SLO;
    Instructions[0x1B].AddressingMode = AbsoluteY;
    Instructions[0x1B].Unofficial = 1;
    Instructions[0x3B].Mnemonic = RLA;
    Instructions[0x3B].AddressingMode = AbsoluteY;
    Instructions[0x3B].Unofficial = 1;
    Instructions[0x5B].Mnemonic = SRE;
    Instructions[0x5B].AddressingMode = AbsoluteY;
    Instructions[0x5B].Unofficial = 1;
    Instructions[0x7B].Mnemonic = RRA;
    Instructions[0x7B].AddressingMode = AbsoluteY;
    Instructions[0x7B].Unofficial = 1;
    Instructions[0x9B].Mnemonic = SHS;
    Instructions[0x9B].AddressingMode = AbsoluteY;
    Instructions[0x9B].Unofficial = 1;
    Instructions[0xBB].Mnemonic = LAS;
    Instructions[0xBB].AddressingMode = AbsoluteY;
    Instructions[0xBB].Unofficial = 1;
    Instructions[0xDB].Mnemonic = DCP;
    Instructions[0xDB].AddressingMode = AbsoluteY;
    Instructions[0xDB].Unofficial = 1;
    Instructions[0xFB].Mnemonic = ISB;
    Instructions[0xFB].AddressingMode = AbsoluteY;
    Instructions[0xFB].Unofficial = 1;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +1c  NOP*    NOP*    NOP*    NOP*    SHY**   LDY     NOP*    NOP*    Absolute,x
    Instructions[0x1C].Mnemonic = NOP;
    Instructions[0x1C].AddressingMode = AbsoluteX;
    Instructions[0x1C].Unofficial = 1;
    Instructions[0x3C].Mnemonic = NOP;
    Instructions[0x3C].AddressingMode = AbsoluteX;
    Instructions[0x3C].Unofficial = 1;
    Instructions[0x5C].Mnemonic = NOP;
    Instructions[0x5C].AddressingMode = AbsoluteX;
    Instructions[0x5C].Unofficial = 1;
    Instructions[0x7C].Mnemonic = NOP;
    Instructions[0x7C].AddressingMode = AbsoluteX;
    Instructions[0x7C].Unofficial = 1;
    Instructions[0x9C].Mnemonic = SHY;
    Instructions[0x9C].AddressingMode = AbsoluteX;
    Instructions[0x9C].Unofficial = 1;
    Instructions[0xBC].Mnemonic = LDY;
    Instructions[0xBC].AddressingMode = AbsoluteX;
    Instructions[0xDC].Mnemonic = NOP;
    Instructions[0xDC].AddressingMode = AbsoluteX;
    Instructions[0xDC].Unofficial = 1;
    Instructions[0xFC].Mnemonic = NOP;
    Instructions[0xFC].AddressingMode = AbsoluteX;
    Instructions[0xFC].Unofficial = 1;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +1d  ORA     AND     EOR     ADC     STA     LDA     CMP     SBC     Absolute,x
    Instructions[0x1D].Mnemonic = ORA;
    Instructions[0x1D].AddressingMode = AbsoluteX;
    Instructions[0x3D].Mnemonic = AND;
    Instructions[0x3D].AddressingMode = AbsoluteX;
    Instructions[0x5D].Mnemonic = EOR;
    Instructions[0x5D].AddressingMode = AbsoluteX;
    Instructions[0x7D].Mnemonic = ADC;
    Instructions[0x7D].AddressingMode = AbsoluteX;
    Instructions[0x9D].Mnemonic = STA;
    Instructions[0x9D].AddressingMode = AbsoluteX;
    Instructions[0xBD].Mnemonic = LDA;
    Instructions[0xBD].AddressingMode = AbsoluteX;
    Instructions[0xDD].Mnemonic = CMP;
    Instructions[0xDD].AddressingMode = AbsoluteX;
    Instructions[0xFD].Mnemonic = SBC;
    Instructions[0xFD].AddressingMode = AbsoluteX;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +1e  ASL     ROL     LSR     ROR     SHX**y) LDX  y) DEC     INC     Absolute,x
    Instructions[0x1E].Mnemonic = ASL;
    Instructions[0x1E].AddressingMode = AbsoluteX;
    Instructions[0x3E].Mnemonic = ROL;
    Instructions[0x3E].AddressingMode = AbsoluteX;
    Instructions[0x5E].Mnemonic = LSR;
    Instructions[0x5E].AddressingMode = AbsoluteX;
    Instructions[0x7E].Mnemonic = ROR;
    Instructions[0x7E].AddressingMode = AbsoluteX;
    Instructions[0x9E].Mnemonic = SHX;
    Instructions[0x9E].AddressingMode = AbsoluteY;
    Instructions[0x9E].Unofficial = 1;
    Instructions[0xBE].Mnemonic = LDX;
    Instructions[0xBE].AddressingMode = AbsoluteY;
    Instructions[0xDE].Mnemonic = DEC;
    Instructions[0xDE].AddressingMode = AbsoluteX;
    Instructions[0xFE].Mnemonic = INC;
    Instructions[0xFE].AddressingMode = AbsoluteX;
    // set  00      20      40      60      80      a0      c0      e0      mode
    // +1f  SLO*    RLA*    SRE*    RRA*    SHA**y) LAX* y) DCP*    ISB*    Absolute,x
    Instructions[0x1F].Mnemonic = SLO;
    Instructions[0x1F].AddressingMode = AbsoluteX;
    Instructions[0x1F].Unofficial = 1;
    Instructions[0x3F].Mnemonic = RLA;
    Instructions[0x3F].AddressingMode = AbsoluteX;
    Instructions[0x3F].Unofficial = 1;
    Instructions[0x5F].Mnemonic = SRE;
    Instructions[0x5F].AddressingMode = AbsoluteX;
    Instructions[0x5F].Unofficial = 1;
    Instructions[0x7F].Mnemonic = RRA;
    Instructions[0x7F].AddressingMode = AbsoluteX;
    Instructions[0x7F].Unofficial = 1;
    Instructions[0x9F].Mnemonic = SHA;
    Instructions[0x9F].AddressingMode = AbsoluteY;
    Instructions[0x9F].Unofficial = 1;
    Instructions[0xBF].Mnemonic = LAX;
    Instructions[0xBF].AddressingMode = AbsoluteY;
    Instructions[0xBF].Unofficial = 1;
    Instructions[0xDF].Mnemonic = DCP;
    Instructions[0xDF].AddressingMode = AbsoluteX;
    Instructions[0xDF].Unofficial = 1;
    Instructions[0xFF].Mnemonic = ISB;
    Instructions[0xFF].AddressingMode = AbsoluteX;
    Instructions[0xFF].Unofficial = 1;
}

internal i32 AddressingModeToSize(addressing_mode AddressingMode) {
    switch (AddressingMode) {
        case Implicit   : return 1;
        case Immediate  : return 2;
        case Accumulator: return 1;
        case Relative   : return 2;
        case ZeroPage   : return 2;
        case ZeroPageX  : return 2;
        case ZeroPageY  : return 2;
        case Absolute   : return 3;
        case AbsoluteX  : return 3;
        case AbsoluteY  : return 3;
        case Indirect   : return 3;
        case IndirectX  : return 2;
        case IndirectY  : return 2;
        default         : return 0;
    }
}

internal void
FormatDisassembledInstruction(u16 Address,
                              u8 InstructionOpCode,
                              bus* Bus,
                              instruction_info* InstructionsDict,
                              u8* CharBuffer) {
    u8* MnemonicString = MnemonicToString(InstructionsDict[InstructionOpCode].Mnemonic);

    switch (InstructionsDict[InstructionOpCode].AddressingMode)
    {
        case Implicit: {
            sprintf(
                CharBuffer,
                "%04X: (%02X)         %s\0",
                Address,
                InstructionOpCode,
                MnemonicString);
        } break;
        case Accumulator: {
            sprintf(
                CharBuffer,
                "%04X: (%02X)         %s A\0",
                Address,
                InstructionOpCode,
                MnemonicString);
        } break;
        case Immediate: {
            u8 ArgumentValue = MemoryRead(Bus, Address + 1);
            sprintf(
                CharBuffer,
                "%04X: (%02X, %02X)     %s #$%02X\0",
                Address,
                InstructionOpCode, ArgumentValue,
                MnemonicString, ArgumentValue);
        } break;
        case ZeroPage: {
            u8 ArgumentValue = MemoryRead(Bus, Address + 1);
            sprintf(
                CharBuffer,
                "%04X: (%02X, %02X)     %s $%02X\0",
                Address,
                InstructionOpCode, ArgumentValue,
                MnemonicString, ArgumentValue);
        } break;
        case ZeroPageX: {
            u8 ArgumentValue = MemoryRead(Bus, Address + 1);
            sprintf(
                CharBuffer,
                "%04X: (%02X, %02X)     %s $%02X,X\0",
                Address,
                InstructionOpCode, ArgumentValue,
                MnemonicString, ArgumentValue);
        } break;
        case ZeroPageY: {
            u8 ArgumentValue = MemoryRead(Bus, Address + 1);
            sprintf(
                CharBuffer,
                "%04X: (%02X, %02X)     %s $%02X,Y\0",
                Address,
                InstructionOpCode, ArgumentValue,
                MnemonicString, ArgumentValue);
        } break;
        case Relative: {
            u8 ArgumentValue = MemoryRead(Bus, Address + 1);
            sprintf(
                CharBuffer,
                "%04X: (%02X, %02X)     %s *%d\0",
                Address,
                InstructionOpCode, ArgumentValue,
                MnemonicString, (i8)ArgumentValue);
        } break;
        case Absolute: {
            u8 ArgumentValue1 = MemoryRead(Bus, Address + 1);
            u8 ArgumentValue2 = MemoryRead(Bus, Address + 2);
            sprintf(
                CharBuffer,
                "%04X: (%02X, %02X, %02X) %s $%02X%02X\0",
                Address,
                InstructionOpCode, ArgumentValue1, ArgumentValue2,
                MnemonicString, ArgumentValue2, ArgumentValue1);
        } break;
        case AbsoluteX: {
            u8 ArgumentValue1 = MemoryRead(Bus, Address + 1);
            u8 ArgumentValue2 = MemoryRead(Bus, Address + 2);
            sprintf(
                CharBuffer,
                "%04X: (%02X, %02X, %02X) %s $%02X%02X,X\0",
                Address,
                InstructionOpCode, ArgumentValue1, ArgumentValue2,
                MnemonicString, ArgumentValue2, ArgumentValue1);
        } break;
        case AbsoluteY: {
            u8 ArgumentValue1 = MemoryRead(Bus, Address + 1);
            u8 ArgumentValue2 = MemoryRead(Bus, Address + 2);
            sprintf(
                CharBuffer,
                "%04X: (%02X, %02X, %02X) %s $%02X%02X,Y\0",
                Address,
                InstructionOpCode, ArgumentValue1, ArgumentValue2,
                MnemonicString, ArgumentValue2, ArgumentValue1);
        } break;
        case Indirect: {
            u8 ArgumentValue1 = MemoryRead(Bus, Address + 1);
            u8 ArgumentValue2 = MemoryRead(Bus, Address + 2);
            sprintf(
                CharBuffer,
                "%04X: (%02X, %02X, %02X) %s $(%02X%02X)\0",
                Address,
                InstructionOpCode, ArgumentValue1, ArgumentValue2,
                MnemonicString, ArgumentValue2, ArgumentValue1);
        } break;
        case IndirectX: {
            u8 ArgumentValue = MemoryRead(Bus, Address + 1);
            sprintf(
                CharBuffer,
                "%04X: (%02X, %02X)     %s ($%02X,X)\0",
                Address,
                InstructionOpCode, ArgumentValue,
                MnemonicString, ArgumentValue);
        } break;
        case IndirectY: {
            u8 ArgumentValue = MemoryRead(Bus, Address + 1);
            sprintf(
                CharBuffer,
                "%04X: (%02X, %02X)     %s ($%02X),Y\0",
                Address,
                InstructionOpCode, ArgumentValue,
                MnemonicString, ArgumentValue);
        } break;
    }
}

internal void PrintDisassembledInstruction(u16 Address,
                                           u8 InstructionOpCode,
                                           bus* Bus,
                                           instruction_info* InstructionsDict,
                                           u8* CharBuffer) {
    FormatDisassembledInstruction(Address,
                                  InstructionOpCode,
                                  Bus,
                                  InstructionsDict,
                                  CharBuffer);
    PlatformPrint(CharBuffer);
}

internal void
Dissasemble(bus* Bus, instruction_info* InstructionsDict, u8** DisassemblyDict, u8* StringData) {
    u32 Address = 0x0000;

    do {
        u8 InstructionOpCode = MemoryRead(Bus, Address);
        // PrintDisassembledInstruction(
        //     Address, InstructionOpCode, Bus, InstructionsDict);

        i32 InstructionSize = AddressingModeToSize(
            InstructionsDict[InstructionOpCode].AddressingMode);
        Address += InstructionSize;
    } while (Address <= 0xFFFF);
}

#endif
