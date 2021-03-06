#include "include/ppu.h"

#include <iostream>
#include <exception>
#include <sstream>

Ppuctrl byteToPpuCtrl(uint8_t data)
{
    Ppuctrl ctrl;
    uint8_t val{static_cast<uint8_t>(data & 0x3u)};
    if(val == 0)
        ctrl.baseNameTableAddress = 0x2000;
    else if(val == 1)
        ctrl.baseNameTableAddress = 0x2400;
    else if(val == 2)
        ctrl.baseNameTableAddress = 0x2800;
    else
        ctrl.baseNameTableAddress = 0x2C00;

    ctrl.vramAddressIncrement = ((data & 0x4) == 0) ? 1 : 32;
    ctrl.spritePatternTableAddress = ((data & 0x8) == 0) ? 0x0000 : 0x1000;
    ctrl.backgroundPatternTableAddress = ((data & 0x10) == 0) ? 0x0000 : 0x1000;
    ctrl.spriteSize = ((data & 0x20) == 0) ? 0 : 1; 
    ctrl.generateNmi = (data & 0x80) > 0;

    return ctrl;   
}

PpuMask::PpuMask()
: showBackground{false}, showSprites{false}
{

}

PpuMask byteToPpuMask(uint8_t data)
{
    PpuMask mask;

    mask.showBackground = ((data & 0x8) > 0);
    mask.showSprites = ((data & 0x10) > 0);
    return mask;
}

Status::Status()
: verticalBlank{false}, spriteZeroHit{false}, spriteOverflow{false}
{

}

uint8_t Status::toByte()
{
    return (static_cast<uint8_t>(verticalBlank) << 7) | (static_cast<uint8_t>(spriteZeroHit) << 6) | (static_cast<uint8_t>(spriteOverflow) << 5);
}

uint16_t VramRegister::getVramAddress() 
{
    return baseNameTable | (tileY << 5) | tileX;
}

void VramRegister::incrementTileX()
{
    if(tileX == 31)
    {
        tileX = 0;
        baseNameTable ^= 0x400;
    }
    else
    {
        tileX += 1;
        vramAddr += 1;
    }
}

void VramRegister::incrementTileY()
{
    if(fineY < 7)
        fineY += 1;
    else
    {
        fineY = 0;
        if(tileY == 29)
        {
            tileY = 0;
            baseNameTable ^= 0x800;
        }
        else
            tileY += 1;
    }
}

void VramRegister::setAddress(uint16_t address)
{
    vramAddr = address;
    tileX = address & 0x1f;
    tileY = ((address & 0x3e0) >> 5) & 0x1F;
    setBaseNameTable(address & 0xc00);
}

void VramRegister::setBaseNameTable(uint8_t value)
{
    if(value == 0)
        baseNameTable = 0x2000;
    else if(value == 1)
        baseNameTable = 0x2400;
    else if(value == 2)
        baseNameTable = 0x2800;
    else if(value == 3)
        baseNameTable = 0x2C00;
}

void VramRegister::scrollX(uint8_t x)
{
    tileX = x >> 3;
}

void VramRegister::scrollY(uint8_t y)
{
    tileY = y >> 3;
    fineY = y - (tileY * 8);
}

std::string VramRegister::str()
{
    std::stringstream ss;
    ss << std::dec << "tileX:" << int(tileX)
                   << "    tileY:" << int(tileY)
                   << "    fineY:" << int(fineY)
                   << std::hex
                   << "    baseNameTable:" << int(baseNameTable)
                   << "    vramAddr:" << getVramAddress();
    return ss.str();
}

OamData::OamData()
: x{0xFF},
  tile_num{0xFF},
  attr{0xFF},
  y{0xFF}
{

}

uint8_t OamData::palette()
{
    return attr & 0x3;
}

bool OamData::flipHorizontally()
{
    return (attr & 0x40) > 0;
}

bool OamData::flipVertical()
{
    return (attr & 0x80) > 0;
}

