#include <ios>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "input.hpp"
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

Board::WinState play_nn(Board &board, NN &nn, bool nn_color) {
    bool passed = false;
    bool to_move = BLACK;
    uint64_t moves;

    for (int i = 0; i < 64; i++) {
        moves = board.valid_moves(to_move);
        if (moves == 0) {
            if (passed) { // double pass indicates no moves left!
                break;
            }
            passed = true;
        } else {
            if (to_move == nn_color) {
                Vector result = nn.apply(NNBatch::vectorize(board, to_move));
                int play_index = NNBatch::max_index(result, moves);
                board.move(play_index, to_move);
            } else {
                int position;
                char x, y;
                do {
                    std::cin >> x >> y;
                    position = XY(x - 'a', y - '1');
                } while ((OFFSET(position) & moves) == 0);
                board.move(position, to_move);
            }
            passed = false;
        }

        // std::cout << "move " << i << "\n" << board << std::endl;

        to_move = !to_move;
    }

    return board.win_state();
}

int main() {
    Board board;
    bool to_move;
    auto arms = CommandArm {
        CommandArm { "move", {
            CommandArm { "nn", [&](std::vector<std::string>& args) {

            } },
            CommandArm { "sq", [&](std::vector<std::string>& args) {
                int moves = board.valid_moves(to_move);
                int position;
                if (args[1].size() != 2) {
                    std::cout << "invalid square" << std::endl;
                    return;
                }
                position = XY(args[1][0] - 'a', args[1][1] - '1');
                board.move(position, to_move);
            } }
        } }
    };

    while (true) {
        std::cout << "> " << std::flush;
        std::string s;
        std::getline(std::cin, s);
        if (!arms.process(s)) {
            std::cout << "no action taken" << std::endl;
        }
    }

    bootstrap_win32_unicode();

    // std::cout << b << std::endl;
    // b.move(5, 4, WHITE);
    // std::cout << b << std::endl;
    // b.move(6, 4, BLACK);
    // std::cout << b << std::endl;
    // visualise_bitboard(b.valid_moves(WHITE));
//
    // NNBatch batch{100, 50, 25};
//
    // constexpr int n = 1000;
    // for (int i = 0; i < n; i++) {
        // if (i%(n/10) == 0) std::cout << "generation " << i << std::endl;
        // batch.play_generation((n - i)/(n/10) + 1);
    // }
//
    // NN nn_black = batch.nns[0];
//
    // std::ofstream o("./model.mnn");
    // o << nn_black;
    // o.close();
//
    // int wins = 0;
    // for (int i = 0; i < 100; i++) {
        // b.reset();
        // NN nn_white({16, 16}, 0);
        // if (NNBatch::play_game(b, nn_black, nn_white) == Board::BlackWins) wins++;
    // }
    // std::cout << wins << std::endl;
    // NN nn(&std::vector<int> { 4, 2, 3 }, 0.01);
    // Vector v(4);
    // v << 1, 2, 3, 4;
    // std::cout << nn.apply(v) << std::endl;
}

