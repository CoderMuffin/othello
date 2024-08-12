#include <iostream>
#include <ostream>
#include <vector>
#include <fstream>
#include "input.hpp"
#include "nnbatch.hpp"
#include "util.hpp"
#include "nn.hpp"
#include "board.hpp"

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
    Board b;

    bootstrap_win32_unicode();

    NNBatch batch{ 100, 50, 25 };

    std::string command;
    auto processor = CommandProcessor {
        CommandArm("nn", {
            CommandArm("move", [](std::vector<std::string> args) {
                return "not implemented";
            }),
            CommandArm("eval", [](std::vector<std::string> args) {
                return "not implemented";
            }),
            CommandArm("play", [](std::vector<std::string> args) {
                return "not implemented";
            }),
            CommandArm("load", [&batch](std::vector<std::string> args) {
                if (args.size() != 3) return std::string("Expected three arguments");

                int nn_start = std::stoi(args[0]);
                int nn_end = std::stoi(args[1]);
                auto filename = args[2] + ".mnn";

                std::ifstream stream(filename);
                if (!stream.is_open()) return std::string("Could not open ") + filename;
                stream >> batch.nns[nn_start];
                stream.close();

                for (int nn = nn_start + 1; nn != nn_end; nn += 1) {
                    batch.nns[nn] = batch.nns[nn_start];
                }

                return std::string("Loaded neural network to range ") + std::to_string(nn_start) + " to " + std::to_string(nn_end) + " from " + filename;
            }),
            CommandArm("save", [&batch](std::vector<std::string> args) {
                if (args.size() != 2) return std::string("Expected two arguments");
                
                int nn = std::stoi(args[0]);
                auto filename = args[1] + ".mnn";

                std::ofstream stream(filename);
                if (!stream.is_open()) return std::string("Could not open ") + filename;
                stream << batch.nns[nn];
                stream.close();

                return std::string("Saved neural network ") + std::to_string(nn) + " in " + filename;
            }),
            CommandArm("train", [&batch](std::vector<std::string> args) {
                if (args.size() != 1) return "Expected one argument";

                int n = std::stoi(args[0]);
                for (int i = 0; i < n; i++) {
                    if (i % (n / 10 + 1) == 0) std::cout << "generation " << i << std::endl;
                    batch.play_generation((n - 1 - i) / (n / 10 + 1) + 1);
                }
                return "";
            })
        }),
        CommandArm("board", {

        }),
        CommandArm("help", [](std::vector<std::string> args) {
            return "uhh todo?";
        }),
        CommandArm("exit", [](std::vector<std::string> args) {
            std::exit(0);
            return "";
        })
    };
    while (true) {
        std::cout << "\x1b[36m>\x1b[0m " << std::flush;
        std::getline(std::cin, command);
        processor.process(command);
        std::cout << std::endl;
    }

    NN nn_black = batch.nns[0];
    NN nn_white = batch.nns[1]; // ({64, 64}, 0.01);

    // std::ofstream o("./model.mnn");
    // o << nn_black;
    // o.close();
    // 
    // std::ofstream o2("./model2.mnn");
    // o2 << nn_white;
    // o2.close();

    std::ifstream i("./model.mnn");
    i >> nn_black;
    i.close();

    std::ifstream i2("./model2.mnn");
    i2 >> nn_white;
    i2.close();

    int wins = 0;
    for (int i = 0; i < 100; i++) {
        b.reset();
        if (NNBatch::play_game(b, nn_black, nn_white) == Board::BlackWins) wins++;
    }
    std::cout << wins << std::endl;
    // NN nn(&std::vector<int> { 4, 2, 3 }, 0.01);
    // Vector v(4);
    // v << 1, 2, 3, 4;
    // std::cout << nn.apply(v) << std::endl;
}