Ppu::Ppu(Cartridge& cartridge, std::function<void(const uint32_t*)> frameUpdate)
: m_cartridge{cartridge} 
 , m_cycle{0}, 
 m_scanline{0}, 
 m_isOddFrame{true}, 
 m_ctrl{}, 
 m_mask{}, 
 m_status{}, 
 m_currAddr{}, 
 m_tmpAddr{},
 m_th{0x0000, 0x0000},
 m_paletteIdx{0x0},
 m_backgroundHalf{0},
 m_frameCnt{1},
 m_nextTileId{0},
 m_paletteBaseAddr{0x3F00},
 m_nextTileData{0x0000, 0x0000},
 m_bgPixel{0x0},
 m_frameData(256*240),
 m_raiseNmi{false},
 m_lastWrittenData{0x00},
 m_addressLatch{0},
 m_readBuffer{0x00},
 m_ppuAddr{0x0000},
 m_oam{},
 m_oamAddr{0x00},
 m_secondaryOam{},
 m_numSecondarySprites{0x00},
 m_secondaryOamNumPixelToDraw{{8, 8, 8, 8, 8, 8, 8, 8}},
 m_secondaryOamXCounter{{0, 0, 0, 0, 0, 0, 0, 0}},
 m_secondaryOamAttrBytes{{8, 8, 8, 8, 8, 8, 8, 8}},
 m_sh{TileHelper(0x0000,0x0000), TileHelper(0x0000,0x0000),TileHelper(0x0000,0x0000),TileHelper(0x0000,0x0000),TileHelper(0x0000,0x0000),TileHelper(0x0000,0x0000),TileHelper(0x0000,0x0000),TileHelper(0x0000,0x0000)},
 m_nextAttribDataH{0x0000, 0x0000},
 m_nextAttribData{0x00},
 m_vblankRead{false},
 m_vblankFlagRead{false},
 m_fineX{0x0}
 , m_raiseNmiNextIns{false}
 , m_frameNum{0x01}
 , m_frameUpdate{frameUpdate}
{
    /*
    m_palette[0x00] = {84, 84, 84};
    m_palette[0x01] = {0, 30, 116};
    m_palette[0x02] = {8, 16, 144};
    m_palette[0x03] = {48, 0, 136};
    m_palette[0x04] = {68, 0, 100};
    m_palette[0x05] = {92, 0, 48};
    m_palette[0x06] = {84, 4, 0};
    m_palette[0x07] = {60, 24, 0};
    m_palette[0x08] = {32, 42, 0};
    m_palette[0x09] = {8, 58, 0};
    m_palette[0x0A] = {0, 64, 0};
    m_palette[0x0B] = {0, 60, 0};
    m_palette[0x0C] = {0, 50, 60};
    m_palette[0x0D] = {0, 0, 0};
    m_palette[0x0E] = {0, 0, 0};
    m_palette[0x0F] = {0, 0, 0};

    m_palette[0x10] = {152, 150, 152};
    m_palette[0x11] = {8, 76, 196};
    m_palette[0x12] = {48, 50, 236};
    m_palette[0x13] = {92, 30, 228};
    m_palette[0x14] = {136, 20, 176};
    m_palette[0x15] = {160, 20, 100};
    m_palette[0x16] = {152, 34, 32};
    m_palette[0x17] = {120, 60, 0};
    m_palette[0x18] = {84, 90, 0};
    m_palette[0x19] = {40, 114, 0};
    m_palette[0x1A] = {8, 124, 0};
    m_palette[0x1B] = {0, 118, 40};
    m_palette[0x1C] = {0, 102, 120};
    m_palette[0x1D] = {0, 0, 0};
    m_palette[0x1E] = {0, 0, 0};
    m_palette[0x1F] = {0, 0, 0};

    m_palette[0x20] = {236, 238, 236};
    m_palette[0x21] = {76, 154, 236};
    m_palette[0x22] = {120, 124, 236};
    m_palette[0x23] = {176, 98, 236};
    m_palette[0x24] = {228, 84, 236};
    m_palette[0x25] = {236, 88, 180};
    m_palette[0x26] = {236, 106, 100};
    m_palette[0x27] = {212, 136, 32};
    m_palette[0x28] = {160, 170, 0};
    m_palette[0x29] = {116, 196, 0};
    m_palette[0x2A] = {76, 208, 32};
    m_palette[0x2B] = {56, 204, 108};
    m_palette[0x2C] = {56, 180, 204};
    m_palette[0x2D] = {60, 60, 60};
    m_palette[0x2E] = {0, 0, 0};
    m_palette[0x2F] = {0, 0, 0};

    m_palette[0x30] = {236, 238, 236};
    m_palette[0x31] = {168, 204, 236};
    m_palette[0x32] = {188, 188, 236};
    m_palette[0x33] = {212, 178, 236};
    m_palette[0x34] = {236, 174, 236};
    m_palette[0x35] = {236, 174, 212};
    m_palette[0x36] = {236, 180, 176};
    m_palette[0x37] = {228, 196, 144};
    m_palette[0x38] = {204, 210, 120};
    m_palette[0x39] = {180, 222, 120};
    m_palette[0x3A] = {168, 226, 144};
    m_palette[0x3B] = {152, 226, 180};
    m_palette[0x3C] = {160, 214, 228};
    m_palette[0x3D] = {160, 162, 160};
    m_palette[0x3E] = {0, 0, 0};
    m_palette[0x3F] = {0, 0, 0};
    */

    m_palette[0x00] = 0x545454;
    m_palette[0x01] = 0x001E74;
    m_palette[0x02] = 0x081090;
    m_palette[0x03] = 0x300088;
    m_palette[0x04] = 0x440064;
    m_palette[0x05] = 0x5C0030;
    m_palette[0x06] = 0x540400;
    m_palette[0x07] = 0x3C1800;
    m_palette[0x08] = 0x202A00;
    m_palette[0x09] = 0x083A00;
    m_palette[0x0A] = 0x004000;
    m_palette[0x0B] = 0x003C00;
    m_palette[0x0C] = 0x00323C;
    m_palette[0x0D] = 0x000000;
    m_palette[0x0E] = 0x000000;
    m_palette[0x0F] = 0x000000;
    m_palette[0x10] = 0x989698;
    m_palette[0x11] = 0x084CC4;
    m_palette[0x12] = 0x3032EC;
    m_palette[0x13] = 0x5C1EE4;
    m_palette[0x14] = 0x8814B0;
    m_palette[0x15] = 0xA01464;
    m_palette[0x16] = 0x982220;
    m_palette[0x17] = 0x783C00;
    m_palette[0x18] = 0x545A00;
    m_palette[0x19] = 0x287200;
    m_palette[0x1A] = 0x087C00;
    m_palette[0x1B] = 0x007628;
    m_palette[0x1C] = 0x006678;
    m_palette[0x1D] = 0x000000;
    m_palette[0x1E] = 0x000000;
    m_palette[0x1F] = 0x000000;
    m_palette[0x20] = 0xECEEEC;
    m_palette[0x21] = 0x4C9AEC;
    m_palette[0x22] = 0x787CEC;
    m_palette[0x23] = 0xB062EC;
    m_palette[0x24] = 0xE454EC;
    m_palette[0x25] = 0xEC58B4;
    m_palette[0x26] = 0xEC6A64;
    m_palette[0x27] = 0xD48820;
    m_palette[0x28] = 0xA0AA00;
    m_palette[0x29] = 0x74C400;
    m_palette[0x2A] = 0x4CD020;
    m_palette[0x2B] = 0x38CC6C;
    m_palette[0x2C] = 0x38B4CC;
    m_palette[0x2D] = 0x3C3C3C;
    m_palette[0x2E] = 0x000000;
    m_palette[0x2F] = 0x000000;
    m_palette[0x30] = 0xECEEEC;
    m_palette[0x31] = 0xA8CCEC;
    m_palette[0x32] = 0xBCBCEC;
    m_palette[0x33] = 0xD4B2EC;
    m_palette[0x34] = 0xECAEEC;
    m_palette[0x35] = 0xECAED4;
    m_palette[0x36] = 0xECB4B0;
    m_palette[0x37] = 0xE4C490;
    m_palette[0x38] = 0xCCD278;
    m_palette[0x39] = 0xB4DE78;
    m_palette[0x3A] = 0xA8E290;
    m_palette[0x3B] = 0x98E2B4;
    m_palette[0x3C] = 0xA0D6E4;
    m_palette[0x3D] = 0xA0A2A0;
    m_palette[0x3E] = 0x000000;
    m_palette[0x3F] = 0x000000;

    std::cout << "m_raiseNmi " << m_raiseNmi << std::endl;

}

