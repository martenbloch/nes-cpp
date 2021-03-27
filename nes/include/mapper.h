#pragma once

#include <cstdint>

class Mapper
{
    public:
        virtual ~Mapper() = default;
        virtual uint16_t cpuRead(uint16_t address) = 0;
        virtual void cpuWrite(uint16_t address, uint8_t data) = 0;
        virtual uint16_t ppuRead(uint16_t address) = 0;
        virtual void ppuWrite(uint16_t address, uint8_t data) = 0;
        virtual void scanline(){};
        virtual bool isIrqActive(){return false;};
        virtual void clearIrq(){};
};