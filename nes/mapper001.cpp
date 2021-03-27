#include "include/mapper001.h"

Mapper001::Mapper001(std::vector<uint8_t> prg, uint8_t numPrgBlocks, std::vector<uint8_t> chr, uint8_t numChr)
: m_prg{std::move(prg)}
, m_numBlocks{numPrgBlocks}
, m_chr{chr}
, m_sr{0x10}
, m_writeCounter{5}
, m_ctrlData{0x1C}
, m_bankMode{0}
, m_chrBankMode{0}
, m_selectedPrgBank{0}
, m_maxNumBank16k{(uint8_t)(numPrgBlocks - 1)}
, m_numBank16k{0}
, m_numBank32k{0}

, m_numChrBank0{0}
, m_numChrBank1{0}
, m_numChrBank8k{0}
, m_chrSwitchMode{0}
, m_numChrBanks{numChr}

, m_ram(8192, 0)
{

}

uint16_t Mapper001::cpuRead(uint16_t address)
{
    if(address >= 0x6000 && address <= 0x7FFF)
        return m_ram[address & 0x1FFF];

    auto mode =  (m_ctrlData >> 2) & 0x3;

    if(mode == 2 || mode == 3)
    {
        // 16kB Mode
        if(mode == 3)
        {
            // 0xC000 - 0xFFFF fixed
            if(address >= 0xc000 && address <= 0xffff)
                return m_prg[(m_maxNumBank16k * 0x4000) | (address & 0x3fff)];
            else
                return m_prg[(m_numBank16k * 0x4000) | (address & 0x3fff)];
        }
        else
        {
            if(address >= 0x8000 && address <= 0xbfff)
            {
                return m_prg[(0 * 0x4000) | (address & 0x3fff)];
            }
            else
                return m_prg[(m_numBank16k * 0x4000) | (address & 0x3fff)];
        }
    }
    else
    {
        // 32kB mode
        return m_prg[(m_numBank32k * 0x8000) | (address & 0x7fff)];
    }
}

void Mapper001::cpuWrite(uint16_t address, uint8_t data)
{
    if(address >= 0x8000 && address <= 0xFFFF)
    {
        if((data & 0x80) > 0)
        {
            // reset
            m_sr = 0b00000;
            m_ctrlData |= 0x0c;
            m_bankMode = (m_ctrlData & 0b1100) >> 2;
            m_writeCounter = 0;
        }
        else
        {
            // read data
            uint8_t bit = (data & 0x01) << 4;
            m_sr >>= 1;
            m_sr |= bit;
            m_writeCounter += 1;
            if(m_writeCounter == 5)
            {
                internalWrite(address, m_sr);
                m_sr = 0b00000;
                m_writeCounter = 0;
            }
        }
    }
}

uint16_t Mapper001::ppuRead(uint16_t address)
{
    if(address < 0x2000)
    {
        if(m_numChrBanks == 0)
            return address;
        else
        {
            if(m_chrSwitchMode == 1)
            {
                // 4k banks
                if(address >= 0x0000 && address <= 0x0fff)
                    return m_chr[(m_numChrBank0 * 0x1000) | (address & 0x0fff)];
                if(address >= 0x1000 && address <= 0x1fff)
                    return m_chr[(m_numChrBank1 * 0x1000) | (address & 0x0fff)];
            }
            else
            {
                // 8k banks
                return m_chr[(m_numChrBank8k * 0x2000) | (address & 0x1fff)];
            }
        }
    }
    return 0x00;
}

void Mapper001::ppuWrite(uint16_t address, uint8_t data)
{

}

void Mapper001::internalWrite(uint16_t address, uint8_t data)
{
    if(address >= 0x8000 && address <= 0x9FFF)
    {
        m_chrSwitchMode = ((data & 0b10000) >> 4);
        m_bankMode = (data & 0b01100) >> 2;
        m_ctrlData = (data & 0x1f);
    }

    if(address >= 0xA000 && address <= 0xBFFF)
    {
        if(m_chrSwitchMode)
        {
            // 4k
            m_numChrBank0 = data & 0b11111;
        }
        else
        {
            // 8k
            m_numChrBank8k = ((data & 0b11110) >> 1);
        }
    }
    if(address >= 0xC000 && address <= 0xDFFF)
    {
        if(m_chrSwitchMode)
        {
            // 4k
            m_numChrBank1 = data & 0b11111;
        }
    }
    if(address >= 0xe000 && address <= 0xffff)
    {
        // bank select
        auto mode =  (m_ctrlData >> 2) & 0x3;
        if(mode == 2 || mode ==3)
        //if(m_ctrlData & 0b1000)
        {
            // 16k mode
            m_numBank16k = data & 0b1111;
        }
        else
        {
            // 32k mode
            m_numBank32k = ((data & 0b1110) >> 1);
        }
    }
}