#ifndef CPU_HPP
#define CPU_HPP

#include "mmu.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <sstream>

class CPU {
    public:
    void reset();
    void bindMMU(MMU *target);
    void run();

    // return a formatted debug string 
    std::string getState();

    int cycles;

    // interrupt handling logic
    bool IME = true;
    bool halt = true;
    void callIntVector(u16 addr);

    private:
    u8 A, B, C, D, E, H, L;
    u16 SP, PC;

    MMU *mmu;

    bool branched = false;

    // CPU flags and shortcuts to set multiple at once
    bool flagZ, flagN, flagH, flagC;
    void setZNHC(bool fZ, bool fN, bool fH, bool fC);
    void setZNH(bool fZ, bool fN, bool fH);
    void setNHC(bool fN, bool fH, bool fC);

    // dispatch functions
    void exec(u8 op);
    void execCB(u8 op);

    // loads and move instructions
    void LD(u8 &target, u8 val);
    void LDaddrsp(u16 addr);
    void LDhl(u16 val);
    void LDrr(u8 &hi, u8 &lo, u16 val);
    void LDsp(u16 val);

    void LDD(u8 &target, u8 val);
    void LDI(u8 &target, u8 val);
    
    void PUSH(u8 hi, u8 lo);
    void PUSHaf();

    void POP(u8 &hi, u8 &lo);
    void POPaf();

    // logic instructions
    void AND(u8 val);
    void CCF();
    void CP(u8 val);
    void CPL();
    void DAA();
    void OR(u8 val);
    void SCF();
    void XOR(u8 val);

    // arithmetic instructions
    void ADC(u8 val);
    void ADD(u8 val);
    void ADDhl(u16 val);
    void ADDsp(s8 val);

    void DEC(u8 &target);
    void DECrr(u8 &hi, u8 &lo);
    void DECsp();

    void INC(u8 &target);
    void INCrr(u8 &hi, u8 &lo);
    void INCsp();

    void SBC(u8 val);
    void SUB(u8 val);

    // jump and return instructions
    void CALL(u16 addr);
    void CALLcond(u16 addr, bool cond);

    void JP(u16 addr);
    void JPcond(u16 addr, bool cond);

    void JR(s8 val);
    void JRcond(s8 val, bool cond);

    void RET();
    void RETcond(bool cond);
    void RETI();

    // bit rotating and shifting instructions
    void RL(u8 &target, bool circular);
    void RLa(bool circular);
    void RR(u8 &target, bool circular);
    void RRa(bool circular);

    void SLA(u8 &target);
    void SRA(u8 &target);
    void SRL(u8 &target);

    // bit setting and clearing
    void BIT(int bit, u8 val);
    void RES(int bit, u8 &target);
    void SET(int bit, u8 &target);
    void SWAP(u8 &target);

    // control instructions
    void DI();
    void EI();
    void HALT();
    void NOP();
    void RST(u16 addr);
    void STOP();

    // special function to catch unimplemented or missed opcodes
    void XXX(u8 op);

    // tables for fetching PC offsets and cycle count based on the op performed
    // (conditional jumps and calls may add extra cycles if a branch is taken)
    int getCycleCount(u8 op);
    int getPCOffset(u8 op);

    int pcOffset[0x100] = {
         1,  3,  1,  1,  1,  1,  2,  1,  3,  1,  1,  1,  1,  1,  2,  1,
         2,  3,  1,  1,  1,  1,  2,  1,  2,  1,  1,  1,  1,  1,  2,  1,
         2,  3,  1,  1,  1,  1,  2,  1,  2,  1,  1,  1,  1,  1,  2,  1,
         2,  3,  1,  1,  1,  1,  2,  1,  2,  1,  1,  1,  1,  1,  2,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  3,  3,  3,  1,  2,  0,  1,  1,  3,  0,  3,  3,  2,  0,
         1,  1,  3,  1,  3,  1,  2,  0,  1,  1,  3,  1,  3,  1,  2,  0,
         2,  1,  2,  1,  1,  1,  2,  0,  2,  1,  3,  1,  1,  1,  2,  0,
         2,  1,  2,  1,  1,  1,  2,  0,  2,  1,  3,  1,  1,  1,  2,  0
    };

    int cycleCount[0x100] = {
         4, 12,  8,  8,  4,  4,  8,  4, 20,  8,  8,  8,  4,  4,  8,  4,
         4, 12,  8,  8,  4,  4,  8,  4, 12,  8,  8,  8,  4,  4,  8,  4,
         8, 12,  8,  8,  4,  4,  8,  4,  8,  8,  8,  8,  4,  4,  8,  4,
         8, 12,  8,  8, 12, 12, 12,  4,  8,  8,  8,  8,  4,  4,  8,  4,
         4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
         4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
         4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
         8,  8,  8,  8,  8,  8,  4,  8,  4,  4,  4,  4,  4,  4,  8,  4,
         4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
         4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
         4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
         4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
         8, 12, 12, 16, 12, 16,  8, 16,  8, 16, 12,  0, 12, 24,  8, 16,
         8, 12, 12,  0, 12, 16,  8, 16,  8, 16, 12,  0, 12,  0,  8, 16,
        12, 12,  8,  0,  0, 16,  8, 16, 16,  4, 16,  0,  0,  0,  8, 16,
        12, 12,  8,  4,  0, 16,  8, 16, 12,  8, 16,  4,  0,  0,  8, 16
    };
};

#endif // "cpu.hpp" included
