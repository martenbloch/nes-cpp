#pragma once

#include <vector>
#include <cstdint>
#include <string>

std::vector<uint8_t> getFileConent(const std::string& filePath);

std::string toHexString(uint16_t val, uint8_t width);
std::string toHex(uint16_t val, uint8_t width);

int hexToSignedInt(uint8_t v);
uint8_t signedIntToHex(int v);
