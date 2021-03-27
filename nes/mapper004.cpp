#include "include/mapper004.h"
#include <iostream>

Mapper004::Mapper004(std::vector<uint8_t> prg, uint8_t numPrgBlocks, std::vector<uint8_t> chr, uint8_t numChr)
: m_prg{std::move(prg)}
, m_numBlocks{uint8_t(numPrgBlocks*(uint8_t)2)}
, m_chr{chr}
, m_numChrBanks{numChr}
, m_ram(8192, 0)

, m_prgMode{0}
, m_chrMode{0}
, m_bankRegisterSelect{0}
, m_irqCounter{0x00}
, m_irqReloadValue{0x00}
, m_irqEnabled{false}
, m_irqActive{false}
, m_r{}
{
    m_r.fill(0);
    std::cout << "numBlocks:" << int(m_numBlocks) << std::endl;
}

uint16_t Mapper004::cpuRead(uint16_t address)
{
    if(address >= 0x6000 && address <= 0x7FFF)
        return m_ram[address & 0x1FFF];

    if(m_prgMode == 0)
    {
        if(address >= 0x8000 && address <= 0x9fff)
        {
            return m_prg[(m_r[6] * 0x2000) + (address & 0x1fff)];
        } 
        else if(address >= 0xa000 && address <= 0xbfff)
        {
            return m_prg[(m_r[7] * 0x2000) + (address & 0x1fff)];
        } 
        else if(address >= 0xc000 && address <= 0xdfff)
        {
            return m_prg[((m_numBlocks -2) * 0x2000) + (address & 0x1fff)];
        }
        else if(address >= 0xe000 && address <= 0xffff)
        {
            return m_prg[((m_numBlocks -1) * 0x2000) + (address & 0x1fff)];
        }
    }
    else
    {
        if(address >= 0x8000 && address <= 0x9fff)
        {
            return m_prg[((m_numBlocks -2) * 0x2000) + (address & 0x1fff)];
        } 
        else if(address >= 0xa000 && address <= 0xbfff)
        {
            return m_prg[(m_r[7] * 0x2000) + (address & 0x1fff)];
        } 
        else if(address >= 0xc000 && address <= 0xdfff)
        {
            return m_prg[(m_r[6] * 0x2000) + (address & 0x1fff)];
        }
        else if(address >= 0xe000 && address <= 0xffff)
        {
            return m_prg[((m_numBlocks -1) * 0x2000) + (address & 0x1fff)];
        }        
    }

    return 0x00;
}

void Mapper004::cpuWrite(uint16_t address, uint8_t data)
{
    std::cout << std::hex << "addr:0x" << address << "    data:0x" << data << std::endl;
    if(address >= 0x8000 && address <= 0x9ffe && ((address & 0x1) == 0))
    {
        //bank select
        m_bankRegisterSelect = data & 0x7;
        m_prgMode = (data & 0x40) >> 6;
        m_chrMode = (data & 0x80) >> 7;
        std::cout << "PRG mode:" << int(m_prgMode) << std::endl;
    }
    else if(address >= 0x8001 && address <= 0x9fff && ((address & 0x1) == 1))
    {
        if(m_bankRegisterSelect == 6 || m_bankRegisterSelect == 7)
        {
            m_r[m_bankRegisterSelect] = data & 0x3f;
        }
        else if(m_bankRegisterSelect == 0 || m_bankRegisterSelect == 1)
        {
            m_r[m_bankRegisterSelect] = data & 0xfe;
        }
        else
        {
            m_r[m_bankRegisterSelect] = data;
        }
        std::cout << "select register: " << int(m_bankRegisterSelect) << "   val:" << int(data) << std::endl;
        //m_r[m_bankRegisterSelect] &= 0x7;
    }
    else if(address >= 0xc000 && address <= 0xdffe && ((address & 0x1) == 0) )
    {
        m_irqReloadValue = data;
    }
    else if(address >= 0xc001 && address <= 0xdfff && ((address & 0x1) == 0) )
    {
        m_irqCounter = 0;
    }
    else if(address >= 0xe000 && address <= 0xfffe && ((address & 0x1) == 0) )
    {
        m_irqEnabled = false;
        m_irqActive = false;
    }
    else if(address >= 0xe001 && address <= 0xffff && ((address & 0x1) == 1) )
    {
        m_irqEnabled = true;
    }
}

uint16_t Mapper004::ppuRead(uint16_t address)
{
    if(m_chrMode == 0)
    {
        if(address >= 0x0000 && address <= 0x03ff)
        {
            return m_chr[(m_r[0] * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x0400 && address <= 0x07ff)
        {
            return m_chr[((m_r[0]+1) * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x0800 && address <= 0x0bff)
        {
            return m_chr[(m_r[1] * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x0c00 && address <= 0x0fff)
        {
            return m_chr[((m_r[1]+1) * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x1000 && address <= 0x13ff)
        {
            return m_chr[(m_r[2] * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x1400 && address <= 0x17ff)
        {
            return m_chr[(m_r[3] * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x1800 && address <= 0x1bff)
        {
            return m_chr[(m_r[4] * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x1c00 && address <= 0x1fff)
        {
            return m_chr[(m_r[5] * 0x400) + (address & 0x3ff)];
        }
    }
    else
    {
        if(address >= 0x0000 && address <= 0x03ff)
        {
            return m_chr[(m_r[2] * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x0400 && address <= 0x07ff)
        {
            return m_chr[(m_r[3] * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x0800 && address <= 0x0bff)
        {
            return m_chr[(m_r[4] * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x0c00 && address <= 0x0fff)
        {
            return m_chr[(m_r[5] * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x1000 && address <= 0x13ff)
        {
            return m_chr[(m_r[0] * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x1400 && address <= 0x17ff)
        {
            return m_chr[((m_r[0]+1) * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x1800 && address <= 0x1bff)
        {
            return m_chr[(m_r[1] * 0x400) + (address & 0x3ff)];
        }
        else if(address >= 0x1c00 && address <= 0x1fff)
        {
            return m_chr[((m_r[1]+1) * 0x400) + (address & 0x3ff)];
        }
    }

    return 0x00;
}

void Mapper004::ppuWrite(uint16_t address, uint8_t data)
{
    return;
}

void Mapper004::scanline()
{
    if(m_irqCounter == 0)
    {
        m_irqCounter = m_irqReloadValue;
    }
    else
    {
        --m_irqCounter;
    }

    if(m_irqCounter == 0 && m_irqEnabled)
    {
        m_irqActive = true;
    }
}

bool Mapper004::isIrqActive()
{
    return m_irqActive;
}
    
void Mapper004::clearIrq()
{
    m_irqActive = false;
}