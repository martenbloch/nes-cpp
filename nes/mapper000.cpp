#include "include/mapper000.h"


Mapper000::Mapper000(std::vector<uint8_t> prg, uint8_t numPrgBlocks, std::vector<uint8_t> chr)
: m_prg{std::move(prg)}
, m_numBlocks{numPrgBlocks}
, m_chr{chr}
{

}

uint16_t Mapper000::cpuRead(uint16_t address)
{
    return (m_numBlocks > 1) ? m_prg[address & 0x7fff] : m_prg[address & 0x3fff];
}

void Mapper000::cpuWrite(uint16_t address, uint8_t data)
{
    return;
}

uint16_t Mapper000::ppuRead(uint16_t address)
{
    return m_chr[address];
}

void Mapper000::ppuWrite(uint16_t address, uint8_t data)
{
    m_chr[address] = data;
}