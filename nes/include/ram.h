#pragma once

#include "device.h"

#include <vector>

class Ram : public Device
{
    public:
        Ram();
        uint8_t cpuRead(uint16_t address) override;
        void cpuWrite(uint16_t address, uint8_t data) override;
        bool isAddressInRange(uint16_t address) const override;

    private:
        std::vector<uint8_t> m_data;
};