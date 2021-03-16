#ifndef MMU_HPP
#define MMU_HPP

#include "types.hpp"
#include <string>
#include <fstream>

class MMU {
    public:
    // memory location constants
    static const u16 IE = 0xFFFF, IF = 0xFF0F;

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

    private:
    u8 memory[0xFFFF] = {0};
};

#endif // "mmu.hpp" included
