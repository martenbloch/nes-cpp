#include "include/cartridge.h"
#include "include/utils.h"
#include "include/mapper000.h"
#include "include/mapper001.h"
#include "include/mapper002.h"
#include "include/mapper004.h"
#include "include/mapper071.h"
#include "include/mapper232.h"

#include <iostream>

Cartridge::Cartridge(const std::string& nesFile)
{
    auto data = getFileConent(nesFile);
    m_nesFileHeader = getNesFileHeader(data);

    auto prgStart = 16;
    auto chrStart = 16 + 16384 * m_nesFileHeader.numPrgBlocks;
    if(m_nesFileHeader.trainer)
    {
        prgStart += 512;
        chrStart += 512;
    }

    std::vector<uint8_t> prg{data.begin() + prgStart, data.begin() + prgStart + (16384 * m_nesFileHeader.numPrgBlocks)};
    std::vector<uint8_t> chr;
    if(m_nesFileHeader.numChrBlocks == 0)
    {
        chr = std::vector<uint8_t>(8192, 0);
    }
    else
    {
        chr = std::vector<uint8_t>(data.begin() + chrStart, data.begin() + chrStart + (8192 * m_nesFileHeader.numChrBlocks));
    }

    m_mapper = createMapper(m_nesFileHeader, std::move(prg), std::move(chr));
}

uint8_t Cartridge::cpuRead(uint16_t address)
{
    return m_mapper->cpuRead(address);
}

void Cartridge::cpuWrite(uint16_t address, uint8_t data)
{
    m_mapper->cpuWrite(address, data);
}

bool Cartridge::isAddressInRange(uint16_t address) const
{
    return address >= 0x4020 && address <= 0xFFFF;
}

uint8_t Cartridge::ppuRead(uint16_t address)
{
    return m_mapper->ppuRead(address);
}

void Cartridge::ppuWrite(uint16_t address, uint8_t data)
{
    m_mapper->ppuWrite(address, data);
}

Cartridge::NesFileHeader Cartridge::getNesFileHeader(const std::vector<uint8_t>& data)
{
    NesFileHeader header;

    header.numPrgBlocks = data[4];
    header.numChrBlocks = data[5];
    header.trainer = data[6] & 0x4;
    header.mirroring = (data[6] & 0x1) ? Cartridge::Mirroring::VERTICAL : Cartridge::Mirroring::HORIZONTAL;
    header.mapperId = ((data[7] >> 4) << 4) | (data[6] >> 4);

    std::cout << std::dec << "numPRG:" << int(header.numPrgBlocks) << std::endl;

    return header;
}

std::unique_ptr<Mapper> Cartridge::createMapper(const NesFileHeader& header, std::vector<uint8_t> prg, std::vector<uint8_t> chr)
{
    std::cout << "mapper:" << int(header.mapperId) << std::endl;
    switch(header.mapperId)
    {
        case 0:
            return std::make_unique<Mapper000>(prg, header.numPrgBlocks, chr);
        case 1:
            return std::make_unique<Mapper001>(prg, header.numPrgBlocks, chr, header.numChrBlocks);
        case 2:
            return std::make_unique<Mapper002>(prg, header.numPrgBlocks, chr);
        case 4:
            return std::make_unique<Mapper004>(prg, header.numPrgBlocks, chr, header.numChrBlocks);
        case 71:
            return std::make_unique<Mapper071>(prg, header.numPrgBlocks, chr);
        case 232:
            return std::make_unique<Mapper232>(prg, header.numPrgBlocks, chr);        
        default:
            throw std::runtime_error("Unknown mapperId:" + std::to_string(header.mapperId));
    }
}

Cartridge::Mirroring Cartridge::getMirroring()
{
    return m_nesFileHeader.mirroring;
}

void Cartridge::scanline()
{
    m_mapper->scanline();
}

bool Cartridge::isIrqActive()
{
    return m_mapper->isIrqActive();
}

void Cartridge::clearIrq()
{
    m_mapper->clearIrq();
}