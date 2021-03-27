#include "include/mapper071.h"

Mapper071::Mapper071(std::vector<uint8_t> prg, uint8_t numPrgBlocks, std::vector<uint8_t> chr)
: m_prg{std::move(prg)}
, m_numBlocks{numPrgBlocks}
, m_chr{chr}
, m_selectedBank{0x0000}
{

}

uint16_t Mapper071::cpuRead(uint16_t address)
{
    if(address >= 0x8000 && address <= 0xBFFF)
        return m_prg[(m_selectedBank * 0x4000) | (address & 0x3fff)];
    else if(address >= 0xc000 && address <= 0xffff)
        return m_prg[((m_numBlocks -1) * 0x4000) | (address & 0x3fff)];
    if(address >= 0x4020 && address <= 0x5FFF)
    {
        return (address >> 8) & 0xff;
    }

    return m_prg[address & 0x3fff];
}

void Mapper071::cpuWrite(uint16_t address, uint8_t data)
{
    if(address >= 0xC000 && address <= 0xffff)
    {
        m_selectedBank = (data & 0xF) & (m_numBlocks - 1);
    }
}

uint16_t Mapper071::ppuRead(uint16_t address)
{
    return m_chr[address];
}

void Mapper071::ppuWrite(uint16_t address, uint8_t data)
{
    m_chr[address] = data;
}