uint8_t Ppu::readVideoMem(uint16_t address)
{
    address &= 0x3FFF;
    if(address >= 0x2000 && address <= 0x3eff)
    {
        uint16_t index{static_cast<uint16_t>(address & 0x03ff)};
        if(m_cartridge.getMirroring() == Cartridge::Mirroring::HORIZONTAL)  // horizontal
        {
            if( (address >= 0x2000 && address <= 0x23ff) || (address >= 0x2400 && address <= 0x27ff))
                return m_nt0[index];
            else if((address >= 0x2800 && address <= 0x2bff) || (address >= 0x2c00 && address <= 0x2fff))
                return m_nt1[index];
            else if( (address >= 0x3000 && address <= 0x33ff) || (address >= 0x3400 && address <= 0x37ff))
                return m_nt0[index];
            else if((address >= 0x3800 && address <= 0x3bff) || (address >= 0x3c00 && address <= 0x3eff))
                return m_nt1[index];
        }
        else
        {
            if((address >= 0x2000 && address <= 0x23ff) || (address >= 0x2800 && address <= 0x2bff))
                return m_nt0[index];
            else if((address >= 0x2400 && address <= 0x27ff) || (address >= 0x2c00 && address <= 0x2fff))
                return m_nt1[index];
            else if((address >= 0x3000 && address <= 0x33ff) || (address >= 0x3800 && address <= 0x3bff))
                return m_nt0[index];
             else if((address >= 0x3400 && address <= 0x37ff) || (address >= 0x3c00 && address <= 0x3eff))
                return m_nt1[index];               
        }
    } 
    else if(address >= 0x3f00 && address <= 0x3fff) 
    {
        if(address == 0x3f04 || address == 0x3F08 || address == 0x3F0C)
        {
            address = 0x3f00;
        }

        address &= 0x001F;

        return m_paletteRam[address & 0xff]; // TODO: check it

    }
    else if( address >= 0 && address <= 0x1fff)
    {
        return m_cartridge.ppuRead(address);  // TODO: check it
    }

    std::cout << "readVideoMem address out of range: 0x" << std::hex << address << std::endl;
    return 0x0000;
}

