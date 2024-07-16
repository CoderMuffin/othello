#include <iostream>
#include <ostream>
#include <vector>
#include "nnbatch.hpp"
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

    NNBatch batch{100, 50, 25};

    constexpr int n = 1000;
    for (int i = 0; i < n; i++) {
        if (i%(n/10) == 0)std::cout << "generation " << i << std::endl;
        batch.play_generation((n-1 - i)/(n/10) + 1);
    }

    NN nn_black = batch.nns[0];

    int wins = 0;
    for (int i = 0; i < 10000; i++) {
        b.reset();
        NN nn_white({16, 16}, 0.01);
        if (NNBatch::play_game(b, nn_black, nn_white) == Board::BlackWins) wins++;
    }
    std::cout << wins << std::endl;
    // NN nn(&std::vector<int> { 4, 2, 3 }, 0.01);
    // Vector v(4);
    // v << 1, 2, 3, 4;
    // std::cout << nn.apply(v) << std::endl;
}

