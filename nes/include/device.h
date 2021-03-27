#pragma once

#include <cstdint>

class Device
{
    public:
        virtual ~Device() = default;
        virtual uint8_t cpuRead(uint16_t address) = 0;
        virtual void cpuWrite(uint16_t address, uint8_t data) = 0;
        virtual bool isAddressInRange(uint16_t address) const = 0;
};