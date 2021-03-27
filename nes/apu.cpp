#include "include/apu.h"

#include <iostream>

namespace
{
    constexpr auto FOUR_STEP_DIVIDER = 89490;
    constexpr auto FIVE_STEP_DIVIDER = 111860;
}

Dmc::Dmc()
: enabled{false}
, irqEnabled{false}
, loop{false}
, frequency{0x00}
, loadCounter{0x00}
, sampleAddress{0x00}
, sampleLength{0x0000}

, addressOffset{0x0000}
, currentSample{0x0000}
, numPlayedSmaples{0x0000}
, cnt{0x00}
, rates{428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54}
, rateCnt{0x0000}
, irqRaised{false}
{

}

void Dmc::clock()
{
    if(!enabled)
        return;

    if(cnt % 3 == 0)
    {
        rateCnt += 1;
        if(rateCnt == rates[frequency])
        {
            rateCnt = 0;
            std::cout << "playedSamples:" << int(numPlayedSmaples) << "\r\n";
            numPlayedSmaples += 1;
            if(numPlayedSmaples == sampleLength && !loop && irqEnabled)
            {
                irqRaised = true;
                numPlayedSmaples = 0;
            }
        }

    }
    cnt += 1;
    if(cnt == 4)
        cnt = 0;
}

Apu::Apu()
: m_mode{Mode::FOUR_STEP}
, m_divider{FOUR_STEP_DIVIDER}
, m_counter{m_divider}
, m_step{0}
, m_irqEnabled{false}
, m_irqRaised{false}
{

}

uint8_t Apu::cpuRead(uint16_t address)
{
    return 0x40;
}

void Apu::cpuWrite(uint16_t address, uint8_t data)
{
    return;
    if(address == 0x4017)
    {
        if(((data & 0x80) > 0))
        {
            m_mode = Mode::FIVE_STEP;
            m_divider = FIVE_STEP_DIVIDER;
            m_counter = FIVE_STEP_DIVIDER;
        }
        else
        {
            m_mode = Mode::FOUR_STEP;
            m_divider = FOUR_STEP_DIVIDER;
            m_counter = FOUR_STEP_DIVIDER;            
        }

        m_irqEnabled = (data & 0x40) == 0;
        //std::cout << "0x4017 data:" << std::hex << int(data) << "  irq enabled:" << (m_irqEnabled ? "YES" : "NO") << "\r\n"; 
    }
    else if(address == 0x4010)
    {
        m_dmc.irqEnabled = ((data & 0x80) > 0);
        m_dmc.loop = ((data & 0x40) > 0);
        m_dmc.frequency = data & 0xf;
        std::cout << "0x4010 frequency:" << int(m_dmc.frequency) << " irqEnabled:" << (m_dmc.irqEnabled ? "YES" : "NO") << "   loop:" << (m_dmc.loop ? "YES" : "NO")  << "\r\n";
    }
    else if(address == 0x4011)
    {
        m_dmc.loadCounter = data & 0x7f;
    }
    else if(address == 0x4012)
    {
        m_dmc.sampleAddress = 0xc000 | (data << 6);
        std::cout << "0x4012 Sample addr:" << std::hex << int(m_dmc.sampleAddress) << "\r\n";
    }
    else if(address == 0x4013)
    {
        m_dmc.sampleLength = (data << 4) | 0b0001;
        std::cout << "0x4013 Sample length:" << int(m_dmc.sampleLength) << "\r\n";
    }
    else if(address == 0x4015)
    {
        m_dmc.enabled = (data & 0x10) > 0;
    }

    return;
}

bool Apu::isAddressInRange(uint16_t address) const
{
    return address >= 0x4000 && address <= 0x4017 && address != 0x4014 && address != 0x4016;
}

void Apu::clock()
{
    //m_dmc.clock();

    m_counter--;
    if(m_counter == 0)
    {
        // executed at rate 240Hz / 192Hz
        m_counter = m_divider;

        (m_mode == Mode::FOUR_STEP) ? fourStepClock() : fiveStepClock();
    }
}

void Apu::fourStepClock()
{
    if(m_step == 1)
    {
        // clock len counter
    }
    else if(m_step == 3)
    {
        // clock len counter
        if(m_irqEnabled)
        {
            m_irqRaised = true;
            //std::cout << "APU - raise IRQ\r\n";
        }
    }
    // clock linear counter

    ++m_step;
    if(m_step == 4)
    {
        m_step = 0;
    }
}

void Apu::fiveStepClock()
{

}

bool Apu::irqRaised()
{
    if(m_irqRaised)
    {
        m_irqRaised = false;
        return true;
    }
    return false;
}

bool Apu::dmcIrqRaised()
{
    if(m_dmc.irqRaised)
    {
        std::cout << "dmc IRQ raised\r\n";
        m_dmc.irqRaised = false;
        return true;
    }
    return false;
}