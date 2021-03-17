#ifndef MMU_HPP
#define MMU_HPP

#include "types.hpp"
#include <string>
#include <fstream>

class MMU {
    public:
    // memory location constants
    static const u16 
        JOYP = 0xFF00,
        DIV = 0xFF04,
        TIMA = 0xFF05,
        TMA = 0xFF06,
        TAC = 0xFF07,
        IF = 0xFF0F,
        DMA = 0xFF46,
        IE = 0xFFFF;

    // attempt to write / read data into memory - this will eventually
    // contain more logic pertaining to read / write only sections of RAM
    void write8(u16 addr, u8 data);
    void write16(u16 addr, u16 data);

    u8 read8(u16 addr);
    u16 read16(u16 addr);

    // return a reference to the given address - only allow for writable memory
    u8 &getRef(u16 addr);

    // load a ROM into memory - does not support switchable ROM banks yet...
    void loadROM(std::string path);

    void updateSpecialRegs();

    // perform a DMA transfer - RAM -> OAM
    void doDMATransfer();

    private:
    u8 memory[0xFFFF] = {0};

    // store old values of DIV and DMA to see if they need updating
    u8 oldDIV = 0x00;
    u8 oldDMA = 0x00;
};

#endif // "mmu.hpp" included
