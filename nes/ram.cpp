#include "include/ram.h"
#include "include/utils.h"

#include <iostream>

Ram::Ram()
: m_data(0x1FFF, 0)
{
}

uint8_t Ram::cpuRead(uint16_t address)
{
    if(address == 0x0000)
    {
        //std::cout << "read:" << toHex(m_data[address & 0x07FF],2) << std::endl;
    }

    return m_data[address & 0x07FF];
}

void Ram::cpuWrite(uint16_t address, uint8_t data)
{
    if(address == 0x0000)
    {
        //std::cout << "write:" << toHex(data,2) << std::endl;
    }
    m_data[address & 0x07FF] = data;
}

bool Ram::isAddressInRange(uint16_t address) const
{
    return address >= 0x0000 && address <= 0x1FFF;
}