TileRow Ppu::getTileData(uint8_t tileNum, uint8_t row, uint8_t half)
{
    uint16_t lowerIdx{static_cast<uint16_t>((16 * tileNum) + row)};
    uint16_t upperIdx{static_cast<uint16_t>(lowerIdx + 8)};
    if(half == 0)   // left
        return {m_cartridge.ppuRead(lowerIdx), m_cartridge.ppuRead(upperIdx)};
    else           // right - background
        return {m_cartridge.ppuRead(lowerIdx | 0x1000), m_cartridge.ppuRead(upperIdx | 0x1000)};
}

void Ppu::getPaletteIdx()
{
    uint8_t attr_data = readVideoMem(m_currAddr.baseNameTable | 0x3c0 | (m_currAddr.tileX >> 2) | ((m_currAddr.tileY >> 2) << 3));

    if(m_currAddr.tileY & 0x02)
        attr_data = attr_data >> 4;
    else if(m_currAddr.tileX & 0x02)
        attr_data = attr_data >> 2;
    attr_data = attr_data & 0x03;
    m_paletteIdx = attr_data;
}

void Ppu::clearSecondaryOam()
{
    m_secondaryOam.fill(OamData());
}

void Ppu::fillSecondaryOam(int y)
{
    m_numSecondarySprites = 0;
    m_secondaryOamNumPixelToDraw.fill(0);
    uint8_t n = (m_ctrl.spriteSize == 1) ? 16 : 8;
    
    for(int i =0; i < 64; ++i)
    {
        if(y >= m_oam[i].y && y < (m_oam[i].y + n))
        {
            if(m_numSecondarySprites < 8)
            {
                m_secondaryOam[m_numSecondarySprites] = m_oam[i];
                m_secondaryOamXCounter[m_numSecondarySprites] = m_oam[i].x + 1;
                m_secondaryOamAttrBytes[m_numSecondarySprites] = m_oam[i].attr;
                m_secondaryOamNumPixelToDraw[m_numSecondarySprites] = 8;
            }
            m_numSecondarySprites += 1;
            if(m_numSecondarySprites >= 8)
            {
                //std::cout << "Sprite Overflow\r\n";
                m_status.spriteOverflow = true;
                break;
            }
        }
    }
}

void Ppu::decrementSpriteXCounters()
{
    for(int i =0; i < m_numSecondarySprites; ++i )
    {
        if(m_secondaryOamXCounter[i] > 0)
            m_secondaryOamXCounter[i] -= 1;
        if(m_secondaryOamXCounter[i] == 0 && m_secondaryOamNumPixelToDraw[i] > 0)
        {
            uint8_t color = m_sh[i].shift();

            uint8_t pallete_idx = m_secondaryOam[i].palette();
            uint8_t idx = (readVideoMem(0x3F10 + (pallete_idx << 2) + color)) & 0x3f;

            uint8_t priority = (m_secondaryOamAttrBytes[i] & 0x20) >> 5;

            if(m_bgPixel == 0 && color == 0)
            {
                idx = (readVideoMem(0x3F00) & 0x3f);
                m_frameData[(m_cycle - 1) + (256 * m_scanline)] = m_palette[idx];
            }
            else if(m_bgPixel == 0 && color != 0)
                m_frameData[(m_cycle - 1) + (256 * m_scanline)] = m_palette[idx];
            else if(m_bgPixel != 0 && color != 0 && priority == 0)
                m_frameData[(m_cycle - 1) + (256 * m_scanline)] = m_palette[idx];

            m_secondaryOamNumPixelToDraw[i] -= 1;

            if(!m_status.spriteZeroHit && m_oam[0].tile_num == m_secondaryOam[i].tile_num && color != 0 && m_bgPixel != 0)
            {
                m_status.spriteZeroHit = true;
            }
        }
    }
}

