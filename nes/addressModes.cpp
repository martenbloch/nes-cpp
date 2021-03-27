#include "include/addressModes.h"
#include "include/utils.h"

#include <iostream>

AddressModeAccumulator::AddressModeAccumulator(Cpu& cpu)
: m_cpu{cpu}
{

}

uint32_t AddressModeAccumulator::getAddress()
{
    return 0xA0000;
}

uint8_t AddressModeAccumulator::cycles()
{
    return 2;
}

std::string AddressModeAccumulator::str()
{
    return "A";
}

uint8_t AddressModeAccumulator::size()
{
    return 1;
}

AddressModeAbsolute::AddressModeAbsolute(Cpu& cpu, uint8_t cycles)
: m_cpu{cpu}
, m_absAddress{0x0000}
, m_cycles{cycles}
, m_ll{0x00}
, m_hh{0x00}
{

}

uint32_t AddressModeAbsolute::getAddress()
{
    auto& state = m_cpu.getState();
    m_ll = m_cpu.read(state.pc);
    m_hh = m_cpu.read(state.pc + 1);
    state.pc += 2;
    m_absAddress = (m_hh << 8) | m_ll;
    return m_absAddress;
}

uint8_t AddressModeAbsolute::cycles()
{
    return m_cycles;
}

std::string AddressModeAbsolute::str()
{
    return "$" + toHex(m_absAddress, 4);
}

uint8_t AddressModeAbsolute::size()
{
    return 3;
}


AddressModeAbsoluteXIndexed::AddressModeAbsoluteXIndexed(Cpu& cpu, uint8_t cycles, bool extraCycle)
: m_cpu{cpu}
, m_absAddress{0x0000}
, m_ll{0x00}
, m_hh{0x00}
, m_cycles{cycles}
, m_cyclesO{cycles}
, m_extraCycle{extraCycle}
{

}

uint32_t AddressModeAbsoluteXIndexed::getAddress()
{
    auto& state = m_cpu.getState();
    m_cycles = m_cyclesO;
    m_ll = m_cpu.read(state.pc);
    state.pc += 1;
    m_hh = m_cpu.read(state.pc);
    state.pc += 1;

    uint16_t addr = ((m_hh << 8) | m_ll) + state.x;
    addr = addr & 0xffff;

    if((addr & 0xFF00) != (m_hh << 8) && m_extraCycle)
        m_cycles += 1;
    return addr;
}

uint8_t AddressModeAbsoluteXIndexed::cycles()
{
    return m_cycles;
}

std::string AddressModeAbsoluteXIndexed::str()
{
    auto& state = m_cpu.getState();
    uint16_t baseAddr = (m_hh << 8) | m_ll;
    uint16_t absAddr = baseAddr + state.x;
    return "$" + toHex(baseAddr, 4) + ",X @ $" + toHex(absAddr, 4);
}

uint8_t AddressModeAbsoluteXIndexed::size()
{
    return 3;
}

AddressModeAbsoluteYIndexed::AddressModeAbsoluteYIndexed(Cpu& cpu, uint8_t cycles, bool extraCycle)
: m_cpu{cpu}
, m_ll{0x00}
, m_hh{0x00}
, m_cycles{cycles}
, m_cyclesO{cycles}
, m_extraCycle{extraCycle}
{

}

uint32_t AddressModeAbsoluteYIndexed::getAddress()
{
    auto& state = m_cpu.getState();
    m_cycles = m_cyclesO;
    m_ll = m_cpu.read(state.pc);
    state.pc += 1;
    m_hh = m_cpu.read(state.pc);
    state.pc += 1;

    uint16_t addr = ((m_hh << 8) | m_ll) + state.y;
    addr = addr & 0xffff;

    if((addr & 0xFF00) != (m_hh << 8) && m_extraCycle)
        m_cycles += 1;
    return addr;
}

uint8_t AddressModeAbsoluteYIndexed::cycles()
{
    return m_cycles;
}

std::string AddressModeAbsoluteYIndexed::str()
{
    uint16_t baseAddr = (m_hh << 8) | m_ll;
    uint16_t absAddr = baseAddr + m_cpu.getState().y;
    return "$" + toHex(baseAddr, 4) + ",Y @ $" + toHex(absAddr, 4);
}

