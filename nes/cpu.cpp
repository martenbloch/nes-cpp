#include "include/cpu.h"
#include "include/instructions.h"
#include "include/utils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

StatusRegister::StatusRegister()
: n{0}
, v{0}
, unused{0}
, b{0}
, d{0}
, i{0}
, z{0}
, c{0}
{

}

uint8_t StatusRegister::toByte()
{
    return (n << 7) + (v << 6) + (unused << 5) + (b << 4) + (d << 3) + (i << 2) + (z << 1) + c;
}

void StatusRegister::fromByte(uint8_t data)
{
    c = data & 0x1;
    z = (data >> 1) & 0x1;
    i = (data >> 2) & 0x1;
    d = (data >> 3) & 0x1;
    b = (data >> 4) & 0x1;
    unused = (data >> 5) & 0x1;
    v = (data >> 6) & 0x1;
    n = (data >> 7) & 0x1;   
}

Cpu::Cpu(Bus& bus, Controller& c, Ppu& p)
: m_bus{bus}
, m_clockTicks{7}
, m_execBitIns{false}
, m_logMsg{""}
, m_cpuMsg{""}
, m_cyclesLeftToPerformCurrentInstruction{0}
, m_newInstruction{false}
, m_enablePrint{false}
, m_clk{0}
, m_c{c}
, m_ppu{p}
{
    m_cpuState.sp = 0xfd;

    m_instructions[0x61] = std::make_unique<Adc>(*this, std::make_unique<AddressModeIndirectX>(*this));
    m_instructions[0x65] = std::make_unique<Adc>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0x69] = std::make_unique<Adc>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0x6D] = std::make_unique<Adc>(*this, std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0x71] = std::make_unique<Adc>(*this, std::make_unique<AddressModeIndirectY>(*this));
    m_instructions[0x75] = std::make_unique<Adc>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0x79] = std::make_unique<Adc>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 4));
    m_instructions[0x7D] = std::make_unique<Adc>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4));

    m_instructions[0x21] = std::make_unique<And>(*this, std::make_unique<AddressModeIndirectX>(*this));
    m_instructions[0x25] = std::make_unique<And>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0x29] = std::make_unique<And>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0x2D] = std::make_unique<And>(*this, std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0x31] = std::make_unique<And>(*this, std::make_unique<AddressModeIndirectY>(*this));
    m_instructions[0x35] = std::make_unique<And>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0x39] = std::make_unique<And>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 4));
    m_instructions[0x3D] = std::make_unique<And>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4));

    m_instructions[0x06] = std::make_unique<Asl>(*this, std::make_unique<AddressModeZeroPage>(*this, 5));
    m_instructions[0x0A] = std::make_unique<Asl>(*this, std::make_unique<AddressModeAccumulator>(*this));
    m_instructions[0x0E] = std::make_unique<Asl>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));
    m_instructions[0x16] = std::make_unique<Asl>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 6));
    m_instructions[0x1E] = std::make_unique<Asl>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 7));

    m_instructions[0x90] = std::make_unique<Bcc>(*this, std::make_unique<AddressModeRelative>(*this));

    m_instructions[0xB0] = std::make_unique<Bcs>(*this, std::make_unique<AddressModeRelative>(*this));

    m_instructions[0xF0] = std::make_unique<Beq>(*this, std::make_unique<AddressModeRelative>(*this));

    m_instructions[0x24] = std::make_unique<Bit>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0x2C] = std::make_unique<Bit>(*this, std::make_unique<AddressModeAbsolute>(*this));

    m_instructions[0x30] = std::make_unique<Bmi>(*this, std::make_unique<AddressModeRelative>(*this));

    m_instructions[0xD0] = std::make_unique<Bne>(*this, std::make_unique<AddressModeRelative>(*this));

    m_instructions[0x10] = std::make_unique<Bpl>(*this, std::make_unique<AddressModeRelative>(*this));

    m_instructions[0x00] = std::make_unique<Brk>(*this, std::make_unique<AddressModeImplied>(7));

    m_instructions[0x50] = std::make_unique<Bvc>(*this, std::make_unique<AddressModeRelative>(*this));

    m_instructions[0x70] = std::make_unique<Bvs>(*this, std::make_unique<AddressModeRelative>(*this));

    m_instructions[0x18] = std::make_unique<Clc>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0xD8] = std::make_unique<Cld>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0x58] = std::make_unique<Cli>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0xB8] = std::make_unique<Clv>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0xC1] = std::make_unique<Cmp>(*this, std::make_unique<AddressModeIndirectX>(*this));
    m_instructions[0xC5] = std::make_unique<Cmp>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0xC9] = std::make_unique<Cmp>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0xCD] = std::make_unique<Cmp>(*this, std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0xD1] = std::make_unique<Cmp>(*this, std::make_unique<AddressModeIndirectY>(*this));
    m_instructions[0xD5] = std::make_unique<Cmp>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0xD9] = std::make_unique<Cmp>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 4));
    m_instructions[0xDD] = std::make_unique<Cmp>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4));

    m_instructions[0xE0] = std::make_unique<Cpx>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0xE4] = std::make_unique<Cpx>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0xEC] = std::make_unique<Cpx>(*this, std::make_unique<AddressModeAbsolute>(*this));

    m_instructions[0xC0] = std::make_unique<Cpy>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0xC4] = std::make_unique<Cpy>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0xC4] = std::make_unique<Cpy>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0xCC] = std::make_unique<Cpy>(*this, std::make_unique<AddressModeAbsolute>(*this));

    m_instructions[0xC3] = std::make_unique<Dcp>(*this, std::make_unique<AddressModeIndirectX>(*this, 8));
    m_instructions[0xC7] = std::make_unique<Dcp>(*this, std::make_unique<AddressModeZeroPage>(*this, 5));
    m_instructions[0xCF] = std::make_unique<Dcp>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));
    m_instructions[0xD3] = std::make_unique<Dcp>(*this, std::make_unique<AddressModeIndirectY>(*this, 8, false));
    m_instructions[0xD7] = std::make_unique<Dcp>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 6));
    m_instructions[0xDB] = std::make_unique<Dcp>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 7, false));
    m_instructions[0xDF] = std::make_unique<Dcp>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 7, false));

    m_instructions[0xC6] = std::make_unique<Dec>(*this, std::make_unique<AddressModeZeroPage>(*this, 5));
    m_instructions[0xCE] = std::make_unique<Dec>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));
    m_instructions[0xD6] = std::make_unique<Dec>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 6));
    m_instructions[0xDE] = std::make_unique<Dec>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 7));

    m_instructions[0xCA] = std::make_unique<Dex>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0x88] = std::make_unique<Dey>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0x41] = std::make_unique<Eor>(*this, std::make_unique<AddressModeIndirectX>(*this));
    m_instructions[0x45] = std::make_unique<Eor>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0x49] = std::make_unique<Eor>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0x4D] = std::make_unique<Eor>(*this, std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0x51] = std::make_unique<Eor>(*this, std::make_unique<AddressModeIndirectY>(*this));
    m_instructions[0x55] = std::make_unique<Eor>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0x59] = std::make_unique<Eor>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 4));
    m_instructions[0x5D] = std::make_unique<Eor>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4));

    m_instructions[0xE6] = std::make_unique<Inc>(*this, std::make_unique<AddressModeZeroPage>(*this, 5));
    m_instructions[0xEE] = std::make_unique<Inc>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));
    m_instructions[0xF6] = std::make_unique<Inc>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 6));
    m_instructions[0xFE] = std::make_unique<Inc>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 7));

    m_instructions[0xE8] = std::make_unique<Inx>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0xC8] = std::make_unique<Iny>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0xE3] = std::make_unique<Isb>(*this, std::make_unique<AddressModeIndirectX>(*this, 8));
    m_instructions[0xE7] = std::make_unique<Isb>(*this, std::make_unique<AddressModeZeroPage>(*this, 5));
    m_instructions[0xEF] = std::make_unique<Isb>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));
    m_instructions[0xF3] = std::make_unique<Isb>(*this, std::make_unique<AddressModeIndirectY>(*this, 8, false));
    m_instructions[0xF7] = std::make_unique<Isb>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 6));
    m_instructions[0xFB] = std::make_unique<Isb>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 7, false));
    m_instructions[0xFF] = std::make_unique<Isb>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 7, false));

    m_instructions[0x4C] = std::make_unique<Jmp>(*this, std::make_unique<AddressModeAbsolute>(*this, 3));
    m_instructions[0x6C] = std::make_unique<Jmp>(*this, std::make_unique<AddressModeIndirect>(*this, 5));

    m_instructions[0x20] = std::make_unique<Jsr>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));

    m_instructions[0xA3] = std::make_unique<Lax>(*this, std::make_unique<AddressModeIndirectX>(*this));
    m_instructions[0xAF] = std::make_unique<Lax>(*this, std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0xB3] = std::make_unique<Lax>(*this, std::make_unique<AddressModeIndirectY>(*this));
    m_instructions[0xB7] = std::make_unique<Lax>(*this, std::make_unique<AddressModeZeroPageYIndexed>(*this));
    m_instructions[0xBF] = std::make_unique<Lax>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 4));
    m_instructions[0xA7] = std::make_unique<Lax>(*this, std::make_unique<AddressModeZeroPage>(*this));

    m_instructions[0xA1] = std::make_unique<Lda>(*this, std::make_unique<AddressModeIndirectX>(*this));
    m_instructions[0xA5] = std::make_unique<Lda>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0xA9] = std::make_unique<Lda>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0xAD] = std::make_unique<Lda>(*this, std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0xB1] = std::make_unique<Lda>(*this, std::make_unique<AddressModeIndirectY>(*this));
    m_instructions[0xB5] = std::make_unique<Lda>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0xB9] = std::make_unique<Lda>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 4));
    m_instructions[0xBD] = std::make_unique<Lda>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4));

    m_instructions[0xA2] = std::make_unique<Ldx>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0xA6] = std::make_unique<Ldx>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0xAE] = std::make_unique<Ldx>(*this, std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0xB6] = std::make_unique<Ldx>(*this, std::make_unique<AddressModeZeroPageYIndexed>(*this));
    m_instructions[0xBE] = std::make_unique<Ldx>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 4));

    m_instructions[0xA0] = std::make_unique<Ldy>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0xA4] = std::make_unique<Ldy>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0xAC] = std::make_unique<Ldy>(*this, std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0xB4] = std::make_unique<Ldy>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0xBC] = std::make_unique<Ldy>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4));

    m_instructions[0x46] = std::make_unique<Lsr>(*this, std::make_unique<AddressModeZeroPage>(*this, 5));
    m_instructions[0x4A] = std::make_unique<Lsr>(*this, std::make_unique<AddressModeAccumulator>(*this));
    m_instructions[0x4E] = std::make_unique<Lsr>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));
    m_instructions[0x56] = std::make_unique<Lsr>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 6));
    m_instructions[0x5E] = std::make_unique<Lsr>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 7));

    m_instructions[0x04] = std::make_unique<Nop>(std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0x0C] = std::make_unique<Nop>(std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0x14] = std::make_unique<Nop>(std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0x1A] = std::make_unique<Nop>(std::make_unique<AddressModeImplied>());
    m_instructions[0x1C] = std::make_unique<Nop>(std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4));
    m_instructions[0x34] = std::make_unique<Nop>(std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0x3A] = std::make_unique<Nop>(std::make_unique<AddressModeImplied>());
    m_instructions[0x3C] = std::make_unique<Nop>(std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4)); // TODO: duplication here
    m_instructions[0x54] = std::make_unique<Nop>(std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0x5A] = std::make_unique<Nop>(std::make_unique<AddressModeImplied>());
    m_instructions[0x7A] = std::make_unique<Nop>(std::make_unique<AddressModeImplied>());
    m_instructions[0x44] = std::make_unique<Nop>(std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0x5C] = std::make_unique<Nop>(std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4));  // TODO: duplication here
    m_instructions[0x64] = std::make_unique<Nop>(std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0x74] = std::make_unique<Nop>(std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0x7C] = std::make_unique<Nop>(std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4));  // TODO: duplication here
    m_instructions[0x80] = std::make_unique<Nop>(std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0xD4] = std::make_unique<Nop>(std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0xDA] = std::make_unique<Nop>(std::make_unique<AddressModeImplied>());
    m_instructions[0xDC] = std::make_unique<Nop>(std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4));  // TODO: duplication here
    m_instructions[0xEA] = std::make_unique<Nop>(std::make_unique<AddressModeImplied>());
    m_instructions[0xF4] = std::make_unique<Nop>(std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0xFA] = std::make_unique<Nop>(std::make_unique<AddressModeImplied>());
    m_instructions[0xFC] = std::make_unique<Nop>(std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4)); // TODO: duplication here

    m_instructions[0x01] = std::make_unique<Ora>(*this, std::make_unique<AddressModeIndirectX>(*this));
    m_instructions[0x05] = std::make_unique<Ora>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0x09] = std::make_unique<Ora>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0x0D] = std::make_unique<Ora>(*this, std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0x11] = std::make_unique<Ora>(*this, std::make_unique<AddressModeIndirectY>(*this));
    m_instructions[0x15] = std::make_unique<Ora>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0x19] = std::make_unique<Ora>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 4));
    m_instructions[0x1D] = std::make_unique<Ora>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4));

    m_instructions[0x48] = std::make_unique<Pha>(*this, std::make_unique<AddressModeImplied>(3));

    m_instructions[0x08] = std::make_unique<Php>(*this, std::make_unique<AddressModeImplied>(3));

    m_instructions[0x68] = std::make_unique<Pla>(*this, std::make_unique<AddressModeImplied>(4));

    m_instructions[0x28] = std::make_unique<Plp>(*this, std::make_unique<AddressModeImplied>(4));

    m_instructions[0x23] = std::make_unique<Rla>(*this, std::make_unique<AddressModeIndirectX>(*this, 8));
    m_instructions[0x27] = std::make_unique<Rla>(*this, std::make_unique<AddressModeZeroPage>(*this, 5));
    m_instructions[0x2F] = std::make_unique<Rla>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));
    m_instructions[0x33] = std::make_unique<Rla>(*this, std::make_unique<AddressModeIndirectY>(*this, 8, false));
    m_instructions[0x37] = std::make_unique<Rla>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 6));
    m_instructions[0x3B] = std::make_unique<Rla>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 7, false));
    m_instructions[0x3F] = std::make_unique<Rla>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 7, false));

    m_instructions[0x2A] = std::make_unique<Rol>(*this, std::make_unique<AddressModeAccumulator>(*this));
    m_instructions[0x26] = std::make_unique<Rol>(*this, std::make_unique<AddressModeZeroPage>(*this, 5));
    m_instructions[0x36] = std::make_unique<Rol>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 6));
    m_instructions[0x2E] = std::make_unique<Rol>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));
    m_instructions[0x3E] = std::make_unique<Rol>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 7));

    m_instructions[0x6A] = std::make_unique<Ror>(*this, std::make_unique<AddressModeAccumulator>(*this));
    m_instructions[0x66] = std::make_unique<Ror>(*this, std::make_unique<AddressModeZeroPage>(*this, 5));
    m_instructions[0x76] = std::make_unique<Ror>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 6));
    m_instructions[0x6E] = std::make_unique<Ror>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));
    m_instructions[0x7E] = std::make_unique<Ror>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 7));

    m_instructions[0x63] = std::make_unique<Rra>(*this, std::make_unique<AddressModeIndirectX>(*this, 8));
    m_instructions[0x67] = std::make_unique<Rra>(*this, std::make_unique<AddressModeZeroPage>(*this, 5));
    m_instructions[0x6F] = std::make_unique<Rra>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));
    m_instructions[0x73] = std::make_unique<Rra>(*this, std::make_unique<AddressModeIndirectY>(*this, 8, false));
    m_instructions[0x77] = std::make_unique<Rra>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 6));
    m_instructions[0x7B] = std::make_unique<Rra>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 7, false));
    m_instructions[0x7F] = std::make_unique<Rra>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 7, false));

    m_instructions[0x40] = std::make_unique<Rti>(*this, std::make_unique<AddressModeImplied>(6));

    m_instructions[0x60] = std::make_unique<Rts>(*this, std::make_unique<AddressModeImplied>(6));

    m_instructions[0xCB] = std::make_unique<Sax>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0x83] = std::make_unique<Sax>(*this, std::make_unique<AddressModeIndirectX>(*this));
    m_instructions[0x87] = std::make_unique<Sax>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0x8F] = std::make_unique<Sax>(*this, std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0x97] = std::make_unique<Sax>(*this, std::make_unique<AddressModeZeroPageYIndexed>(*this));

    m_instructions[0xE9] = std::make_unique<Sbc>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0xE5] = std::make_unique<Sbc>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0xEB] = std::make_unique<Sbc>(*this, std::make_unique<AddressModeImmediate>(*this));
    m_instructions[0xF5] = std::make_unique<Sbc>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0xED] = std::make_unique<Sbc>(*this, std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0xFD] = std::make_unique<Sbc>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 4));
    m_instructions[0xF9] = std::make_unique<Sbc>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 4));
    m_instructions[0xE1] = std::make_unique<Sbc>(*this, std::make_unique<AddressModeIndirectX>(*this));
    m_instructions[0xF1] = std::make_unique<Sbc>(*this, std::make_unique<AddressModeIndirectY>(*this));

    m_instructions[0x38] = std::make_unique<Sec>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0xF8] = std::make_unique<Sed>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0x78] = std::make_unique<Sei>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0x03] = std::make_unique<Slo>(*this, std::make_unique<AddressModeIndirectX>(*this, 8));
    m_instructions[0x07] = std::make_unique<Slo>(*this, std::make_unique<AddressModeZeroPage>(*this, 5));
    m_instructions[0x0F] = std::make_unique<Slo>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));
    m_instructions[0x13] = std::make_unique<Slo>(*this, std::make_unique<AddressModeIndirectY>(*this, 8, false));
    m_instructions[0x17] = std::make_unique<Slo>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 6));
    m_instructions[0x1B] = std::make_unique<Slo>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 7, false));
    m_instructions[0x1F] = std::make_unique<Slo>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 7, false));

    m_instructions[0x43] = std::make_unique<Sre>(*this, std::make_unique<AddressModeIndirectX>(*this, 8));
    m_instructions[0x47] = std::make_unique<Sre>(*this, std::make_unique<AddressModeZeroPage>(*this, 5));
    m_instructions[0x4F] = std::make_unique<Sre>(*this, std::make_unique<AddressModeAbsolute>(*this, 6));
    m_instructions[0x53] = std::make_unique<Sre>(*this, std::make_unique<AddressModeIndirectY>(*this, 8, false));
    m_instructions[0x57] = std::make_unique<Sre>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 6));
    m_instructions[0x5B] = std::make_unique<Sre>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 7, false));
    m_instructions[0x5F] = std::make_unique<Sre>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 7, false));

    m_instructions[0x81] = std::make_unique<Sta>(*this, std::make_unique<AddressModeIndirectX>(*this));
    m_instructions[0x85] = std::make_unique<Sta>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0x8D] = std::make_unique<Sta>(*this, std::make_unique<AddressModeAbsolute>(*this));
    m_instructions[0x91] = std::make_unique<Sta>(*this, std::make_unique<AddressModeIndirectY>(*this, 6, false));
    m_instructions[0x95] = std::make_unique<Sta>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this, 4));
    m_instructions[0x99] = std::make_unique<Sta>(*this, std::make_unique<AddressModeAbsoluteYIndexed>(*this, 5, false));
    m_instructions[0x9D] = std::make_unique<Sta>(*this, std::make_unique<AddressModeAbsoluteXIndexed>(*this, 5, false));

    m_instructions[0x86] = std::make_unique<Stx>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0x96] = std::make_unique<Stx>(*this, std::make_unique<AddressModeZeroPageYIndexed>(*this));
    m_instructions[0x8E] = std::make_unique<Stx>(*this, std::make_unique<AddressModeAbsolute>(*this));

    m_instructions[0x84] = std::make_unique<Sty>(*this, std::make_unique<AddressModeZeroPage>(*this));
    m_instructions[0x94] = std::make_unique<Sty>(*this, std::make_unique<AddressModeZeroPageXIndexed>(*this));
    m_instructions[0x8C] = std::make_unique<Sty>(*this, std::make_unique<AddressModeAbsolute>(*this));

    m_instructions[0xAA] = std::make_unique<Tax>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0xA8] = std::make_unique<Tay>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0xBA] = std::make_unique<Tsx>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0x8A] = std::make_unique<Txa>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0x9A] = std::make_unique<Txs>(*this, std::make_unique<AddressModeImplied>());

    m_instructions[0x98] = std::make_unique<Tya>(*this, std::make_unique<AddressModeImplied>());
}

