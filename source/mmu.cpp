#include "mmu.hpp"

void MMU::write8(u16 addr, u8 data) {
    memory[addr] = data;

    // any write to the DIV timing register causes it to be reset
    if (addr == DIV) {
        memory[DIV] = 0;
    }
}

void MMU::write16(u16 addr, u16 data) {
    memory[addr] = data & 0x00FF;
    memory[addr + 1] = (data & 0xFF00) >> 8;
}

u8 MMU::read8(u16 addr) {
    return memory[addr];
}

u16 MMU::read16(u16 addr) {
    return (memory[addr + 1] << 8) | memory[addr];
}

u8 &MMU::getRef(u16 addr) {
    return memory[addr];
}

void MMU::loadROM(std::string path) {
    std::ifstream ROM(path, std::ios::binary);
    ROM.read((char *)memory, 0x8000);
    ROM.close();
}

void MMU::updateSpecialRegs() {
    // DIV is reset to 0 if anything is written to it
    u8 div = read8(DIV);
    if (oldDIV != div) {
        div == 0;
    }
    oldDIV = div;

    // if DMA changed then a DMA transfer needs to happen
    u8 dma = read8(DMA);
    if (dma != oldDMA) {
        doDMATransfer();
    }
    oldDMA = dma;
}

void MMU::doDMATransfer() {
    u16 source = read8(DMA) * 100;
    for (int i = 0; i < 160; i++) {
        write8(0xFE00 + i, read8(source + i));
    }
}
