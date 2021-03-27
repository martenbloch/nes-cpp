#pragma once

#include "device.h"
#include "mapper.h"

#include <string>
#include <memory>
#include <vector>

class Cartridge : public Device
{
    public:
        enum class Mirroring
        {
            HORIZONTAL = 0,
            VERTICAL = 1
        };

        struct NesFileHeader
        {
            uint8_t numPrgBlocks;
            uint8_t numChrBlocks;
            Mirroring mirroring;
            uint8_t mapperId;
            bool trainer;
        };

        Cartridge(const std::string& nesFile);

        uint8_t cpuRead(uint16_t address) override;
        void cpuWrite(uint16_t address, uint8_t data) override;
        bool isAddressInRange(uint16_t address) const override;

        uint8_t ppuRead(uint16_t address);
        void ppuWrite(uint16_t address, uint8_t data);

        Mirroring getMirroring();

        void scanline();
        bool isIrqActive();
        void clearIrq();

    private:
        std::unique_ptr<Mapper> m_mapper;
        NesFileHeader m_nesFileHeader;

        NesFileHeader getNesFileHeader(const std::vector<uint8_t>& data);
        std::unique_ptr<Mapper> createMapper(const NesFileHeader& nesFileHeader, std::vector<uint8_t> prg, std::vector<uint8_t> chr);
};