CpuState& Cpu::getState()
{
    return m_cpuState;
}

uint8_t Cpu::read(uint16_t address)
{
    return m_bus.read(address);
}

void Cpu::write(uint16_t address, uint8_t data)
{
    m_bus.write(address, data);
}

void Cpu::push(uint8_t value)
{
    //std::cout << "PUSH " << std::hex << int(value) << "  clk:" << std::dec << int(m_clockTicks) << "\r\n";
    write(0x0100 | m_cpuState.sp, value);
    m_cpuState.sp -= 1;
    if(m_cpuState.sp == -1)
        m_cpuState.sp = 0xFF;
}

uint8_t Cpu::pop()
{
    m_cpuState.sp += 1;
    auto value = read(0x0100 | m_cpuState.sp);
    //std::cout << "POP " << std::hex << int(value) << "  clk:" << std::dec << int(m_clockTicks) << "\r\n";
    return value;
}

std::string Cpu::instAsBytes(uint16_t pc, uint8_t instructionSize)
{
    std::string s;
    for(std::size_t i =0; i < instructionSize; ++i)
    {
        s += "$" + toHex(read(pc + i), 2) + " ";
    }
    return s + std::string(12 - s.size(), ' ');
}

void Cpu::clock()
{
    if(m_cyclesLeftToPerformCurrentInstruction == 1 && m_execBitIns == true)
    {
        m_execBitIns = false;
        m_instructions[0x2c]->execute();
        if(m_enablePrint)
        {
            std::string logMsg = m_logMsg;
            logMsg += m_instructions[0x2c]->str();
            logMsg += m_cpuMsg;

            std::ofstream fh("log.txt", std::ios::app);
            fh << logMsg;
            fh.close();
            m_logMsg = "";
            m_cpuMsg = "";
        }
    }

    if(m_cyclesLeftToPerformCurrentInstruction == 0)
    {
        m_newInstruction = true;
        std::string logMsg = "";
        std::string cpuStateBefore = "";

        auto instruction = read(m_cpuState.pc);

        if(m_instructions[instruction] == nullptr)
            throw std::runtime_error("Unknown instruction :" + toHexString(instruction, 2));

        if(m_enablePrint)
        {
            logMsg += toHex(m_cpuState.pc, 4) + " ";
            logMsg += instAsBytes(m_cpuState.pc, m_instructions[instruction]->size());
            cpuStateBefore = m_cpuState.str();
            //ppu = self.bus.get_device_by_address(0x2000)

            std::stringstream c;
            c << std::setw(3) << std::left << m_ppu.getCycle();
            std::stringstream s;
            s << std::setw(3) << std::left << m_ppu.getScanline();

            cpuStateBefore += " CYC:" + c.str() + " SL:" + s.str();
            cpuStateBefore += " CPU Cycle:" + std::to_string(m_clockTicks);
            cpuStateBefore += "\r\n";
        }

        m_cpuState.pc += 1;

        if(instruction == 0x2c)
        {
            m_cyclesLeftToPerformCurrentInstruction = 4;
            m_execBitIns = true;
        }
        else
        { 
            /*
            if(m_clockTicks == 10541237 || m_clockTicks == 10571763 || m_clockTicks == 10631413 || m_clockTicks == 10600395 || m_clockTicks == 10659955)
            {
                m_c.m_startPressed = true;
            } 
            if(m_clockTicks == 10691011 || m_clockTicks == 10719517 || m_clockTicks == 10750567 || m_clockTicks == 12565866)
            {
                m_c.m_startPressed = true;
            }      
            if(m_clockTicks == 12596882 || m_clockTicks == 12625462 || m_clockTicks == 12656450 || m_clockTicks == 12685022)
            {
                m_c.m_startPressed = true;
            } 
            if(m_clockTicks == 12716018 || m_clockTicks == 12744582 || m_clockTicks == 12775718 || m_clockTicks == 12804144)
            {
                m_c.m_startPressed = true;
            } 
            if(m_clockTicks == 12835134 || m_clockTicks == 12863706 || m_clockTicks == 12894698 || m_clockTicks == 12804144)
            {
                m_c.m_startPressed = true;
            }
            */ 

            m_cyclesLeftToPerformCurrentInstruction = m_instructions[instruction]->execute();
        }

        if(m_enablePrint && m_execBitIns == false)
        {
            logMsg += m_instructions[instruction]->str();
            logMsg += cpuStateBefore;
            std::ofstream fh("log.txt", std::ios::app);
            fh << logMsg;
            fh.close();
        }
        if(m_enablePrint && m_execBitIns == true)
        {
            m_logMsg = logMsg;
            m_cpuMsg = cpuStateBefore;
        }

        m_clockTicks += m_cyclesLeftToPerformCurrentInstruction;
    }
    else
        m_newInstruction = false;

    m_cyclesLeftToPerformCurrentInstruction -= 1;
    m_clk += 1;
}