void Ppu::fillSpritesShiftRegisters(int y)
{
    for(int i=0; i<8; ++i)
    {
        OamData sprite = m_secondaryOam[i];
        uint8_t row = y - sprite.y;
        uint8_t half = 0;
        if(m_ctrl.spritePatternTableAddress == 0x1000)
            half = 1;
        
        uint8_t tile_num = sprite.tile_num;
        if(m_ctrl.spriteSize == 1)
        {
            row = row % 8;
            tile_num &= 0xFE;
            half = sprite.tile_num & 0x1;
            if(y - sprite.y >= 8)
                tile_num += 1;
        }

        if(sprite.flipVertical())
        {
            row = 7 - row;
        }

        TileRow tr = getTileData(tile_num, row, half);
        
        if(sprite.flipHorizontally())
        {
            tr.lower = reverseBits(tr.lower);
            tr.upper = reverseBits(tr.upper);
        }

        uint16_t l = uint16_t(tr.lower) << 8;
        uint16_t u = uint16_t(tr.upper) << 8;

        m_sh[i].writeLower(l);
        m_sh[i].writeUpper(u);
    }
}


const std::vector<uint32_t>& Ppu::getScreenData()
{
    return m_frameData;
}

void Ppu::cpuWrite(uint16_t address, uint8_t data)
{
    address &= 0x7;
    if(address == 0x0)
    {
        auto oldNmi = m_ctrl.generateNmi;
        m_lastWrittenData = data;
        m_ctrl = byteToPpuCtrl(data);
        m_tmpAddr.setBaseNameTable(data & 0x3);

        if(data & 0x10)    // TODO: remove it
            m_backgroundHalf = 1;
        else
            m_backgroundHalf = 0;

        if( m_status.verticalBlank && m_ctrl.generateNmi && !oldNmi)
        {
            std::cout << "PPU write 0x2000 generate NMI now\r\n";
            m_raiseNmi = true;
            m_raiseNmiNextIns = true;
        }

        return;
    }
    else if(address == 0x1)
    {
        m_lastWrittenData = data;
        m_mask = byteToPpuMask(data);
        return;
    }
    else if(address == 0x3)
    {
        m_lastWrittenData = data;
        m_oamAddr = data;
    }
    else if(address == 0x4)
    {
        m_lastWrittenData = data;
        writeOamData(m_oamAddr, data);
        m_oamAddr += 1;
    }
    else if(address == 0x5)
    {
        m_lastWrittenData = data;
        if(m_addressLatch == 0)
        {
            m_addressLatch = 1;
            m_tmpAddr.scrollX(data);
            m_fineX = data & 0x7;
        }
        else
        {
            m_addressLatch = 0;
            m_tmpAddr.scrollY(data);
        }
    }
    else if(address == 0x6)
    {
        m_lastWrittenData = data;
        if(m_addressLatch == 0)
        {
            m_addressLatch = 1;
            m_ppuAddr = 0x0000 | ((data & 0x3f) << 8);
            m_tmpAddr.setAddress((m_tmpAddr.vramAddr & 0x00ff) | ((data & 0x3f) << 8));
        }
        else
        {
            m_addressLatch = 0;
            m_ppuAddr = m_ppuAddr | data;
            
            m_currAddr.setAddress(m_ppuAddr);
            m_tmpAddr.setAddress(m_ppuAddr);
        }
        return;
    }
    else if(address == 0x7)
    {
        m_lastWrittenData = data;
        if(m_currAddr.vramAddr >= 0x2000 && m_currAddr.vramAddr <= 0x3eff)
        {
            uint16_t index = m_currAddr.vramAddr & 0x3ff;
            if(m_cartridge.getMirroring() == Cartridge::Mirroring::HORIZONTAL)
            {
                if(m_currAddr.vramAddr >= 0x2000 && m_currAddr.vramAddr <= 0x23ff)
                    m_nt0[index] = data;
                else if(m_currAddr.vramAddr >= 0x2400 && m_currAddr.vramAddr <= 0x27ff)
                    m_nt0[index] = data;
                else if(m_currAddr.vramAddr >= 0x2800 && m_currAddr.vramAddr <= 0x2bff)
                    m_nt1[index] = data;
                else if(m_currAddr.vramAddr >= 0x2c00 && m_currAddr.vramAddr <= 0x2fff)
                    m_nt1[index] = data;
            }
            else
                if(m_currAddr.vramAddr >= 0x2000 && m_currAddr.vramAddr <= 0x23ff)
                    m_nt0[index] = data;
                else if(m_currAddr.vramAddr >= 0x2400 && m_currAddr.vramAddr <= 0x27ff)
                    m_nt1[index] = data;
                else if(m_currAddr.vramAddr >= 0x2800 && m_currAddr.vramAddr <= 0x2bff)
                    m_nt0[index] = data;
                else if(m_currAddr.vramAddr >= 0x2c00 && m_currAddr.vramAddr <= 0x2fff)
                    m_nt1[index] = data;
        }
        else if(m_currAddr.vramAddr >= 0x3f00 && m_currAddr.vramAddr <= 0x3fff)
        {
            uint16_t addr = m_currAddr.vramAddr & 0x001F;
            if(addr == 0x0010)
                addr = 0x0000;
            else if(addr == 0x0014)
                addr = 0x0004;
            else if(addr == 0x0018)
                addr = 0x0008;
            else if(addr == 0x001C)
                addr = 0x000C;
            m_paletteRam[addr] = data;
        }
        else if(m_currAddr.vramAddr >= 0 && m_currAddr.vramAddr <= 0x1fff)
            m_cartridge.ppuWrite(m_currAddr.vramAddr, data);
        else
            std::cout << "ppu::write() Eception, unhandled vram addr: " << m_currAddr.vramAddr << " data: " << int(data) << std::endl;

        m_currAddr.vramAddr += m_ctrl.vramAddressIncrement;
    }
    else
        std::cout << "ppu::write() Exception, unhandled address: " << address << " data: " << int(data) << std::endl;
}


