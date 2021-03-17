#include "emulator.hpp"

Emulator::Emulator(char *romPath) {
    mmu.loadROM(romPath);

    cpu.reset();
    cpu.bindMMU(&mmu);

    win.create(sf::VideoMode(160, 144), "gbpp");
}

void Emulator::run() {
    while (win.isOpen()) {
        // main game logic is updated at 60FPS
        if (timer.getElapsedTime().asSeconds() >= 1.0 / 60) {
            
            // CPU executes 4194304 cycles per second == 69905 per frame
            while (cycles < 69905) {
                cyclesThisLoop = cpu.run();

                updateTimers(cyclesThisLoop);
                handleInterrupts();

                mmu.updateSpecialRegs();

                cycles += cyclesThisLoop;
            }

            cycles = 0;
            timer.restart();
        }

        handleEvents();
    }
}

void Emulator::updateTimers(int cycles) {
    divCounter += cycles;
    // DIV is incremented at a rate of 16384 Hz = every 256th cycle
    if (divCounter >= 256) {
        u8 &DIV = mmu.getRef(MMU::DIV);
        DIV++;
        divCounter = 0;
    }
    // TIMA needs to be incremented by the rate defined in TAC and only if the
    // enable bit is set
    u8 &TAC = mmu.getRef(MMU::TAC);
    if (Utils::getBit(TAC, 2)) {
        tmaCounter += cycles;

        bool B0 = Utils::getBit(TAC, 0);
        bool B1 = Utils::getBit(TAC, 1);
        bool incTIMA = false;
        
        if (!B1 && !B0) {
            // increment every 1024 cycles
            if (tmaCounter >= 1024) {
                incTIMA = true;
                tmaCounter = 0;
            }    
        } else if (!B1 && B0) {
            // increment every 16 cycles
            if (tmaCounter >= 16) {
                incTIMA = true;
                tmaCounter = 0;
            }
        } else if (B1 && !B0) {
            // incrememnt every 64 cycles
            if (tmaCounter >= 64) {
                incTIMA = true;
                tmaCounter = 0;
            }
        } else {
            // increment every 256 cycles
            if (tmaCounter >= 256) {
                incTIMA = true;
                tmaCounter = 0;
            }
        }

        // TIMA imcremented, but if an overflow occurs it is set to the value
        // of TMA and a TIMER interrupt is triggered
        if (incTIMA) {
            u8 &TIMA = mmu.getRef(MMU::TIMA); 
            if (TIMA == 0xFF) {
                // overflow is about to happen
                u8 &TMA = mmu.getRef(MMU::TMA);
                u8 &IF = mmu.getRef(MMU::IF);
                Utils::setBit(IF, 2, true);
                TIMA = TMA;
            } else {
                TIMA++;
            }
        }
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
    // clear all key states first - note that true -> unpressed
    u8 &JOYP = mmu.getRef(MMU::JOYP);
    for (int i = 0; i < 4; i++) {
        Utils::setBit(JOYP, i, true);
    }

    // now handle new events
    while (win.pollEvent(ev)) {
        // check for window 'X' clicks
        if (ev.type == sf::Event::Closed) {
            win.close();
            return;
        }

        // check for key presses
        if (ev.type == sf::Event::KeyPressed) {
            // handle escape button first
            if (ev.key.code == sf::Keyboard::Escape) {
                win.close();
                return;
            }
            // update the joypad register, depending on bit 4 and 5 of JOYP
            if (!Utils::getBit(JOYP, 5)) {
                // update button key states (start, select, B, A)
                switch (ev.key.code) {
                    case sf::Keyboard::Q:   // GB START
                        Utils::setBit(JOYP, 3, false);
                        break;

                    case sf::Keyboard::W:   // GB SELECT
                        Utils::setBit(JOYP, 2, false);
                        break;

                    case sf::Keyboard::A:   // GB B
                        Utils::setBit(JOYP, 1, false);
                        break;

                    case sf::Keyboard::S:   // GB A
                        Utils::setBit(JOYP, 0, false);
                        break;
                }

            } else if (!Utils::getBit(JOYP, 4)) {
                // update d-pad key states (down, up, left, right)
                switch (ev.key.code) {
                    case sf::Keyboard::Down:
                        Utils::setBit(JOYP, 3, false);
                        break;

                    case sf::Keyboard::Up:
                        Utils::setBit(JOYP, 2, false);
                        break;

                    case sf::Keyboard::Left:
                        Utils::setBit(JOYP, 1, false);
                        break;

                    case sf::Keyboard::Right:
                        Utils::setBit(JOYP, 0, false);
                        break;
                }
            }
            // since a keypress occured, request the JOYPAD interrupt
            u8 &IF = mmu.getRef(MMU::IF);
            Utils::setBit(IF, 4, true);
        }
    }
}
