#include "include/nes.h"

#include <fstream>
#include <iostream>

Nes::Nes(const std::string& nesFile, std::function<uint8_t()> btnStateGetter, std::function<void(const uint32_t*)> frameUpdate)
: m_controller{btnStateGetter}
, m_cartridge{nesFile}
, m_ppu{m_cartridge, frameUpdate}
, m_cpu{m_bus, m_controller, m_ppu}
, m_numOfCycles{1}
, m_writeComplete{false}
, m_dmaData{0x00}
, m_dmaOffset{0x00}
, m_dummyDma{true}
{
    m_bus.connect(m_cartridge);
    m_bus.connect(m_ram);
    m_bus.connect(m_ppu);
    m_bus.connect(m_apu);
    m_bus.connect(m_controller);
}

void Nes::start()
{
    uint64_t i = 0;

    while(true)
    {
    //while(i < 8000000)
        m_ppu.clock();
        //m_apu.clock();
        if(m_numOfCycles % 3 == 0)
        {
            if(m_bus.isDmaRequested())
            {
                // before start we have to wait 1/2 idle cycles
                if(m_dummyDma)
                {
                    if(m_cpu.getClockTicks() % 2 == 0)
                        m_dummyDma = false;
                    else
                        m_cpu.increaseClockTicks(1);
                }
                else
                {
                    if(m_cpu.getClockTicks() % 2 == 0)
                    {
                        m_dmaData = m_cpu.read(((m_bus.getHighByte() << 8) | m_dmaOffset));
                    }
                    else
                    {
                        m_ppu.writeOamData(m_dmaOffset, m_dmaData);
                        m_dmaOffset += 1;
                        if(m_dmaOffset > 0xff)
                        {
                            m_bus.clearDmaRequest();
                            m_writeComplete = false;
                            m_dmaOffset = 0x00;
                            m_dummyDma = true;
                            m_cpu.increaseClockTicks(1);
                        }
                    }
                    m_cpu.increaseClockTicks(1);
                }
            }
            else
                m_cpu.clock();
        }

        
        if(m_ppu.isNmiRaised() && (m_cpu.cyclesLeft() != 0) && !m_ppu.m_raiseNmiNextIns)
        {
            m_cpu.nmi();
            if(m_cpu.isPrintEnbled())
            {
                std::ofstream fh("log.txt", std::ios::app);
                fh << "[NMI - Cycle: " << std::to_string(m_cpu.getClockTicks()-1) << "]\r\n";
                fh.close();
            }
            m_ppu.clearNmi();
        }
        if(m_ppu.isNmiRaised() && (m_cpu.cyclesLeft() == 0) && m_ppu.m_raiseNmiNextIns)
        {
            m_ppu.m_raiseNmiNextIns = false;
        }

        if(m_cartridge.isIrqActive())
        {
            m_cartridge.clearIrq();
            m_cpu.irq();
        }
        /*
        if(m_apu.irqRaised())
        {
            m_cpu.irq();
        }
        if(m_apu.dmcIrqRaised())
        {
            m_cpu.irq();
            if(m_cpu.isPrintEnbled())
            {
                std::ofstream fh("log.txt", std::ios::app);
                fh << "[IRQ - Cycle: " << std::to_string(m_cpu.getClockTicks()-1) << "]\r\n";
                fh.close();
            }
        }
        */

        m_numOfCycles += 1;
        i+=1;
    }
}

void Nes::reset()
{
    m_cpu.reset();
    m_ppu.reset();
}