uint8_t Ppu::cpuRead(uint16_t address)
{
    address &= 0x7;
    if(address == 0x2)
    {
        // fix sprite zero hit timing
        if(m_status.spriteZeroHit)
        {
            if(m_scanline == 260 && (m_cycle + 9) >= 341)
                m_status.spriteZeroHit = false;
        }

        // handle race condition in VBLANK reading
        if (((m_cycle + 9) == 341) && m_scanline == 240)
            m_vblankRead = true;

        if ((((m_cycle + 9) == 342) || ((m_cycle + 9) == 343)) && m_scanline == 240)
        {
            m_vblankRead = true;
            m_status.verticalBlank = true;
        }

        if ((m_cycle == 0 || m_cycle == 1) && m_scanline == 241)
        {
            m_vblankFlagRead = true;
            m_status.verticalBlank = true;            
        }

        if(m_cycle + 9 > 342 && m_scanline == 240)
        {
            m_vblankFlagRead = true;
            m_status.verticalBlank = true;             
        }

        uint8_t val = m_lastWrittenData & 0x1f | m_status.toByte() & 0xe0;
        m_status.verticalBlank = false;
        m_addressLatch = 0;
        return val;
    }
    else if(address == 0x4)
    {
        return readOamData(m_oamAddr);
    }
    else if(address == 0x7)
    {
        uint8_t val{0};
        uint16_t adr = m_currAddr.vramAddr & 0x3FFF;

        if( adr >= 0x2000 && adr <= 0x3eff)
        {
            val = m_readBuffer;
            m_readBuffer = readVideoMem(adr);
        }
        else if(adr >= 0x3f00 && adr <= 0x3fff)
        {
            uint8_t index = adr & 0x3ff;
            m_readBuffer = m_nt1[index];
            uint8_t addr = adr & 0x001F;
            if(addr == 0x0010)
                addr = 0x0000;
            else if(addr == 0x0014)
                addr = 0x0004;
            else if(addr == 0x0018)
                addr = 0x0008;
            else if(addr == 0x001C)
                addr = 0x000C;

            val = m_paletteRam[addr];
        }
        else if(adr >= 0 && adr <= 0x1fff)
        {
            val = m_readBuffer;
            m_readBuffer = m_cartridge.ppuRead(adr);
        }
        else
            throw std::exception();

        m_currAddr.vramAddr += m_ctrl.vramAddressIncrement;

        return val;
    }
    
    std::cout << "Ppu::read() Exception, unhandled address: 0x" << std::hex << address << std::endl;
    return 0x00;
}

void Ppu::reset()
{
    m_cycle = 24;
}

bool Ppu::isAddressInRange(uint16_t address) const
{
    return address >= 0x2000 && address < 0x3FFF;
}

bool Ppu::isNmiRaised()
{
    return m_raiseNmi;
}

