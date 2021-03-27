#pragma once

#include "addressModes.h"
#include "instruction.h"

#include <memory>

class Adc : public Instruction
{
    public:
        Adc(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;
};

class And : public Instruction
{
    public:
        And(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Asl : public Instruction
{
    public:
        Asl(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Bcc : public Instruction
{
    public:
        Bcc(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Bcs : public Instruction
{
    public:
        Bcs(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Beq : public Instruction
{
    public:
        Beq(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Bit : public Instruction
{
    public:
        Bit(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Bmi : public Instruction
{
    public:
        Bmi(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Bne : public Instruction
{
    public:
        Bne(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Bpl : public Instruction
{
    public:
        Bpl(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Brk : public Instruction
{
    public:
        Brk(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Bvc : public Instruction
{
    public:
        Bvc(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Bvs : public Instruction
{
    public:
        Bvs(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Clc : public Instruction
{
    public:
        Clc(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Cld : public Instruction
{
    public:
        Cld(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Cli : public Instruction
{
    public:
        Cli(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Clv : public Instruction
{
    public:
        Clv(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Cmp : public Instruction
{
    public:
        Cmp(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Cpx : public Instruction
{
    public:
        Cpx(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Cpy : public Instruction
{
    public:
        Cpy(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Dcp : public Instruction
{
    public:
        Dcp(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Dec : public Instruction
{
    public:
        Dec(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Dex : public Instruction
{
    public:
        Dex(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Dey : public Instruction
{
    public:
        Dey(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Eor : public Instruction
{
    public:
        Eor(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Inc : public Instruction
{
    public:
        Inc(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Inx : public Instruction
{
    public:
        Inx(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Iny : public Instruction
{
    public:
        Iny(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Isb : public Instruction
{
    public:
        Isb(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Jmp : public Instruction
{
    public:
        Jmp(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Jsr : public Instruction
{
    public:
        Jsr(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Lax : public Instruction
{
    public:
        Lax(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Lda : public Instruction
{
    public:
        Lda(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Ldx : public Instruction
{
    public:
        Ldx(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Ldy : public Instruction
{
    public:
        Ldy(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Lsr : public Instruction
{
    public:
        Lsr(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Nop : public Instruction
{
    public:
        Nop(std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Ora : public Instruction
{
    public:
        Ora(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Pha : public Instruction
{
    public:
        Pha(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Php : public Instruction
{
    public:
        Php(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Pla : public Instruction
{
    public:
        Pla(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Plp : public Instruction
{
    public:
        Plp(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Rla : public Instruction
{
    public:
        Rla(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Rol : public Instruction
{
    public:
        Rol(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Ror : public Instruction
{
    public:
        Ror(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Rra : public Instruction
{
    public:
        Rra(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Rti : public Instruction
{
    public:
        Rti(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Rts : public Instruction
{
    public:
        Rts(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Sax : public Instruction
{
    public:
        Sax(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Sbc : public Instruction
{
    public:
        Sbc(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Sec : public Instruction
{
    public:
        Sec(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Sed : public Instruction
{
    public:
        Sed(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Sei : public Instruction
{
    public:
        Sei(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Slo : public Instruction
{
    public:
        Slo(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Sre : public Instruction
{
    public:
        Sre(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Sta : public Instruction
{
    public:
        Sta(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Stx : public Instruction
{
    public:
        Stx(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Sty : public Instruction
{
    public:
        Sty(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Tax : public Instruction
{
    public:
        Tax(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Tay : public Instruction
{
    public:
        Tay(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Tsx : public Instruction
{
    public:
        Tsx(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Txa : public Instruction
{
    public:
        Txa(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Txs : public Instruction
{
    public:
        Txs(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};

class Tya : public Instruction
{
    public:
        Tya(Cpu& cpu, std::unique_ptr<AddressMode> addressMode);

        uint8_t execute() override;
        std::string str() override;
        uint8_t size() override;

    private:
        Cpu& m_cpu;
        std::unique_ptr<AddressMode> m_addressMode;    
};