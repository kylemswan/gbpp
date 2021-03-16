#include "emulator.hpp"
#include <iostream>

#include "utils.hpp"
#include "types.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./cppboy <ROM>\n";
        return -1;
    }

    Emulator gameboy(argv[1]);
    gameboy.run();

    return 0;
}
