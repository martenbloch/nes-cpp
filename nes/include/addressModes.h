#pragma once

#include "cpu.h"

#include <cstdint>
#include <string>

class AddressMode
{
    public:
        virtual uint32_t getAddress() = 0;
        virtual uint8_t cycles() = 0;
        virtual std::string str() = 0;
        virtual uint8_t size() = 0;
};

class AddressModeAccumulator : public AddressMode
{
    public:
        AddressModeAccumulator(Cpu& cpu);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;
    
    private:
        Cpu& m_cpu;
};

class AddressModeAbsolute : public AddressMode
{
    public:
        AddressModeAbsolute(Cpu& cpu, uint8_t cycles = 4);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;
    
    private:
        Cpu& m_cpu;
        uint16_t m_absAddress;
        uint8_t m_cycles;
        uint8_t m_ll;
        uint8_t m_hh;
};

class AddressModeAbsoluteXIndexed : public AddressMode
{
    public:
        AddressModeAbsoluteXIndexed(Cpu& cpu, uint8_t cycles, bool extraCycle = true);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        uint16_t m_absAddress;
        uint8_t m_ll;
        uint8_t m_hh;
        uint8_t m_cycles;
        uint8_t m_cyclesO;
        bool m_extraCycle;
};

class AddressModeAbsoluteYIndexed : public AddressMode
{
    public:
        AddressModeAbsoluteYIndexed(Cpu& cpu, uint8_t cycles, bool extraCycle = true);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        uint8_t m_ll;
        uint8_t m_hh;
        uint8_t m_cycles;
        uint8_t m_cyclesO;
        bool m_extraCycle;
};

class AddressModeImmediate : public AddressMode
{
    public:
        AddressModeImmediate(Cpu& cpu);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        uint16_t m_addr;
        uint8_t m_cycles;
};

class AddressModeImplied : public AddressMode
{
    public:
        AddressModeImplied(uint8_t cycles = 2);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;

    private:
        uint8_t m_cycles;
};

class AddressModeIndirect : public AddressMode
{
    public:
        AddressModeIndirect(Cpu& cpu, uint8_t cycles=5);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        uint8_t m_cycles;
        uint16_t m_ptr;
        uint8_t m_ll;
        uint8_t m_hh;
        uint16_t m_addr;
};

class AddressModeIndirectX : public AddressMode
{
    public:
        AddressModeIndirectX(Cpu& cpu, uint8_t cycles = 6);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        uint16_t m_addr;
        uint8_t m_ll;
        uint8_t m_cycles;
};

class AddressModeIndirectY : public AddressMode
{
    public:
        AddressModeIndirectY(Cpu& cpu, uint8_t cycles = 5, bool extraCycles = true);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        uint16_t m_addr;
        uint8_t m_ll;
        uint8_t m_cycles;
        uint8_t m_cyclesO;
        uint8_t m_extraCycles;
};

class AddressModeRelative : public AddressMode
{
    public:
        AddressModeRelative(Cpu& cpu);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        uint16_t m_addr;
        uint8_t m_cycles;
        uint8_t m_data;
};

class AddressModeZeroPage : public AddressMode
{
    public:
        AddressModeZeroPage(Cpu& cpu, uint8_t cycles=3);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        uint16_t m_address;
        uint8_t m_cycles;
};

class AddressModeZeroPageXIndexed : public AddressMode
{
    public:
        AddressModeZeroPageXIndexed(Cpu& cpu, uint8_t cycles=4);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        uint16_t m_absAddress;
        uint8_t m_ll;
        uint8_t m_cycles;
        uint8_t m_addr;
};

class AddressModeZeroPageYIndexed : public AddressMode
{
    public:
        AddressModeZeroPageYIndexed(Cpu& cpu);

        uint32_t getAddress() override;
        uint8_t cycles() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        uint8_t m_ll;
        uint8_t m_cycles;
        uint16_t m_addr;
};
