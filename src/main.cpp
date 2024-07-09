#include <iostream>
#include <ostream>
#include <vector>
#include "util.hpp"
#include "nn.hpp"
#include "board.hpp"

void visualise_bitboard(uint64_t m) {
    Board b;
    b.occupied = m;
    b.color = 0;
    std::cout << b << std::endl;
}

int main() {
    Board b;

    bootstrap_win32_unicode();

    std::cout << b << std::endl;
    b.move(5, 4, WHITE);
    std::cout << b << std::endl;
    b.move(6, 4, BLACK);
    std::cout << b << std::endl;
    visualise_bitboard(b.valid_moves(WHITE));

    // NN nn(&std::vector<int> { 4, 2, 3 }, 0.01);
    // Vector v(4);
    // v << 1, 2, 3, 4;
    // std::cout << nn.apply(v) << std::endl;
}

