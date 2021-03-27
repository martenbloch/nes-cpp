#include "include/bus.h"

#include <iostream>

Bus::Bus()
: m_dmaRequest{false}
, m_dmaHighByte{0x00}
{

}

uint8_t Bus::read(uint16_t address)
{
    return (address != 0x4014) ?  getDeviceByAddress(address).cpuRead(address) : m_dmaHighByte;
}

void Bus::write(uint16_t address, uint8_t data)
{
    if(address != 0x4014)
    {
        getDeviceByAddress(address).cpuWrite(address, data);
    }
    else
    {
        m_dmaHighByte = data;
        m_dmaRequest = true;
    }
}

void Bus::connect(Device& device)
{
    m_devices.push_back(device);
}

Device& Bus::getDeviceByAddress(uint16_t address)
{
    for(auto& device : m_devices)
    {
        if(device.get().isAddressInRange(address))
        {
            return device.get();
        }
    }
    throw std::runtime_error("Device not found for address:" + std::to_string(address));
}

bool Bus::isDmaRequested()
{
    return m_dmaRequest;
}

uint8_t Bus::getHighByte()
{
    return m_dmaHighByte;
}

void Bus::clearDmaRequest()
{
    m_dmaRequest = false;
}