uint8_t AddressModeAbsoluteYIndexed::size()
{
    return 3;
}

AddressModeImmediate::AddressModeImmediate(Cpu& cpu)
: m_cpu{cpu}
, m_addr{0x0000}
, m_cycles{2}
{

}

uint32_t AddressModeImmediate::getAddress()
{
    auto& state = m_cpu.getState();
    m_addr = state.pc;
    state.pc += 1;
    return m_addr;
}

uint8_t AddressModeImmediate::cycles()
{
    return m_cycles;
}

std::string AddressModeImmediate::str()
{
    return "#$" + toHex(m_cpu.read(m_addr), 2);
}

uint8_t AddressModeImmediate::size()
{
    return 2;
}

AddressModeImplied::AddressModeImplied(uint8_t cycles)
: m_cycles{cycles}
{

}

uint32_t AddressModeImplied::getAddress()
{
    return 0x0000;
}

uint8_t AddressModeImplied::cycles()
{
    return m_cycles;
}

std::string AddressModeImplied::str()
{
    return "";
}

uint8_t AddressModeImplied::size()
{
    return 1;
}

AddressModeIndirect::AddressModeIndirect(Cpu& cpu, uint8_t cycles)
: m_cpu{cpu}
, m_cycles{cycles}
, m_ptr{0x0000}
, m_ll{0x00}
, m_hh{0x00}
, m_addr{0x0000}
{

}

uint32_t AddressModeIndirect::getAddress()
{
    auto& state = m_cpu.getState();
    m_ll = m_cpu.read(state.pc);
    state.pc += 1;
    m_hh = m_cpu.read(state.pc);
    state.pc += 1;

    m_ptr = (m_hh << 8) | m_ll;

    if(m_ll == 0xFF) // emulate bug
        m_addr = (m_cpu.read(m_ptr & 0xFF00) << 8) | m_cpu.read(m_ptr + 0);
    else // Behave normally
        m_addr = (m_cpu.read(m_ptr+1) << 8) | m_cpu.read(m_ptr);

    return m_addr;
}

uint8_t AddressModeIndirect::cycles()
{
    return m_cycles;
}

std::string AddressModeIndirect::str()
{
    return "($" + toHex(m_ptr, 4) + ") @ $" + toHex(m_addr, 4);
}

uint8_t AddressModeIndirect::size()
{
    return 3;
}

AddressModeIndirectX::AddressModeIndirectX(Cpu& cpu, uint8_t cycles)
: m_cpu{cpu}
, m_addr{0x0000}
, m_ll{0x00}
, m_cycles{cycles}
{

}

uint32_t AddressModeIndirectX::getAddress()
{
    auto& state = m_cpu.getState();
    m_ll = m_cpu.read(state.pc);
    state.pc += 1;
    m_addr = (m_cpu.read((m_ll + state.x + 1) & 0xff) << 8) | m_cpu.read((m_ll + state.x) & 0xff);
    return m_addr;
}

uint8_t AddressModeIndirectX::cycles()
{
    return 6;
}

std::string AddressModeIndirectX::str()
{
    return "(" + toHex(m_ll, 2) + ", X)";
}

uint8_t AddressModeIndirectX::size()
{
    return 2;
}

AddressModeIndirectY::AddressModeIndirectY(Cpu& cpu, uint8_t cycles, bool extraCycles)
: m_cpu{cpu}
, m_addr{0x0000}
, m_ll{0x00}
, m_cycles{cycles}
, m_cyclesO{cycles}
, m_extraCycles{extraCycles}
{

}

uint32_t AddressModeIndirectY::getAddress()
{
    auto& state = m_cpu.getState();
    m_cycles = m_cyclesO;
    m_ll = m_cpu.read(state.pc);
    state.pc += 1;

    uint8_t ll = m_cpu.read(m_ll);
    uint8_t hh = m_cpu.read((m_ll + 1) & 0xff);

    m_addr = (hh << 8) | ll;
    m_addr += state.y;
    m_addr = m_addr & 0xffff;

    //std::cout << "addr:" << toHex(m_addr, 4) << "   hh:" << toHex(hh, 2) << std::endl; 

    if(((m_addr & 0xff00) != (hh << 8)) && m_extraCycles)
    {
        m_cycles += 1;
    }

    return m_addr;
}

