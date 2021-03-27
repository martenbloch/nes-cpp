#pragma once

#include "bus.h"
#include "instruction.h"
#include "controller.h" //remove it
#include "ppu.h" // remove it

#include <cstdint>
#include <array>
#include <memory>

struct StatusRegister
{
    uint8_t n;      // negative
    uint8_t v;      // overflow
    uint8_t unused; // unused
    uint8_t b;      // break
    uint8_t d;      // decimal
    uint8_t i;      // interrupt
    uint8_t z;      // zero
    uint8_t c;      // carry

    StatusRegister();
    uint8_t toByte();
    void fromByte(uint8_t data);
};

struct CpuState
{
    uint16_t pc;
    StatusRegister sr;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    int sp;

    std::string str(); 
};

class Cpu
{
    public:
        Cpu(Bus& bus, Controller& c, Ppu& p);
        CpuState& getState();
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        void push(uint8_t value);
        uint8_t pop();

        void clock();
        void reset();
        void nmi();
        void irq();
        uint64_t getClockTicks();
        void increaseClockTicks(uint16_t value);
        uint8_t cyclesLeft();
        bool isPrintEnbled();
        Controller& m_c;
        Ppu& m_ppu;

    private:
        Bus& m_bus;
        CpuState m_cpuState;
        bool m_enablePrint;
        uint64_t m_clockTicks;
        uint8_t m_operand;
        std::array<std::unique_ptr<Instruction>, 256> m_instructions;
        bool m_execBitIns;
        std::string m_logMsg;
        std::string m_cpuMsg;
        uint8_t m_cyclesLeftToPerformCurrentInstruction;
        bool m_newInstruction;
        uint64_t m_clk;

        std::string instAsBytes(uint16_t pc, uint8_t instructionSize);
};