void Cpu::reset()
{
    m_clockTicks = 8;
    m_clk = 8;
    m_cpuState.pc = 0xFFFC;
    
    uint8_t ll = read(m_cpuState.pc + 0);
    uint8_t hh = read(m_cpuState.pc + 1);

    m_cpuState.pc = (hh << 8) | ll;
    std::cout << "Reset set PC:" << std::hex << m_cpuState.pc << std::endl;

    m_cpuState.a = 0x00;
    m_cpuState.x = 0x08;
    m_cpuState.y = 0x00;
    m_cpuState.sp = 0xfa;

    m_cpuState.sr.fromByte(0x07);
}

void Cpu::nmi()
{
    push((m_cpuState.pc & 0xFF00) >> 8);
    push(m_cpuState.pc & 0xFF);


    auto copy = m_cpuState.sr;
    m_cpuState.sr.unused = 1;
    m_cpuState.sr.b = 0;

    push(m_cpuState.sr.toByte());
    m_cpuState.sr = copy;

    m_cpuState.pc = 0xFFFA;

    uint8_t ll = read(m_cpuState.pc + 0);
    uint8_t hh = read(m_cpuState.pc + 1);

    m_cpuState.pc = (hh << 8) | ll;
    m_cpuState.sr.i = 1;
    m_clockTicks += 7;
    m_clk += 7;
}

void Cpu::irq()
{
    push((m_cpuState.pc & 0xFF00) >> 8);
    push(m_cpuState.pc & 0xFF);
    push(m_cpuState.sr.toByte());

    m_cpuState.pc = 0xFFFE;

    uint8_t ll = read(m_cpuState.pc + 0);
    uint8_t hh = read(m_cpuState.pc + 1);

    m_cpuState.pc = (hh << 8) | ll;
    m_cpuState.sr.i = 1;
    m_clockTicks += 7;
    m_clk += 7;
}

uint64_t Cpu::getClockTicks()
{
    return m_clockTicks;
}

void Cpu::increaseClockTicks(uint16_t value)
{
    m_clockTicks += value;
}

uint8_t Cpu::cyclesLeft()
{
    return m_cyclesLeftToPerformCurrentInstruction;
}

bool Cpu::isPrintEnbled()
{
    return m_enablePrint;
}


std::string CpuState::str()
{
    return "A:"  + toHex(a, 2) + 
          " X:"  + toHex(x, 2) + 
          " Y:"  + toHex(y,2) + 
          " P:"  + toHex(sr.toByte(), 2) +
          " SP:" + toHex(sp, 2);
}