#pragma once

#include "device.h"
#include <vector>

enum class Mode
{
    FOUR_STEP = 0,
    FIVE_STEP = 1
};

struct Dmc
{
    Dmc();
    bool enabled;
    bool irqEnabled;
    bool loop;
    uint8_t frequency;
    uint8_t loadCounter;
    uint16_t sampleAddress;
    uint16_t sampleLength;

    uint16_t addressOffset;
    uint16_t currentSample;
    uint16_t numPlayedSmaples;

    uint8_t cnt;

    std::vector<uint16_t> rates;
    uint16_t rateCnt;
    bool irqRaised;

    void clock();

};

class Apu : public Device
{
    public:
        Apu();
        uint8_t cpuRead(uint16_t address) override;
        void cpuWrite(uint16_t address, uint8_t data) override;
        bool isAddressInRange(uint16_t address) const override;

        void clock();
        bool irqRaised();
        bool dmcIrqRaised();

    private:
        Mode m_mode;
        int32_t m_divider;
        int32_t m_counter;
        uint8_t m_step;
        bool m_irqEnabled;
        bool m_irqRaised;
        Dmc m_dmc;

        void fourStepClock();
        void fiveStepClock();
};