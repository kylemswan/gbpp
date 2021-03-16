#include "emulator.hpp"

Emulator::Emulator(char *romPath) {
    mmu.loadROM(romPath);

    cpu.reset();
    cpu.bindMMU(&mmu);

    win.create(sf::VideoMode(640, 320), "gbpp");
}

void Emulator::run() {
    while (win.isOpen()) {
        // main game logic is updated at 60FPS
        if (timer.getElapsedTime().asSeconds() >= 1.0 / 60) {
            
            // CPU executes 4194304 cycles per second == 69905 per frame
            while (cpu.cycles < 69905) {
                cpu.run();
            }
            cpu.cycles = 0;

            // update timers, draw graphics, handle interrupts, etc... here
            handleInterrupts();

            timer.restart();
        }

        handleEvents();
    }
}

void Emulator::handleInterrupts() {
    // check if there are no pending interrupts or if the master flag (IME)
    // is set to false
    u8 &IE = mmu.getRef(MMU::IE);
    u8 &IF = mmu.getRef(MMU::IF);
    if (!IF || !cpu.IME) {
        return;
    }

    // check for VBLANK interrupt (bit 0)
    if (Utils::getBit(IF, 0) && Utils::getBit(IE, 0)) {
        cpu.callIntVector(0x40);
        Utils::setBit(IF, 0, false);
    } 
    
    // check for LCD interrupt (bit 1)
    else if (Utils::getBit(IF, 1) && Utils::getBit(IE, 1)) {
        cpu.callIntVector(0x48);
        Utils::setBit(IF, 1, false);
    }

    // check for TIMER interrupt (bit 2)
    else if (Utils::getBit(IF, 2) && Utils::getBit(IE, 2)) {
        cpu.callIntVector(0x50);
        Utils::setBit(IF, 2, false);
    }

    // check for SERIAL interrupt (bit 3)
    else if (Utils::getBit(IF, 3) && Utils::getBit(IE, 3)) {
        cpu.callIntVector(0x58);
        Utils::setBit(IF, 3, false);
    }

    // check for JOYPAD interrupt (bit 4)
    else if (Utils::getBit(IF, 4) && Utils::getBit(IE, 4)) {
        cpu.callIntVector(0x60);
        Utils::setBit(IF, 4, false);
    }
}

void Emulator::handleEvents() {
    while (win.pollEvent(ev)) {
        // check for window 'X' clicks
        if (ev.type == sf::Event::Closed) {
            win.close();
        }
        // check for 'ESC' key presses
        if (ev.key.code == sf::Keyboard::Escape) {
            win.close();
        }
    }
}
