#pragma once

#include "controller.h"
#include "bus.h"
#include "cpu.h"
#include "ram.h"
#include "apu.h"
#include "cartridge.h"
#include "ppu.h"

#include <string>
#include <functional>
#include <cstdint>

class Nes
{
    public:
        Nes(const std::string& nesFile, std::function<uint8_t()> btnStateGetter, std::function<void(const uint32_t*)> frameUpdate);

        void start();
        void reset();

    private:
        Controller m_controller;
        Bus m_bus;
        Cartridge m_cartridge;
        Ppu m_ppu;
        Cpu m_cpu;
        Ram m_ram;
        Apu m_apu;
        uint64_t m_numOfCycles;
        bool m_writeComplete;
        uint8_t m_dmaData;
        uint16_t m_dmaOffset;
        bool m_dummyDma;
};