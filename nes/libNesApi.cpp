#include "include/nes.h"

extern "C"
{
    Nes* nes_new(const char* nesFile, uint8_t(*btnStateGetter)(void), void(*onNewFrame)(const uint32_t*))
    {
        return new Nes(nesFile, btnStateGetter, onNewFrame);
    }

    void nes_start(Nes* nesPtr)
    {
        nesPtr->start();
    }

    void nes_reset(Nes* nesPtr)
    {
        nesPtr->reset();
    }
}