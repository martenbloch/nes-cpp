#pragma once

#include "mapper.h"

#include <vector>
#include <array>

class Mapper004 : public Mapper
{
    public:
        Mapper004(std::vector<uint8_t> prg, uint8_t numPrgBlocks, std::vector<uint8_t> chr, uint8_t numChr);

        uint16_t cpuRead(uint16_t address) override;
        void cpuWrite(uint16_t address, uint8_t data) override;
        uint16_t ppuRead(uint16_t address) override;
        void ppuWrite(uint16_t address, uint8_t data) override;
        void scanline() override;
        bool isIrqActive();
        void clearIrq();

    private:
        std::vector<uint8_t> m_prg;
        std::vector<uint8_t> m_chr;
        uint8_t m_numBlocks;
        uint8_t m_numChrBanks;
        std::vector<uint8_t> m_ram;
        
        uint8_t m_prgMode;
        uint8_t m_chrMode;
        uint8_t m_bankRegisterSelect;
        uint8_t m_irqCounter;
        uint8_t m_irqReloadValue;
        bool m_irqEnabled;
        bool m_irqActive;

        std::array<uint8_t, 8> m_r;
};