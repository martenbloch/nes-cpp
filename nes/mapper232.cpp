#include "include/mapper232.h"

Mapper232::Mapper232(std::vector<uint8_t> prg, uint8_t numPrgBlocks, std::vector<uint8_t> chr)
: m_prg{std::move(prg)}
, m_numBlocks{numPrgBlocks}
, m_chr{chr}
, m_selectedOuterBank{0x0000}
, m_selectedInnerBank{0x0000}
{

}

uint16_t Mapper232::cpuRead(uint16_t address)
{
    if(address >= 0x8000 && address <= 0xBFFF)
        return m_prg[(m_selectedOuterBank * 0x10000) | (m_selectedInnerBank * 0x4000) | (address & 0x3fff)];
    else if(address >= 0xc000 && address <= 0xffff)
        return m_prg[(m_selectedOuterBank * 0x10000) | (3 * 0x4000) | (address & 0x3fff)];
    return m_prg[address & 0x3fff];
}

void Mapper232::cpuWrite(uint16_t address, uint8_t data)
{
    if(address >= 0x8000 && address <= 0xBFFF)
        m_selectedOuterBank = ((data & 0x18) >> 3) & 0x3;
    if(address >= 0xC000 && address <= 0xffff)
        m_selectedInnerBank = (data & 0xF) & 0x3;
}

uint16_t Mapper232::ppuRead(uint16_t address)
{
    return m_chr[address];
}

void Mapper232::ppuWrite(uint16_t address, uint8_t data)
{
    m_chr[address] = data;
}