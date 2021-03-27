#pragma once

#include "mapper.h"

#include <vector>

class Mapper001 : public Mapper
{
    public:
        Mapper001(std::vector<uint8_t> prg, uint8_t numPrgBlocks, std::vector<uint8_t> chr, uint8_t numChr);
        
        uint16_t cpuRead(uint16_t address) override;
        void cpuWrite(uint16_t address, uint8_t data) override;
        uint16_t ppuRead(uint16_t address) override;
        void ppuWrite(uint16_t address, uint8_t data) override;
    
    private:
        std::vector<uint8_t> m_prg;
        uint8_t m_numBlocks;
        std::vector<uint8_t> m_chr;
        uint8_t m_numChr;
        uint8_t m_sr;
        uint8_t m_writeCounter;
        uint8_t m_ctrlData;
        uint8_t m_bankMode;
        uint8_t m_chrBankMode;
        uint8_t m_selectedPrgBank;
        uint8_t m_maxNumBank16k;
        uint8_t m_numBank16k;
        uint8_t m_numBank32k;

        uint8_t m_numChrBank0;
        uint8_t m_numChrBank1;
        uint8_t m_numChrBank8k;
        uint8_t m_chrSwitchMode;
        uint8_t m_numChrBanks;

        std::vector<uint8_t> m_ram;

        void internalWrite(uint16_t address, uint8_t data);
};