uint8_t AddressModeIndirectY::cycles()
{
    return m_cycles;
}

std::string AddressModeIndirectY::str()
{
    return "($" + toHex(m_ll, 4) + "),Y @ $" + toHex(m_addr, 4);
}

uint8_t AddressModeIndirectY::size()
{
    return 2;
}

AddressModeRelative::AddressModeRelative(Cpu& cpu)
: m_cpu{cpu}
, m_addr{0x0000}
, m_cycles{2}
, m_data{0x00}
{

}

uint32_t AddressModeRelative::getAddress()
{
    auto& state = m_cpu.getState();
    m_cycles = 2;
    m_data = m_cpu.read(state.pc);
    m_addr = m_data;
    if(m_addr & 0x80)
    {
        // negative number, which we must extend to 16bit value
        m_addr = ~ m_addr & 0xff;
        m_addr = state.pc - m_addr;

        if((m_addr & 0xff00) != (state.pc & 0xff00))
            m_cycles += 2;
        else
            m_cycles += 1;

        state.pc += 1;
        return m_addr;
    }
    else
    {
        state.pc += 1;
        m_addr += state.pc;

        if((m_addr & 0xff00) != (state.pc & 0xff00))
            m_cycles += 2;
        else
            m_cycles += 1;

        return m_addr;
    }
}

uint8_t AddressModeRelative::cycles()
{
    return m_cycles;
}

std::string AddressModeRelative::str()
{
    return "$" + toHex(m_addr, 4);
}

uint8_t AddressModeRelative::size()
{
    return 2;
}

AddressModeZeroPage::AddressModeZeroPage(Cpu& cpu, uint8_t cycles)
: m_cpu{cpu}
, m_address{0x0000}
, m_cycles{cycles}
{

}

uint32_t AddressModeZeroPage::getAddress()
{
    auto& state = m_cpu.getState();
    m_address = m_cpu.read(state.pc);
    state.pc += 1;
    m_address &= 0x00FF;
    return m_address;
}

uint8_t AddressModeZeroPage::cycles()
{
    return m_cycles;
}

std::string AddressModeZeroPage::str()
{
    return "$" + toHex(m_address, 4);
}

uint8_t AddressModeZeroPage::size()
{
    return 2;
}

AddressModeZeroPageXIndexed::AddressModeZeroPageXIndexed(Cpu& cpu, uint8_t cycles)
: m_cpu{cpu}
, m_absAddress{0x0000}
, m_ll{0x00}
, m_cycles{cycles}
, m_addr{0x0000}
{

}

uint32_t AddressModeZeroPageXIndexed::getAddress()
{
    auto& state = m_cpu.getState();
    m_ll = m_cpu.read(state.pc);
    state.pc += 1;
    m_addr = m_ll + state.x;
    m_addr &= 0x00FF;
    return m_addr;
}

uint8_t AddressModeZeroPageXIndexed::cycles()
{
    return m_cycles;
}

std::string AddressModeZeroPageXIndexed::str()
{
    return "$" + toHex(m_ll, 4) + ",X @ $" + toHex(m_addr, 2);
}

uint8_t AddressModeZeroPageXIndexed::size()
{
    return 2;
}

AddressModeZeroPageYIndexed::AddressModeZeroPageYIndexed(Cpu& cpu)
: m_cpu{cpu}
, m_ll{0x00}
, m_cycles{4}
, m_addr{0x0000}
{

}

uint32_t AddressModeZeroPageYIndexed::getAddress()
{
    auto& state = m_cpu.getState();
    m_ll = m_cpu.read(state.pc);
    state.pc += 1;
    m_addr = m_ll + state.y;
    m_addr &= 0x00FF;
    return m_addr;
}

uint8_t AddressModeZeroPageYIndexed::cycles()
{
    return m_cycles;
}

std::string AddressModeZeroPageYIndexed::str()
{
    return "$" + toHex(m_ll, 4) + ",Y @ $" + toHex(m_addr, 2);
}

uint8_t AddressModeZeroPageYIndexed::size()
{
    return 2;
}
