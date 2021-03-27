#pragma once

#include <cstdint>
#include <string>

class Instruction
{
    public:
        virtual uint8_t execute() = 0;
        virtual std::string str() = 0;
        virtual uint8_t size() = 0;
};
