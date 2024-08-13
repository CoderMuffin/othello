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

std::string eval_nn(NN nn_black, std::function<NN(int)> nn_white_generator, int games) {
    Board b;

    int black_wins = 0, white_wins = 0, draws = 0;

    for (int i = 0; i < games; i++) {
        b.reset();
        switch (NNBatch::play_game(b, nn_black, nn_white_generator(i))) {
        case Board::BlackWins:
            black_wins++;
            break;
        case Board::WhiteWins:
            white_wins++;
            break;
        default:
            draws++;
            break;
        }
    }

    std::ostringstream out;
    out << "played " << games << " games:\n\t"
        << black_wins << " black wins\n\t"
        << white_wins << " white wins\n\t"
        << draws << " draws";
    return out.str();
}

int main() {
    bootstrap_win32_unicode();

    Board board;
    NNBatch batch{ 100, 50, 25 };

    std::string command;
    auto processor = CommandProcessor {
        CommandArm("nn", {
            CommandArm("move", [](std::vector<std::string> args) {
                return "not implemented";
            }),
            CommandArm("eval", {
                CommandArm("batch", [&batch](std::vector<std::string> args) {
                    return eval_nn(batch.nns[std::stoi(args[0])], [&batch](int game) {
                    }, batch.size());
                })
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

                for (int nn = nn_start + 1; nn <= nn_end; nn += 1) {
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
}

