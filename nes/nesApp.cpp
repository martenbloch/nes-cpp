#include "include/nes.h"

#include <iostream>

uint8_t btnStateGetter()
{
    return 0x00;
}

void onFrameUpdate(const uint32_t* data)
{
    return;
}

int main()
{
    Nes nes("tsubasa.nes", btnStateGetter, onFrameUpdate);
    nes.reset();
    nes.start();

    return 0;
}