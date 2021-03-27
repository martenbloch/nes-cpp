#include "include/utils.h"

#include <fstream>
#include <sstream>
#include <iomanip>

std::vector<uint8_t> getFileConent(const std::string& filePath)
{
    std::ifstream ifs{filePath, std::ios::binary};

    if(!ifs.is_open())
    {
        throw std::runtime_error("getFileConent() File does not exists:" + filePath);
    }

    ifs.seekg(0, ifs.end);
    auto size = ifs.tellg();

    if(size == 0)
    {
       throw std::runtime_error("getFileConent() Empty file:" + filePath); 
    }

    std::vector<uint8_t> data(size);

    ifs.seekg(0, ifs.beg);
    if(!ifs.read((char*)data.data(), data.size()))
        throw std::runtime_error("getFileConent() Failed to read file:" + filePath);

    return data;
}

std::string toHexString(uint16_t val, uint8_t width)
{
    std::stringstream ss;
    ss << std::uppercase << "0x" << std::setfill ('0') << std::setw(width) << std::hex << val;
    return ss.str();
}

std::string toHex(uint16_t val, uint8_t width)
{
    std::stringstream ss;
    ss << std::uppercase << std::setfill ('0') << std::setw(width) << std::hex << val;
    return ss.str();
}

int hexToSignedInt(uint8_t v)
{
    if(v & 0x80)
        return -((~ v + 1) & 0xff);
    return v;
}

uint8_t signedIntToHex(int v)
{
    return v & 0xFF;
    /*
    if(v < 0)
        return (v & (2^32-1)) & 0xff;
    else
        return v;
    */
}