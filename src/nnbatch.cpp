#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include "nnbatch.hpp"
#include "nn.hpp"

// template <unsigned int N>
// std::array<int, N> NNBatch<N>::rankings() {
    // std::array<int, N> scores;
    // for (int a = 0; a < N; a++) {
        // for (int b = 0; b < N; b++) {
            // 
        // }
    // }
    // return scores;
// }

int map8x8(int miniboard) {
    return miniboard + 18 + (std::floor(miniboard/4) * 4);
}

int map4x4(int board) {
    return board - 18 - std::floor((board - 18) / 8) * 4;
}

Vector vectorize(Board& board, bool pos_color) {
    Vector result(16);
    for (int i = 0; i < 16; i++) {
        result[i] = (BIT(board.occupied, map8x8(i)) ?
            0 :
            BIT(board.color, i) ^ pos_color ? // if bit is not the same color (0^0=0, 1^1=1)
                -1 : // set bit when white indicates us
                1); // unset bit when white indicates not us
    }
    return result;
}

unsigned int max_index(Vector& v, uint64_t valid) {
    unsigned int max_index = 0;
    double max = -INFINITY;
    for (unsigned int i = 0; i < 16; i++) {
        if (BIT(valid, map8x8(i)) && v[i] > max) {
            max_index = i;
            max = v[i];
        }
    }
    return max_index;
}

std::mt19937 generation_random{std::random_device()()};

void NNBatch::mutate(NN& nn) {
    for (int i = 0; i < 5; i++) {
        auto mutation = (generation_random() % 2);
        if (mutation == 0) {
            nn.mutate_change_weight();
        } else if (mutation == 1) {
            nn.mutate_change_bias();
        } else if (mutation == 2) {
            // never occurs (x%2 != 2)
            // nn.mutate_change_weight();
        }
    }
}

void NNBatch::play_generation() {
    for (auto& board : boards) {
        board.reset();
    }
    std::shuffle(nns.begin(), nns.end(), generation_random);

    std::vector<int> winning_indeces;
    winning_indeces.reserve(nns.size() / 2); // max is where no draws occur
    std::vector<int> losing_drawing_indeces;
    winning_indeces.reserve(nns.size() / 2);

    for (int i = 0; i < nns.size() - 1; i += 2) {
        switch (play_game(boards[i/2], nns[i], nns[i+1])) {
            case Board::BlackWins:
                winning_indeces.push_back(i);
                losing_drawing_indeces.push_back(i+1);
                break;
            case Board::WhiteWins:
                winning_indeces.push_back(i+1);
                losing_drawing_indeces.push_back(i);
                break;
            case Board::Draw:
                losing_drawing_indeces.push_back(i);
                losing_drawing_indeces.push_back(i+1);
                break;
        }
    }
   
    std::vector<NN> new_nns;
    new_nns.reserve(nns.size());
    for (int i = 0; i < std::min(mutate_count, winning_indeces.size()); i++) {
        NN nn(nns[winning_indeces[i]]);
        mutate(nn);
        new_nns.push_back(std::move(nn));
    }
    for (int i = std::min(copy_count, winning_indeces.size()); i >= 0; i--) {
        new_nns.push_back(nns[winning_indeces[i]]);
    }

    std::uniform_int_distribution<size_t> dist(0, losing_drawing_indeces.size()-1);
    while (new_nns.size() < nns.size()) {
        new_nns.push_back(nns[losing_drawing_indeces[dist(generation_random)]]);
    }

    nns = std::move(new_nns);
}

Board::WinState NNBatch::play_game(Board &board, NN &black, NN &white) {
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
            Vector result = ((to_move == BLACK) ? black : white).apply(vectorize(board, to_move));
            int play_index = map8x8(max_index(result, moves));
            board.move(play_index, to_move);
            passed = false;
        }

        // std::cout << "move " << i << "\n" << board << std::endl;

        to_move = !to_move;
    }

    return board.win_state();
}
