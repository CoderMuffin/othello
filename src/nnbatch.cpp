#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
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

Vector NNBatch::vectorize(const Board& board, bool pos_color) {
    Vector result(64);
    for (Eigen::Index i = 0; i < 64; i++) {
        result[i] = (BIT(board.occupied, i) ?
            0.0f :
            BIT(board.color, i) ^ pos_color ? // if bit is not the same color (0^0=0, 1^1=1)
                -1.0f : // set bit when white indicates us
                1.0f); // unset bit when white indicates not us
    }
    return result;
}

unsigned int NNBatch::max_index(const Vector& v, uint64_t valid) {
    unsigned int max_index = 0;
    double max = -INFINITY;
    for (unsigned int i = 0; i < 64; i++) {
        if (BIT(valid, i) && v[i] > max) {
            max_index = i;
            max = v[i];
        }
    }
    return max_index;
}

std::mt19937 generation_random{std::random_device()()};
void NNBatch::mutate(NN& nn, int mutations, float temperature) {
    for (int i = 0; i < mutations; i++) {
        auto mutation = (generation_random() % 2);
        if (mutation == 0) {
            nn.mutate_change_weight(temperature);
        } else if (mutation == 1) {
            nn.mutate_change_bias(temperature);
        }
    }
}
/*
void NNBatch::play_generation(int mutations) {
    for (auto& board : boards) {
        board.reset();
    }
    std::shuffle(nns.begin(), nns.end(), generation_random);

    std::vector<size_t> winning_indeces;
    winning_indeces.reserve(nns.size() / 2); // max is where no draws occur
    std::vector<size_t> losing_drawing_indeces;
    winning_indeces.reserve(nns.size() / 2);

    for (size_t i = 0; i < nns.size() - 1; i += 2) {
        switch (play_game(boards[i / 2], nns[i], nns[i + 1])) {
        case Board::BlackWins:
            winning_indeces.push_back(i);
            losing_drawing_indeces.push_back(i + 1);
            break;
        case Board::WhiteWins:
            winning_indeces.push_back(i + 1);
            losing_drawing_indeces.push_back(i);
            break;
        case Board::Draw:
            losing_drawing_indeces.push_back(i);
            losing_drawing_indeces.push_back(i + 1);
            break;
        }
    }

    std::vector<NN> new_nns;
    new_nns.reserve(nns.size());
    for (size_t i = 0; i < std::min(mutate_count, winning_indeces.size()); i++) {
        NN nn(nns[winning_indeces[i]]);
        mutate(nn, mutations);
        new_nns.push_back(std::move(nn));
    }
    // HACK: changed i >= 0 to i > 0 to make loop stop on time. maybe change from size_t
    for (size_t i = std::min(copy_count, winning_indeces.size()); i > 0; i--) {
        new_nns.push_back(nns[winning_indeces[i]]);
    }

    for (size_t i = 0; i < dummy_count; i++) {
        new_nns.push_back(make_nn());
    }

    std::uniform_int_distribution<size_t> dist(0, losing_drawing_indeces.size() - 1);
    while (new_nns.size() < nns.size()) {
        new_nns.push_back(nns[losing_drawing_indeces[dist(generation_random)]]);
    }

    nns = std::move(new_nns);
}
//*/

constexpr size_t THREADS = 16;

void NNBatch::play_generation(int mutations, float temperature) {
    std::shuffle(nns.begin(), nns.end(), generation_random);

    auto wins = std::make_unique<int[]>(nns.size());
    std::thread workers[THREADS];

    for (size_t worker = 0; worker < THREADS; worker++) {
        workers[worker] = std::thread([this, &wins, worker]() {
            Board board;
            for (size_t i = worker; i < nns.size(); i += THREADS) {
                for (size_t j = 1; j < nns.size(); j++) {
                    switch (play_game(board, nns[i], nns[(i+j)%nns.size()])) {
                        case Board::BlackWins:
                            wins[i]++;
                            break;
                        case Board::WhiteWins:
                            wins[i]--;
                            break;
                        case Board::Draw:
                            break;
                    }
                    board.reset();
                }
            }
        });
    }

    for (auto& worker : workers) {
        worker.join();
    }
   
    std::vector<NN> new_nns;
    new_nns.reserve(nns.size());

    const size_t loser_count = nns.size() - dummy_count - copy_count - mutate_count;

    size_t winners_copied = 0;
    size_t winners_mutated = 0;
    size_t losers_copied = 0;

    constexpr int initial_requirement = 45;
    int requirement = initial_requirement;

    while (winners_copied < copy_count || winners_mutated < mutate_count || losers_copied < loser_count) {
        for (size_t i = 0; i < nns.size(); i++) {
            if (wins[i] >= requirement) { // better winners get put in more (>=)
                if (winners_mutated < mutate_count) {
                    nns[i].source = NN::Source::Mutate;
                    NN nn(nns[i]);
                    mutate(nn, mutations, temperature);
                    new_nns.push_back(std::move(nn));
                    winners_mutated++;
                }
                if (winners_copied < copy_count) {
                    NN nn(nns[i]);
                    nn.source = NN::Source::WinnerClone;
                    new_nns.push_back(std::move(nn));
                    winners_copied++;
                }
            }
            if (wins[i] <= -requirement) {
                if (losers_copied < loser_count) {
                    NN nn(nns[i]);
                    nn.source = NN::Source::LoserClone;
                    new_nns.push_back(std::move(nn));
                    losers_copied++;
                }
            }
        }
        requirement -= 5;
    }

    for (size_t i = 0; i < dummy_count; i++) {
        new_nns.push_back(make_nn());
    }

    nns = std::move(new_nns);
}

void NNBatch::move(Board& board, const NN& nn, bool to_move, uint64_t moves) {
    Vector result = nn.apply(vectorize(board, to_move));
    unsigned int play_index = max_index(result, moves);
    board.move(play_index, to_move);
}

Board::WinState NNBatch::play_game(Board &board, const NN &black, const NN &white) {
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
            move(board, (to_move == BLACK) ? black : white, to_move, moves);
            passed = false;
        }

        // std::cout << "move " << i << "\n" << board << std::endl;

        to_move = !to_move;
    }

    return board.win_state();
}