void Ppu::clearNmi()
{
    m_raiseNmi = false;
    m_cycle += 21;
}

bool Ppu::bgRenderingEnabled()
{
    return m_mask.showBackground || m_mask.showSprites;
}

void Ppu::writeOamData(uint8_t address, uint8_t data)
{
        uint8_t idx = address/4;
        uint8_t param_idx = address % 4;

        if(idx >= m_oam.size())
        {
            std::cout << "out of range" << std::endl;
        }

        if(param_idx == 0)
            m_oam[idx].y = data;
        else if(param_idx == 1)
            m_oam[idx].tile_num = data;
        else if(param_idx == 2)
            m_oam[idx].attr = data;
        else if(param_idx == 3)
            m_oam[idx].x = data;
}

uint8_t Ppu::readOamData(uint8_t address)
{
    uint8_t idx = address/4;
    uint8_t param_idx = address % 4;

    if(idx >= m_oam.size())
    {
        std::cout << "readOamData(), out of range" << std::endl;
    }

    if(param_idx == 0)
        return m_oam[idx].y;
    else if(param_idx == 1)
        return m_oam[idx].tile_num;
    else if(param_idx == 2)
        return m_oam[idx].attr;
    else if(param_idx == 3)
        return m_oam[idx].x;

    std::cout << "read OAM should not happen..." << std::endl;
    return 0x0;
}

uint16_t Ppu::getCycle()
{
    return m_cycle;
}

int Ppu::getScanline()
{
    return m_scanline;
}

