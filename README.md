# GBpp

GBpp is a GameBoy emulator written in C++, using the *[SMFL](https://www.sfml-dev.org/)* library for windowing and IO. It is currently a work in progress - all of the LR35902 CPU (Z80-like) instructions are implemented and cycle accurate but are largely untested. The memory management unit can only handle loading 32kB ROMs (such as *Tetris*, which I have been using for debugging) since I am yet to handle the GameBoy's dynamic ROM banking system.

From here, I need to cover the graphics, interrupts, audio, taking input from the keyboard, and many more subtleties in the memory management unit. Ultimately I'd like to achieve stable emulation of all (or at least most!) GameBoy ROMs, including support for the slightly upgraded GameBoy Color version.