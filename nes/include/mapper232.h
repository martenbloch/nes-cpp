#pragma once

#include "mapper.h"

#include <vector>

class Mapper232 : public Mapper
{
    public:
        Mapper232(std::vector<uint8_t> prg, uint8_t numPrgBlocks, std::vector<uint8_t> chr);

        uint16_t cpuRead(uint16_t address) override;
        void cpuWrite(uint16_t address, uint8_t data) override;
        uint16_t ppuRead(uint16_t address) override;
        void ppuWrite(uint16_t address, uint8_t data) override;

    private:
        std::vector<uint8_t> m_prg;
        uint8_t m_numBlocks;
        std::vector<uint8_t> m_chr;
        uint16_t m_selectedOuterBank;
        uint16_t m_selectedInnerBank;
};