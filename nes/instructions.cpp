#include "include/instructions.h"
#include "include/utils.h"

#include <iostream>

namespace
{
    auto constexpr LOG_WIDTH = 32;
}

Adc::Adc(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Adc::execute()
{
    auto& cpuState = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    uint16_t operand = m_cpu.read(addr);

    uint16_t result = (uint16_t)cpuState.a + operand + (uint16_t)cpuState.sr.c;

    cpuState.sr.n = (result & 0x80) >> 7;

    if((result & 0x00FF) == 0)
        cpuState.sr.z = 1;
    else
        cpuState.sr.z = 0;

    if(result > 255)
        cpuState.sr.c = 1;
    else
        cpuState.sr.c = 0;

    cpuState.sr.v = 0;
    if((cpuState.a & 0x80) == 0 && (operand & 0x80) == 0 && (result & 0x80) > 0)
        cpuState.sr.v = 1;

    if((cpuState.a & 0x80) > 0 && (operand & 0x80) > 0 && (result & 0x80) == 0)
        cpuState.sr.v = 1;

    cpuState.a = result & 0xFF;

    return m_addressMode->cycles();
}

std::string Adc::str()
{
    std::string s = "ADC " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Adc::size()
{
    return m_addressMode->size();
}

And::And(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t And::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(addr);
    
    state.a = state.a & operand;

    if(state.a == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    state.sr.n = (state.a & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string And::str()
{
    std::string s = "AND " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t And::size()
{
    return m_addressMode->size();
}

Asl::Asl(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Asl::execute()
{
    auto& state = m_cpu.getState();
    uint32_t address = m_addressMode->getAddress();
    uint8_t operand = 0x00;
    if(address == 0xA0000)
    {
        operand = state.a;
    }
    else
        operand = m_cpu.read(address);
    uint16_t tmp = (operand << 1);

    state.sr.n = (tmp & 0x80) >> 7;
    if(tmp > 255)
        state.sr.c = 1;
    else
        state.sr.c = 0;

    tmp = tmp & 0xff;

    if(address == 0xA0000)
        state.a = tmp;
    else
        m_cpu.write(address, tmp);

    if(tmp == 0x00)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    return m_addressMode->cycles();
}

std::string Asl::str()
{
    std::string s = "ASL " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Asl::size()
{
    return m_addressMode->size();
}

Bcc::Bcc(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Bcc::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();

    if(state.sr.c == 0)
    {
        state.pc = addr;
        return m_addressMode->cycles();
    }
    return 2;
}

std::string Bcc::str()
{
    std::string s = "BCC " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Bcc::size()
{
    return m_addressMode->size();
}

Bcs::Bcs(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Bcs::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();

    if(state.sr.c == 1)
    {
        state.pc = addr;
        return m_addressMode->cycles();
    }
    return 2;
}

std::string Bcs::str()
{
    std::string s = "BCS " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Bcs::size()
{
    return m_addressMode->size();
}

Beq::Beq(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Beq::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();

    if(state.sr.z == 1)
    {
        state.pc = address;
        return m_addressMode->cycles();
    }
    return 2;
}

std::string Beq::str()
{
    std::string s = "BEQ " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Beq::size()
{
    return m_addressMode->size();
}

Bit::Bit(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Bit::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(addr);

    state.sr.n = (operand >> 7) & 0x1;
    state.sr.v = (operand >> 6) & 0x1;
    if((operand & state.a) == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    return m_addressMode->cycles();
}

std::string Bit::str()
{
    std::string s = "BIT " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Bit::size()
{
    return m_addressMode->size();
}

Bmi::Bmi(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Bmi::execute()
{
    auto& state = m_cpu.getState(); 
    uint16_t addr = m_addressMode->getAddress();
    if(state.sr.n == 1)
    {
        state.pc = addr;
        return m_addressMode->cycles();
    }
    return 2;
}

std::string Bmi::str()
{
    std::string s = "BMI " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Bmi::size()
{
    return m_addressMode->size();
}

Bne::Bne(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Bne::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();

    if(state.sr.z == 0)
    {
        state.pc = address;
        return m_addressMode->cycles();
    }
    return 2;
}

std::string Bne::str()
{
    std::string s = "BNE " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Bne::size()
{
    return m_addressMode->size();
}

Bpl::Bpl(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Bpl::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();

    if(state.sr.n == 0)
    {
        state.pc = addr;
        return m_addressMode->cycles();
    }
    return 2;
}

std::string Bpl::str()
{
    std::string s = "BPL " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Bpl::size()
{
    return m_addressMode->size();
}

Brk::Brk(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Brk::execute()
{
    auto& state = m_cpu.getState();
    state.sr.i = 1;

    uint8_t hh = (state.pc & 0xFF00) >> 8;
    uint8_t ll = state.pc & 0xFF;

    m_cpu.write(state.sp, hh);
    state.sp -=1;
    m_cpu.write(state.sp, ll);
    state.sp -= 1;
    m_cpu.write(state.sp, state.sr.toByte());
    state.sp -= 1;

    ll = m_cpu.read(0xFFFE);
    hh = m_cpu.read(0xFFFF);

    state.pc = (hh << 8) | ll;

    return m_addressMode->cycles();
}

std::string Brk::str()
{
    std::string s = "BRK " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Brk::size()
{
    return m_addressMode->size();
}

Bvc::Bvc(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Bvc::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();
    if(state.sr.v == 0)
    {
        state.pc = address;
        return m_addressMode->cycles();
    }
    return 2;
}

std::string Bvc::str()
{
    std::string s = "BVC " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Bvc::size()
{
    return m_addressMode->size();
}

Bvs::Bvs(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Bvs::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    if(state.sr.v == 1)
    {
        state.pc = addr;
        return m_addressMode->cycles();
    }
    return 2;
}

std::string Bvs::str()
{
    std::string s = "BVS " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Bvs::size()
{
    return m_addressMode->size();
}

Clc::Clc(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Clc::execute()
{
    m_cpu.getState().sr.c = 0;
    return m_addressMode->cycles();
}

std::string Clc::str()
{
    std::string s = "CLC " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Clc::size()
{
    return m_addressMode->size();
}

Cld::Cld(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Cld::execute()
{
    m_cpu.getState().sr.d = 0;
    return m_addressMode->cycles();
}

std::string Cld::str()
{
    std::string s = "CLD " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Cld::size()
{
    return m_addressMode->size();
}

Cli::Cli(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Cli::execute()
{
    m_cpu.getState().sr.i = 0;
    return m_addressMode->cycles();
}

std::string Cli::str()
{
    std::string s = "CLI " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Cli::size()
{
    return m_addressMode->size();
}

Clv::Clv(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Clv::execute()
{
    m_cpu.getState().sr.v = 0;
    return m_addressMode->cycles();
}

std::string Clv::str()
{
    std::string s = "CLV " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Clv::size()
{
    return m_addressMode->size();
}

Cmp::Cmp(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Cmp::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    uint16_t operand = m_cpu.read(addr);

    uint16_t result = (uint16_t)state.a - operand;

    //if(int(state.a) - int(operand) < 0)
    if((result & 0x80) > 0)
        state.sr.n = 1;
    else
        state.sr.n = 0;

    if(state.a == operand)
    {
        state.sr.z = 1;
        state.sr.c = 1;
    }
    else if(state.a < operand)
    {
        state.sr.z = 0;
        state.sr.c = 0;
    }
    else if(state.a > operand)
    {
        state.sr.z = 0;
        state.sr.c = 1;
    }
    return m_addressMode->cycles();
}

std::string Cmp::str()
{
    std::string s = "CMP " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Cmp::size()
{
    return m_addressMode->size();
}

Cpx::Cpx(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Cpx::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    uint16_t operand = m_cpu.read(addr);

    uint16_t result = (uint16_t)state.x - operand;

    //if(int(state.x) - int(operand) < 0)
    if((result & 0x80) > 0)
        state.sr.n = 1;
    else
        state.sr.n = 0;

    if(state.x == operand)
    {
        state.sr.z = 1;
        state.sr.c = 1;
    }
    else if(state.x < operand)
    {
        state.sr.z = 0;
        state.sr.c = 0;
    }
    else if(state.x > operand)
    {
        state.sr.z = 0;
        state.sr.c = 1;
    }

    return m_addressMode->cycles();
}

std::string Cpx::str()
{
    std::string s = "CPX " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Cpx::size()
{
    return m_addressMode->size();
}

Cpy::Cpy(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Cpy::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    uint16_t operand = m_cpu.read(addr);

    uint16_t result = (uint16_t)state.y - operand;

    //if(int(state.y) - int(operand) < 0)
    if((result & 0x80) > 0)
        state.sr.n = 1;
    else
        state.sr.n = 0;

    if(state.y == operand)
    {
        state.sr.z = 1;
        state.sr.c = 1;
    }
    else if(state.y < operand)
    {
        state.sr.z = 0;
        state.sr.c = 0;
    }
    else if(state.y > operand)
    {
        state.sr.z = 0;
        state.sr.c = 1;
    }

    return m_addressMode->cycles();
}

std::string Cpy::str()
{
    std::string s = "CPY " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Cpy::size()
{
    return m_addressMode->size();
}

Dcp::Dcp(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Dcp::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(addr);

    if(operand == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    state.sr.n = (operand & 0x80) >> 7;
    operand -= 1;
    operand &= 0xff;

    uint8_t r = state.a - operand;
    if(r & 0x80 > 0)
        state.sr.n = 1;
    else
        state.sr.n = 0;

    m_cpu.write(addr, operand);
    return m_addressMode->cycles();
}

std::string Dcp::str()
{
    std::string s = "DCP " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Dcp::size()
{
    return m_addressMode->size();
}

Dec::Dec(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Dec::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();
    int operand = m_cpu.read(address);

    operand -= 1;

    if(operand == -1)
        operand = 0xff;

    if(operand == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    state.sr.n = (operand & 0x80) >> 7;

    m_cpu.write(address, operand);
    return m_addressMode->cycles();
}

std::string Dec::str()
{
    std::string s = "DEC " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Dec::size()
{
    return m_addressMode->size();
}

Dex::Dex(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Dex::execute()
{
    auto& state = m_cpu.getState();
    state.x -= 1;
    state.x = state.x & 0xff;
    if(state.x == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    state.sr.n = (state.x & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Dex::str()
{
    std::string s = "DEX";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Dex::size()
{
    return m_addressMode->size();
}

Dey::Dey(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Dey::execute()
{
    auto& state = m_cpu.getState();
    state.y -= 1;
    state.y = state.y & 0xff;
    if(state.y == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    state.sr.n = (state.y & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Dey::str()
{
    std::string s = "DEY";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Dey::size()
{
    return m_addressMode->size();
}

Eor::Eor(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Eor::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(addr);

    state.a = operand ^ state.a;

    if(state.a == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    state.sr.n = (state.a & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Eor::str()
{
    std::string s = "EOR " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Eor::size()
{
    return m_addressMode->size();
}

Inc::Inc(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Inc::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(address);
    operand += 1;
    operand = operand & 0xff;

    if(operand == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    state.sr.n = (operand & 0x80) >> 7;

    m_cpu.write(address, operand);
    return m_addressMode->cycles();
}

std::string Inc::str()
{
    std::string s = "INC " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Inc::size()
{
    return m_addressMode->size();
}

Inx::Inx(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Inx::execute()
{
    auto& state = m_cpu.getState();
    state.x += 1;
    state.x = state.x & 0xff;

    if(state.x == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    state.sr.n = (state.x & 0x80) >> 7;

    return m_addressMode->cycles();
}

std::string Inx::str()
{
    std::string s = "INX " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Inx::size()
{
    return m_addressMode->size();
}

Iny::Iny(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Iny::execute()
{
    auto& state = m_cpu.getState();
    state.y += 1;
    state.y = state.y & 0xff;

    if(state.y == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    state.sr.n = (state.y & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Iny::str()
{
    std::string s = "INY";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Iny::size()
{
    return m_addressMode->size();
}

Isb::Isb(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Isb::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(address);
    int o = hexToSignedInt(m_cpu.read(address));
    o += 1;
    uint8_t old_c = state.sr.c;

    if(operand == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    operand += 1;
    operand &= 0xff;

    m_cpu.write(address, operand);

    uint8_t tmp = operand - state.a;

    if(tmp & 0x80 > 0)
        state.sr.n = 1;
    else
        state.sr.n = 0;

    state.sr.v = 0;

    int signedVal = hexToSignedInt(state.a) - hexToSignedInt(o) - (1 - old_c);
    if(signedVal < 0)
        state.sr.n = 1;
    else
        state.sr.n = 0;

    if(signedVal < -128 || signedVal > 127)
        state.sr.v = 1;

    state.a = signedIntToHex(signedVal);

    return m_addressMode->cycles();
}

std::string Isb::str()
{
    std::string s = "ISB " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Isb::size()
{
    return m_addressMode->size();
}

Jmp::Jmp(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Jmp::execute()
{
    auto& state = m_cpu.getState();
    state.pc = m_addressMode->getAddress();
    return m_addressMode->cycles();
}

std::string Jmp::str()
{
    std::string s = "JMP " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Jmp::size()
{
    return m_addressMode->size();
}

Jsr::Jsr(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Jsr::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();

    m_cpu.push(((state.pc-1) & 0xFF00) >> 8);  // -1 because address mode will move pc to the next instruction
    m_cpu.push((state.pc-1) & 0xFF);

    state.pc = addr;
    return m_addressMode->cycles();
}

std::string Jsr::str()
{
    std::string s = "JSR " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Jsr::size()
{
    return m_addressMode->size();
}

Lax::Lax(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Lax::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(address);
    state.a = operand;
    state.x = operand;

    if(state.a == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    state.sr.n = (state.a & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Lax::str()
{
    std::string s = "LAX " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Lax::size()
{
    return m_addressMode->size();
}

Lda::Lda(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Lda::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();
    state.a = m_cpu.read(address);

    if(state.a == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    state.sr.n = (state.a & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Lda::str()
{
    std::string s = "LDA " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Lda::size()
{
    return m_addressMode->size();
}

Ldx::Ldx(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Ldx::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(addr);
    state.x = operand;

    if(state.x == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    state.sr.n = (state.x & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Ldx::str()
{
    std::string s = "LDX " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Ldx::size()
{
    return m_addressMode->size();
}

Ldy::Ldy(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Ldy::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(address);
    state.y = operand;

    if(state.y == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    state.sr.n = (state.y & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Ldy::str()
{
    std::string s = "LDY " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Ldy::size()
{
    return m_addressMode->size();
}

Lsr::Lsr(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Lsr::execute()
{
    auto& state = m_cpu.getState();
    uint32_t address = m_addressMode->getAddress();
    uint8_t operand = 0;
    if(address == 0xA0000)
        operand = state.a;
    else
        operand = m_cpu.read(address);

    state.sr.c = operand & 0x0001;
    uint8_t tmp = operand >> 1;

    if(address == 0xA0000)
        state.a = tmp;
    else
        m_cpu.write(address, tmp);

    state.sr.n = 0;

    if(tmp == 0x00)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    return m_addressMode->cycles();
}

std::string Lsr::str()
{
    std::string s = "LSR " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Lsr::size()
{
    return m_addressMode->size();
}

Nop::Nop(std::unique_ptr<AddressMode> addressMode)
: m_addressMode{std::move(addressMode)}
{

}

uint8_t Nop::execute()
{
    m_addressMode->getAddress();
    return m_addressMode->cycles();
}

std::string Nop::str()
{
    std::string s = "NOP";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Nop::size()
{
    return m_addressMode->size();
}

Ora::Ora(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Ora::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(addr);

    state.a = state.a | operand;

    state.sr.n = (state.a & 0x80) >> 7;

    if(state.a == 0x00)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    return m_addressMode->cycles();
}

std::string Ora::str()
{
    std::string s = "ORA " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Ora::size()
{
    return m_addressMode->size();
}

Pha::Pha(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Pha::execute()
{
    auto& state = m_cpu.getState();
    m_cpu.push(state.a);
    return m_addressMode->cycles();
}

std::string Pha::str()
{
    std::string s = "PHA " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Pha::size()
{
    return m_addressMode->size();
}

Php::Php(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Php::execute()
{
    auto& state = m_cpu.getState();
    uint8_t val = state.sr.toByte() | (1 << 5) | (1 << 4);
    m_cpu.push(val);
    return m_addressMode->cycles();
}

std::string Php::str()
{
    std::string s = "PHP";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Php::size()
{
    return m_addressMode->size();
}

Pla::Pla(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Pla::execute()
{
    auto& state = m_cpu.getState();
    state.a = m_cpu.pop();

    state.sr.n = (state.a & 0x80) >> 7;

    if(state.a == 0x00)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    return m_addressMode->cycles();
}

std::string Pla::str()
{
    std::string s = "PLA " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Pla::size()
{
    return m_addressMode->size();
}

Plp::Plp(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Plp::execute()
{
    auto& state = m_cpu.getState();
    uint8_t val = m_cpu.pop() & 0xcf;
    state.sr.fromByte(val);
    return m_addressMode->cycles();
}

std::string Plp::str()
{
    std::string s = "PLP";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Plp::size()
{
    return m_addressMode->size();
}

Rla::Rla(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Rla::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(address);

    uint8_t old_c = state.sr.c;
    state.sr.c = (operand & 0x80) >> 7;
    operand = ((operand << 1) + old_c) & 0xff;

    m_cpu.write(address, operand);
    state.a = state.a & operand;

    if(state.a == 0)
        state.sr.z = 1;

    state.sr.n = (state.a & 0x80) >> 7;

    return m_addressMode->cycles();
}

std::string Rla::str()
{
    std::string s = "RLA " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Rla::size()
{
    return m_addressMode->size();
}

Rol::Rol(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Rol::execute()
{
    auto& state = m_cpu.getState();
    uint32_t address = m_addressMode->getAddress();
    uint8_t operand = 0;
    if(address == 0xa0000)
        operand = state.a;
    else
        operand = m_cpu.read(address);

    uint8_t old_c = state.sr.c;
    state.sr.c = (operand & 0x80) >> 7;
    operand = ((operand << 1) + old_c) & 0xff;

    if(address == 0xa0000)
        state.a = operand;
    else
        m_cpu.write(address, operand);

    if(operand == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    state.sr.n = (operand & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Rol::str()
{
    std::string s = "ROL " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Rol::size()
{
    return m_addressMode->size();
}

Ror::Ror(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Ror::execute()
{
    auto& state = m_cpu.getState();
    uint32_t address = m_addressMode->getAddress();
    uint8_t operand = 0;
    if(address == 0xa0000)
        operand = state.a;
    else
        operand = m_cpu.read(address);

    uint8_t old_c = state.sr.c;
    state.sr.c = operand & 0x01;
    operand = (operand >> 1) | (old_c << 7);

    if(address == 0xa0000)
        state.a = operand;
    else
        m_cpu.write(address, operand);

    if(operand == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    state.sr.n = (operand & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Ror::str()
{
    std::string s = "ROR " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Ror::size()
{
    return m_addressMode->size();
}

Rra::Rra(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Rra::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(address);
    uint8_t old_c = state.sr.c;
    uint8_t tmp = (operand >> 1) | (old_c << 7);

    if(old_c == 1 && ((operand & 0x01) == 1))
        old_c = 0;

    m_cpu.write(address, tmp);

    if((operand & 0x80 == 0 && tmp & 0x80 > 0) || (operand & 0x80 > 0 && tmp & 0x80 == 0))
        state.sr.c = 1;

    uint8_t result = hexToSignedInt(state.a) + hexToSignedInt(tmp) + (1 - old_c);

    if(result < 0)
        state.sr.n = 1;
    else
        state.sr.n = 0;

    if(result & 0x00FF == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    state.sr.v = 0;
    if(result < -128 || result > 127)
        state.sr.v = 1;

    state.a = result & 0xFF;

    return m_addressMode->cycles();
}

std::string Rra::str()
{
    std::string s = "RRA " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Rra::size()
{
    return m_addressMode->size();
}

Rti::Rti(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Rti::execute()
{
    auto& state = m_cpu.getState();
    uint8_t val = m_cpu.pop();

    if((val & 0x10) == 0x10)
        val = val & 0xef;
    else
        val = val | (1 << 5);

    if((val & 0x20) == 0x20)
        val = val & 0xdf;
    else
        val = val | (1 << 6);

    state.sr.fromByte(val);
    uint8_t ll = m_cpu.pop();
    uint8_t hh = m_cpu.pop();
    state.pc = (hh << 8) | ll;
    return m_addressMode->cycles();
}

std::string Rti::str()
{
    std::string s = "RTI";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Rti::size()
{
    return m_addressMode->size();
}

Rts::Rts(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Rts::execute()
{
    auto& state = m_cpu.getState();
    uint8_t ll = m_cpu.pop();
    uint8_t hh = m_cpu.pop();
    state.pc = (hh << 8) | ll;
    state.pc += 1;

    return m_addressMode->cycles();
}

std::string Rts::str()
{
    std::string s = "RTS";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Rts::size()
{
    return m_addressMode->size();
}

Sax::Sax(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Sax::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(addr);
    uint8_t tmp = state.a & state.x;
    m_cpu.write(addr, tmp);

    return m_addressMode->cycles();
}

std::string Sax::str()
{
    std::string s = "SAX " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Sax::size()
{
    return m_addressMode->size();
}

Sbc::Sbc(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Sbc::execute()
{
    auto& state = m_cpu.getState();
    // A - M - (1 - C)
    uint16_t address = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(address);

    uint8_t tmp = signedIntToHex((int)state.a - (int)operand - (1 - (int)state.sr.c));
    uint8_t res = signedIntToHex((int)state.a - (int)operand - (1 - (int)state.sr.c));

    //std::cout << "A:" << toHex(state.a,2) << " oper:" << toHex(operand,2) << " res:" << toHex(res,2) << "  c:" << toHex(state.sr.c,2) << std::endl;


    if(((state.a & 0x80) == 0) && ((operand & 0x80) > 0))
        tmp -= 1;

    if(res == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    state.sr.n = (tmp & 0x80) >> 7;

    state.sr.v = 0;

    int sig_a = hexToSignedInt(state.a);
    int sig_o = hexToSignedInt(operand);

    if((state.a > operand) || (state.a == operand && state.sr.c == 1))
        state.sr.c = 1;
    else if(state.a < operand)
        state.sr.c = 0;
    else if(state.a == operand && state.sr.c == 0)
        state.sr.c = 0;

    int u = sig_a - sig_o;
    if(u > 127 || u < -128)
        state.sr.v = 1;

    state.a = res;

    return m_addressMode->cycles();
}

std::string Sbc::str()
{
    std::string s = "SBC " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Sbc::size()
{
    return m_addressMode->size();
}

Sec::Sec(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Sec::execute()
{
    auto& state = m_cpu.getState();
    state.sr.c = 1;
    return m_addressMode->cycles();
}

std::string Sec::str()
{
    std::string s = "SEC " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Sec::size()
{
    return m_addressMode->size();
}

Sed::Sed(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Sed::execute()
{
    auto& state = m_cpu.getState();
    state.sr.d = 1;
    return m_addressMode->cycles();
}

std::string Sed::str()
{
    std::string s = "SED";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Sed::size()
{
    return m_addressMode->size();
}

Sei::Sei(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Sei::execute()
{
    auto& state = m_cpu.getState();
    state.sr.i = 1;
    return m_addressMode->cycles();
}

std::string Sei::str()
{
    std::string s = "SEI";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Sei::size()
{
    return m_addressMode->size();
}

Slo::Slo(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Slo::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(address);

    uint8_t old_c = state.sr.c;
    state.sr.c = (operand & 0x80) >> 7;
    operand = ((operand << 1)) & 0xff;

    m_cpu.write(address, operand);

    state.a = state.a | operand;

    state.sr.n = (state.a & 0x80) >> 7;

    if(state.a == 0x00)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    return m_addressMode->cycles();
}

std::string Slo::str()
{
    std::string s = "SLO " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Slo::size()
{
    return m_addressMode->size();
}

Sre::Sre(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Sre::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();
    uint8_t operand = m_cpu.read(address);

    uint8_t old_c = state.sr.c;
    state.sr.c = operand & 0x01;
    operand = (operand >> 1);

    m_cpu.write(address, operand);

    state.a = operand ^ state.a;

    if(state.a == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;
    state.sr.n = (state.a & 0x80) >> 7;

    return m_addressMode->cycles();
}

std::string Sre::str()
{
    std::string s = "SRE " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Sre::size()
{
    return m_addressMode->size();
}

Sta::Sta(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Sta::execute()
{
    auto& state = m_cpu.getState();
    uint16_t address = m_addressMode->getAddress();
    m_cpu.write(address, state.a);
    return m_addressMode->cycles();
}

std::string Sta::str()
{
    std::string s = "STA " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Sta::size()
{
    return m_addressMode->size();
}

Stx::Stx(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Stx::execute()
{
    auto& state = m_cpu.getState();
    uint16_t absAddr = m_addressMode->getAddress();

    m_cpu.write(absAddr, state.x);
    return m_addressMode->cycles();
}

std::string Stx::str()
{
    std::string s = "STX " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Stx::size()
{
    return m_addressMode->size();
}

Sty::Sty(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Sty::execute()
{
    auto& state = m_cpu.getState();
    uint16_t addr = m_addressMode->getAddress();
    m_cpu.write(addr, state.y);
    return m_addressMode->cycles();
}

std::string Sty::str()
{
    std::string s = "STY " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Sty::size()
{
    return m_addressMode->size();
}

Tax::Tax(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Tax::execute()
{
    auto& state = m_cpu.getState();
    state.x = state.a;

    if(state.x == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    state.sr.n = (state.x & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Tax::str()
{
    std::string s = "TAX " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Tax::size()
{
    return m_addressMode->size();
}

Tay::Tay(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Tay::execute()
{
    auto& state = m_cpu.getState();
    state.y = state.a;

    if(state.y == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    state.sr.n = (state.y & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Tay::str()
{
    std::string s = "TAY " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Tay::size()
{
    return m_addressMode->size();
}

Tsx::Tsx(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Tsx::execute()
{
    auto& state = m_cpu.getState();
    state.x = state.sp;

    if(state.x == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    state.sr.n = (state.x & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Tsx::str()
{
    std::string s = "TSX " + m_addressMode->str();
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Tsx::size()
{
    return m_addressMode->size();
}

Txa::Txa(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Txa::execute()
{
    auto& state = m_cpu.getState();
    state.a = state.x;

    if(state.a == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    state.sr.n = (state.a & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Txa::str()
{
    std::string s = "TXA";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Txa::size()
{
    return m_addressMode->size();
}

Txs::Txs(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Txs::execute()
{
    auto& state = m_cpu.getState();
    state.sp = state.x;
    return m_addressMode->cycles();
}

std::string Txs::str()
{
    std::string s = "TXS";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Txs::size()
{
    return m_addressMode->size();
}

Tya::Tya(Cpu& cpu, std::unique_ptr<AddressMode> addressMode)
: m_cpu{cpu}
, m_addressMode{std::move(addressMode)}
{

}

uint8_t Tya::execute()
{
    auto& state = m_cpu.getState();
    state.a = state.y;

    if(state.a == 0)
        state.sr.z = 1;
    else
        state.sr.z = 0;

    state.sr.n = (state.a & 0x80) >> 7;
    return m_addressMode->cycles();
}

std::string Tya::str()
{
    std::string s = "TYA";
    return s + std::string(LOG_WIDTH - s.size(), ' ');
}

uint8_t Tya::size()
{
    return m_addressMode->size();
}


