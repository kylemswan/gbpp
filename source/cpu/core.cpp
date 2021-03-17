#include "cpu.hpp"

void CPU::reset() {
    // state of the CPU after the internal boot sequence runs
    A = 0x01;
    B = 0x00;
    C = 0x13;
    D = 0x00;
    E = 0xD8;
    H = 0x01;
    L = 0x4D;
    SP = 0xFFFE;
    PC = 0x0100;
    setZNHC(true, false, true, true);
}

void CPU::bindMMU(MMU *target) {
    mmu = target;
}

int CPU::run() {
    extraCycles = 0;
    u8 op = mmu->read8(PC);

    // check for 0xCB prefixed opcodes
    if (op == 0xCB) {
        u8 cb = mmu->read8(PC + 1);
        execCB(cb);
    } else {
        exec(op);
    }

    if (!branched) {
        PC += getPCOffset(op);
    }
    branched = false;

    // return cycles taken to be used by the timers
    return getCycleCount(op) + extraCycles;
}

std::string CPU::getState() {
    u8 op = mmu->read8(PC);
    u8 D8 = mmu->read8(PC + 1);
    u16 D16 = mmu->read16(PC + 1);

    // construct formatted string displaying CPU information
    std::stringstream s;
    s << "OP " << Utils::formatHex(op, 2) << "\n"
      << "A  " << Utils::formatHex(A, 2) << "\n"
      << "B  " << Utils::formatHex(B, 2) << "\n"
      << "C  " << Utils::formatHex(C, 2) << "\n"
      << "D  " << Utils::formatHex(D, 2) << "\n"
      << "E  " << Utils::formatHex(E, 2) << "\n"
      << "H  " << Utils::formatHex(H, 2) << "\n"
      << "L  " << Utils::formatHex(L, 2) << "\n"
      << "F  " << flagZ << flagN << flagH << flagC << "\n"
      << "PC " << Utils::formatHex(PC, 4) << "\n"
      << "SP " << Utils::formatHex(SP, 4) << "\n\n"
      << "D8   " << Utils::formatHex(D8, 2) << "\n"
      << "D16  " << Utils::formatHex(D16, 4) << "\n";
    return s.str();
}

void CPU::setZNHC(bool fZ, bool fN, bool fH, bool fC) {
    setZNH(fZ, fN, fH);
    flagC = fC;
}

void CPU::setZNH(bool fZ, bool fN, bool fH) {
    flagZ = fZ;
    flagN = fN;
    flagH = fH;
}

void CPU::setNHC(bool fN, bool fH, bool fC) {
    flagN = fN;
    flagH = fH;
    flagC = fC;
}

int CPU::getPCOffset(u8 op) {
    if (op == 0xCB) {
        return 2;
    } else {
        return pcOffset[op];
    }
}

int CPU::getCycleCount(u8 op) {
    if (op == 0xCB) {
        // CB opcodes that end in '6' or 'E' take 16 cycles, all others take 8
        u8 cb = mmu->read8(PC + 1);
        u8 n2 = cb & 0x0F;
        return (n2 == 0x06) || (n2 == 0x0E) ? 16 : 8;
    } else {
        return cycleCount[op];
    }
}

void CPU::callIntVector(u16 addr) {
    // call vector, reset IME, and start running the CPU again if it was halted
    CALL(addr);
    IME = false;
    halt = false;
}
