#include "include/controller.h"

#include <iostream>

Controller::Controller(std::function<uint8_t()> btnStateGetter)
: m_btnStateGetter{btnStateGetter}
, m_counter{0}
, m_startPressed{false}
, m_selectPressed{false}
, m_leftPressed{false}
, m_rightPressed{false}
, m_upPressed{false}
, m_downPressed{false}
, m_aPressed{false}
, m_bPressed{false}
, m_readButton{false}
{

}

uint8_t Controller::cpuRead(uint16_t address)
{
    if(address == 0x4016)
    {
        uint8_t result = 0x40;
        if(m_readButton)
        {
            if(m_counter == 7 && m_rightPressed)
            {
                result = 0x41;
            }
            else if(m_counter == 6 && m_leftPressed)
            {
                result = 0x41;
            }
            else if(m_counter == 5 && m_downPressed)
            {
                result = 0x41;
            }
            else if(m_counter == 4 && m_upPressed)
            {
                result = 0x41;
            }
            else if(m_counter == 3 && m_startPressed)
            {
                result = 0x41;
            }
            else if(m_counter == 2 && m_selectPressed)
            {
                result = 0x41;
            }
            else if(m_counter == 1 && m_bPressed)
            {
                result = 0x41;
            }
            else if(m_counter == 0 && m_aPressed)
            {
                result = 0x41;
            }
            else if(m_counter > 7)
                result = 0x40;

            m_counter += 1;
        }
        return result;
    }
    return 0x40;
}

void Controller::cpuWrite(uint16_t address, uint8_t data)
{
    if(address == 0x4016)
    {
        m_counter = 0;
        if((data & 0x1) == 0)
        {
            m_readButton = true;
            auto data = m_btnStateGetter();
            m_startPressed = false;
            m_selectPressed = false;
            m_leftPressed = false;
            m_rightPressed = false;
            m_upPressed = false;
            m_downPressed = false;
            m_aPressed = false;
            m_bPressed = false;
            if(data & 0x01)
            {
                m_upPressed = true;
            }
            if(data & 0x02)
            {
                m_downPressed = true;
            }
            if(data & 0x04)
            {
                m_leftPressed = true;
            }
            if(data & 0x08)
            {
                m_rightPressed = true;
            }
            if(data & 0x10)
            {
                m_startPressed = true;
            }
            if(data & 0x20)
            {
                m_selectPressed = true;
            }
            if(data & 0x40)
            {
                m_bPressed = true;
            }
            if(data & 0x80)
            {
                m_aPressed = true;
            }
        }
        else
            m_readButton = false;
    }
}

bool Controller::isAddressInRange(uint16_t address) const
{
    return address == 0x4016 || address == 0x4017;
}