void Ppu::clock() 
{
    if (m_scanline == -1) 
    {
        if (m_cycle == 1) {
            m_status.verticalBlank = false;
            m_status.spriteZeroHit = false;
            m_status.spriteOverflow = false;
            m_vblankRead = false;
            m_vblankFlagRead = false;
        }
        else if(m_cycle == 257)
        {
            // hori(v) = hori(t)
            m_currAddr.tileX = m_tmpAddr.tileX;
            if(m_tmpAddr.baseNameTable == 0x2000)
                m_currAddr.baseNameTable = 0x2400;
            if(m_tmpAddr.baseNameTable == 0x2800)
                m_currAddr.baseNameTable = 0x2C00;
        }
        else if(m_cycle >= 280 && m_cycle <= 304) {
            if((m_tmpAddr.baseNameTable & 0x800) == 0x800)
                m_currAddr.baseNameTable |= 0x800;
            else
                m_currAddr.baseNameTable &= ~0x800;
            m_currAddr.tileY = m_tmpAddr.tileY;
            m_currAddr.fineY = m_tmpAddr.fineY;
        }
        else if(m_cycle == 322 && m_mask.showBackground)
        {
            uint8_t firstTileId = readVideoMem(m_currAddr.getVramAddress());
            getPaletteIdx();
            m_currAddr.incrementTileX();
            uint8_t secondTileId = readVideoMem(m_currAddr.getVramAddress());
            m_currAddr.incrementTileX();

            TileRow tr1 = getTileData(firstTileId, m_currAddr.fineY,  m_backgroundHalf);
            TileRow tr2 = getTileData(secondTileId, m_currAddr.fineY, m_backgroundHalf);

            m_th.writeLower((tr1.lower << 8) | tr2.lower);
            m_th.writeUpper((tr1.upper << 8) | tr2.upper);
        }
    } // end scanline -1
    // visible scanline section
    else if( m_scanline >= 0 && m_scanline <= 239)
    {
        if(m_mask.showBackground)
        {
            if(m_scanline == 0 && m_cycle == 0 && m_isOddFrame)
                m_cycle = 1;

            else if((m_cycle >= 1 && m_cycle <= 256) || (m_cycle >= 321 && m_cycle <= 337))
            {
                uint8_t r = m_cycle % 8;
                if(r == 2)
                {
                    m_nextTileId = readVideoMem(m_currAddr.getVramAddress());
                }

                else if(r == 4)
                {
                    uint16_t addr = m_currAddr.baseNameTable | 0x3c0 | (m_currAddr.tileX >> 2) | ((m_currAddr.tileY >> 2) << 3);

                    uint8_t attrData = readVideoMem(addr);

                    if(m_currAddr.tileY & 0x02)
                        attrData >>= 4;
                    if( m_currAddr.tileX & 0x02)
                        attrData >>= 2;
                    attrData &= 0x03;
                    m_nextAttribData = attrData;
                    m_paletteIdx = attrData;
                    m_paletteBaseAddr = 0x3F00 + (m_paletteIdx << 2);
                }
                // performcne improvement, read tile data at once
                //else if( m_cycle % 8 == 6:
                //    self.next_tile_low, u1 = self.cardridge.get_tile_data(self.next_tile_id, m_currAddr.fine_y,
                //                                                          self.background_half)

                else if(r == 0)
                {
                    m_nextTileData = getTileData(m_nextTileId, m_currAddr.fineY, m_backgroundHalf);
                    m_currAddr.incrementTileX();
                }
                else if( r == 1 && m_cycle > 1)
                {
                    m_th.writeLowerL(m_nextTileData.lower);
                    m_th.writeUpperL(m_nextTileData.upper);

                    m_nextAttribDataH.writeLowerL((m_nextAttribData & 0x1) ? 0xFF : 0x00);
                    m_nextAttribDataH.writeUpperL((m_nextAttribData & 0x2) ? 0xFF : 0x00);
                }
            }
            if(m_cycle >=1 && m_cycle <= 336)
            {
                m_bgPixel = m_th.shiftBitSelect(m_fineX);
                m_paletteIdx = m_nextAttribDataH.shiftBitSelect(m_fineX);
                if(m_cycle >=1 && m_cycle <= 256)
                {
                    uint8_t idx = (readVideoMem(0x3F00 + (m_paletteIdx << 2) + m_bgPixel)) & 0x3f;

                    m_frameData[(m_cycle - 1) + m_scanline*256] = m_palette[idx];
                }
            }
            if(m_cycle == 256)
                m_currAddr.incrementTileY();
            else if( m_cycle == 257)
            {
                if((m_tmpAddr.baseNameTable & 0x400) == 0x400)
                    m_currAddr.baseNameTable |= 0x400;
                else
                    m_currAddr.baseNameTable &= ~0x400;

                m_currAddr.tileX = m_tmpAddr.tileX;
            }
        }

        // ------------------------------sprite rendering------------------------------
        if(m_mask.showSprites)
        {
            if(m_cycle>=0 && m_cycle <= 255)
                decrementSpriteXCounters();
            else if( m_cycle == 1)
                clearSecondaryOam();
            else if( m_cycle == 256)
                fillSecondaryOam(m_scanline);
            else if( m_cycle == 257)
                fillSpritesShiftRegisters(m_scanline);
        }
    }
    else if(m_scanline == 240 && m_cycle == 0)
        m_frameCnt += 1;
    else if(m_scanline == 241 && m_cycle == 1)
    {
        if(m_vblankRead == false)
        {
            if(m_vblankFlagRead== false)
                m_status.verticalBlank = true;
            if(m_ctrl.generateNmi)
            {
                m_raiseNmi = true;
            }
        }

        /*
        if(m_ctrl.generateNmi)
            m_raiseNmi = true;
        if(m_vblankRead == false)
            m_status.verticalBlank = true;
        */
    }

    m_cycle += 1;

    if(m_mask.showBackground || m_mask.showSprites)
    {
        if(m_cycle == 260 && m_scanline < 240 && m_ctrl.backgroundPatternTableAddress == 0x0000)
        {
            m_cartridge.scanline();
        }
        else if(m_cycle == 324 && m_scanline < 240 && m_ctrl.backgroundPatternTableAddress == 0x1000)
        {
            m_cartridge.scanline();
        }
    }

    if(m_cycle == 341) {
        m_cycle = 0;
        m_scanline += 1;
        if(m_scanline == 261) {
            m_scanline = -1;
            m_isOddFrame = !m_isOddFrame;
            ++m_frameNum;
            m_frameUpdate(m_frameData.data());
        }
    }
}

void Ppu::debug()
{
    std::cout << "NameTable 0\r\n";
    uint16_t i = 0;
    std::string s;
    for(auto& e : m_nt0)
    {
        std::stringstream ss;
        ss << std::hex << int(e);

        s += (ss.str() + "  ");
        i+=1;
        if(i==32)
        {
            std::cout << s << "\r\n";
            s = "";
            i=0;
        }
    }
    std::cout << "NameTable 1\r\n";
    i = 0;
    s = "";
    for(auto& e : m_nt1)
    {
        std::stringstream ss;
        ss << std::hex << int(e);

        s += (ss.str() + "  ");
        i+=1;
        if(i==32)
        {
            std::cout << s << "\r\n";
            s = "";
            i=0;
        }
    }
}

uint8_t reverseBits(uint8_t val)
{
    uint8_t res = 0x00;
    uint8_t pos = 7;
    uint8_t bit = 0x00;
    for(int i=0;i<8;++i)
    {
        bit = val & 0x1;
        res |= bit << pos;
        pos--; 
        val >>= 1;
    }
    return res;
}
