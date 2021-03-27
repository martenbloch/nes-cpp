#pragma once

#include "device.h"

#include <cstdint>
#include <vector>
#include <functional>

class Bus
{
    public:
        Bus();
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        void connect(Device& device);
        bool isDmaRequested();
        uint8_t getHighByte();
        void clearDmaRequest();

        Device& getDeviceByAddress(uint16_t address); // TODO: move to private

    private:
        std::vector<std::reference_wrapper<Device>> m_devices;
        bool m_dmaRequest;
        uint8_t m_dmaHighByte;
};