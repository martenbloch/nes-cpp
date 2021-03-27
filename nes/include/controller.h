#pragma once

#include "device.h"

#include <functional>

class Controller : public Device
{
    public:
        Controller(std::function<uint8_t()> btnStateGetter);
        uint8_t cpuRead(uint16_t address) override;
        void cpuWrite(uint16_t address, uint8_t data) override;
        bool isAddressInRange(uint16_t address) const override;

        bool m_startPressed;
        bool m_selectPressed;
        bool m_leftPressed;
        bool m_rightPressed;
        bool m_upPressed;
        bool m_downPressed;
        bool m_aPressed;
        bool m_bPressed;

    private:
        std::function<uint8_t()> m_btnStateGetter;
        uint8_t m_counter;
        /*
        bool m_startPressed;
        bool m_selectPressed;
        bool m_leftPressed;
        bool m_rightPressed;
        bool m_upPressed;
        bool m_downPressed;
        bool m_aPressed;
        bool m_bPressed;
        */
        bool m_readButton;
};