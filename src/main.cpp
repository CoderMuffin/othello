#include <cstring>
#include <iostream>
#include <ostream>
#include <vector>
#include <fstream>
#include "ui/boardui.hpp"
#include "input.hpp"
#include "nnbatch.hpp"
#include "util.hpp"
#include "nn.hpp"
#include "board.hpp"
#undef main

void eval_nn(NN nn_black, std::function<NN(size_t)> nn_white_generator, size_t games) {
    Board b;

    size_t black_wins = 0, white_wins = 0, draws = 0;

    for (size_t i = 0; i < games; i++) {
        b.reset();
        switch (NNBatch::play_game(b, nn_black, nn_white_generator(i))) {
        case Board::BlackWins:
            black_wins++;
            break;
        case Board::WhiteWins:
            white_wins++;
            break;
        case Board::Draw:
            draws++;
            break;
        }
    }

    std::cout << "evaluating neural network Adam from source ";
    switch (nn_black.source) {
    case NN::Source::Random:
        std::cout << "random";
        break;
    case NN::Source::LoserClone:
        std::cout << "cloned loser";
        break;
    case NN::Source::WinnerClone:
        std::cout << "cloned winner";
        break;
    case NN::Source::Mutate:
        std::cout << "mutated winner";
        break;
    }
    std::cout << "\n" << "played " << games << " games:\n\t"
        << black_wins << " Adam wins\n\t"
        << white_wins << " opponent wins\n\t"
        << draws << " draws" << std::endl;
}

bool validate_square(std::string square) {
    if (square.size() != 2 || !('a' <= square[0] && square[0] <= 'h' && '1' <= square[1] && square[1] <= '8')) {
        std::cout << "Bad square" << std::endl;
        return false;
    }
    return true;
}

void debug_dump_bitboard(uint64_t bitboard) {
    Board aux;
    aux.color = 0;
    aux.occupied = bitboard;
    std::cout << aux << std::endl;
}

void show_help(std::string topic) {
    auto show_command = [](int indent, bool is_category, std::string command, std::string help) {
        std::cout << std::string((indent+1)*4, ' ')
                  << (is_category ? "\x1b[33m" : "\x1b[36m") << command
                  << "\x1b[0m - " << help << "\n";
    };
    std::cout << "Othello by CoderMuffin\n"
              << "Commands:\n";
    show_command(0, true, "help", "displays this message");
    show_command(0, true, "nn", "perform neural network related tasks");
    show_command(1, true, "nn eval", "evaluate a neural network");
    show_command(2, false, "nn eval random <nn> <games>", "evaluate a neural network over a specified number of games using random opponents"); 
    show_command(2, false, "nn eval batch <nn>", "evaluate a neural network against the others in its batch");
    show_command(1, false, "nn save <nn> <filename>", "save a neural network to disk");
    show_command(1, false, "nn load <from> <to> <filename>", "replace the specified range with the neural network in the file"); 
    show_command(1, false, "nn train <generations>", "train the current batch of neural networks");
    show_command(0, true, "board", "board commands");
    show_command(1, false, "board reset", "reset the board state");
    show_command(1, false, "board state [history]", "show the current board state or revert to a past history state");
    show_command(1, false, "board dots", "give the representation of the board as dots (for debug)");
    show_command(1, true, "board move", "make a move on the board");
    show_command(2, false, "board move nn <nn>", "use the neural network specified to make a move");
    show_command(2, false, "board move <square>", "place a piece at the specified square (e.g. a3)");
    show_command(0, true, "ui", "ui commands");
    show_command(1, false, "ui start", "start the board ui. click to place a piece");
    show_command(1, false, "ui stop", "stop the board ui");
    show_command(0, false, "exit", "quit");
}

struct HistoryState {
    Board board;
    bool to_move;
    HistoryState(Board p_board, bool p_to_move) : board(p_board), to_move(p_to_move) {}
};

int main() {
    bootstrap_win32_unicode();

    BoardUI ui;

    std::vector<HistoryState> boards{
        HistoryState(Board(), BLACK)
    };
    NNBatch batch{ 100, 30, 20, 40 };

    std::string command;
    auto processor = CommandProcessor {
        CommandArm("debug", [&boards](auto args) {
            //bool color = boards.back().color;
            //Board b = boards.back().board;

            //std::cout << b << std::endl;
            //boards.push_back(HistoryState(b, !color));
        }),
        CommandArm("nn", {
            CommandArm("eval", {
                CommandArm("batch", [&batch](auto args) {
                    if (args.size() != 1) {
                        std::cout << "Expected one argument (try running help)" << std::endl;
                        return;
                    }

                    eval_nn(batch.nns[std::stoull(args[0])], [&batch](size_t game) {
                        return batch.nns[game];
                    }, batch.nns.size());
                }),
                CommandArm("random", [&batch](auto args) {
                    if (args.size() != 2) {
                        std::cout << "Expected two arguments (try running help)" << std::endl;
                        return;
                    }

                    eval_nn(batch.nns[std::stoull(args[0])], [](size_t game) {
                        return NNBatch::make_nn();
                    }, std::stoull(args[1]));
                })
            }),
            CommandArm("load", [&batch](auto args) {
                if (args.size() != 3) {
                    std::cout << "Expected three arguments (try running help)" << std::endl;
                    return;
                }

                size_t nn_start = std::stoull(args[0]);
                size_t nn_end = std::stoull(args[1]);
                auto filename = args[2] + ".mnn";

                std::ifstream stream(filename);
                if (!stream.is_open()) {
                    std::cout << "Could not open " << filename << std::endl;
                    return;
                }
                stream >> batch.nns[nn_start];
                stream.close();

                for (size_t nn = nn_start + 1; nn <= nn_end; nn += 1) {
                    batch.nns[nn] = batch.nns[nn_start];
                }

                std::cout << "Loaded neural network to range " << nn_start << " to " << " from " << filename;
            }),
            CommandArm("save", [&batch](auto args) {
                if (args.size() != 2) {
                    std::cout << "Expected two arguments (try running help)" << std::endl;
                    return;
                }
                
                size_t nn = std::stoull(args[0]);
                auto filename = args[1] + ".mnn";

                std::ofstream stream(filename);
                if (!stream.is_open()) {
                    std::cout << "Could not open " << filename << std::endl;
                    return;
                }
                stream << batch.nns[nn];
                stream.close();

                std::cout << "Saved neural network " << nn << " in " << filename << std::endl;
            }),
            CommandArm("train", [&batch](auto args) {
                if (args.size() != 1) {
                    std::cout << "Expected one argument (try running help)" << std::endl;
                    return;
                }

                int n = std::stoi(args[0]);
                constexpr int max_learn_rate = 40;
                for (int i = 0; i < n; i++) {
                    if (i % (n / 10) == 0) {
                        std::cout << "generation " << i << std::endl;

                        for (size_t nn_eval_i = 0; nn_eval_i < batch.nns.size(); nn_eval_i++) {
                            if (batch.nns[nn_eval_i].source == NN::Source::WinnerClone) {
                                eval_nn(batch.nns[nn_eval_i], [](size_t game) {
                                    return NNBatch::make_nn();
                                }, 1000);
                                break;
                            }
                        }
                    }
                    batch.play_generation((max_learn_rate * (n - i)) / n + 1, std::clamp(1.5f - ((float)i) / ((float)n), 0.0f, 1.0f));
                }
            })
        }),
        CommandArm("board", {
            CommandArm("reset", [&boards, &ui](auto args) {
                boards.push_back(HistoryState(Board(), BLACK));
                Board b{};
                ui.set_board(b);
                std::cout << boards.back().board << std::endl;
            }),
            CommandArm("state", [&boards](auto args) {
                if (args.size() == 1) {
                    boards.push_back(boards[std::stoull(args[0])]);
                } else if (args.size() == 0) {
                    // fallthrough
                } else {
                    std::cout << "Expected one or no arguments (try running help)" << std::endl;
                    return;
                }

                auto& state = boards.back();
                std::cout << state.board << std::endl;
                auto [black, white] = state.board.piece_count();
                if (state.board.valid_moves(BLACK) == 0 && state.board.valid_moves(WHITE) == 0) {
                    std::cout << "The game is over. ";
                    if (black > white) {
                        std::cout << "Black has won! ";
                    } else if (black == white) {
                        std::cout << "It's a draw! ";
                    } else {
                        std::cout << "White has won! ";
                    }
                } else {
                    std::cout << (state.to_move == BLACK ? "Black" : "White") << " to play ";
                }

                std::cout << "(" << black << " black, " << white << " white)" << std::endl;
                std::cout << "History state: " << (boards.size() - 1) << std::endl;
            }),
            CommandArm("dots", [&boards](auto args) {
                std::cout << boards.back().board.to_dots() << std::endl;
            }),
            CommandArm("move", {
                CommandArm("nn", [&batch, &boards, &ui](auto args) {
                    auto& state = boards.back();
                    if (args.size() != 1) {
                        std::cout << "Expected one argument (try running help)" << std::endl;
                        return;
                    }

                    uint64_t moves = state.board.valid_moves(state.to_move);
                    if (moves == 0) {
                        std::cout << "No valid moves! Passing..." << std::endl;
                        boards.push_back(HistoryState(state.board, !state.to_move));
                        std::cout << "History state: " << (boards.size() - 1) << std::endl;
                        return;
                    }

                    Board new_board(state.board);
                    NN& nn = batch.nns[std::stoull(args[0])];
                    NNBatch::move(new_board, nn, state.to_move, moves);
                    ui.set_board(new_board);

                    boards.push_back(HistoryState(new_board, !state.to_move));
                    std::cout << new_board << std::endl;
                    std::cout << "History state: " << (boards.size() - 1) << std::endl;
                }),
                CommandArm([&boards, &ui](auto args) {
                    auto& state = boards.back();

                    if (args.size() != 1 && args.size() != 2) {
                        std::cout << "Expected one or two arguments" << std::endl;
                        return;
                    }

                    if (!validate_square(args[0])) return;

                    uint64_t moves = state.board.valid_moves(state.to_move);
                    if (moves == 0) {
                        std::cout << "No valid moves! Passing..." << std::endl;
                        boards.push_back(HistoryState(state.board, !state.to_move));
                        std::cout << "History state: " << (boards.size() - 1) << std::endl;
                        return;
                    }

                    Board new_board(state.board);
                    auto position = (unsigned int)XY(args[0][0] - 'a', args[0][1] - '1');
                    if (BIT(moves, position) || (args.size() == 2 && args[1] == "force")) {
                        new_board.move(position, state.to_move);
                        ui.set_board(new_board);

                        boards.push_back(HistoryState(new_board, !state.to_move));
                        std::cout << new_board << std::endl;
                        std::cout << "History state: " << (boards.size() - 1) << std::endl;
                    } else {
                        std::cout << "Invalid move" << std::endl;
                    }
                })
            }),
        }),
        CommandArm("ui", {
            CommandArm("start", [&boards, &ui](auto args) {
                ui.start();
                ui.set_board(boards.back().board);
            }),
            CommandArm("stop", [&ui](auto args) {
                ui.stop();
            })
        }),
        CommandArm("help", [](auto args) {
            if (args.size() >= 1) {
                show_help(args[1]);
            } else {
                show_help("help");
            }
        }),
        CommandArm("exit", [](auto args) {
            std::exit(0);
        })
    };

    std::mutex mutex;

    ui.on_click.bind([&](int x, int y) {
        std::string command("board move ");
        command.push_back('a' + x);
        command.push_back('1' + y);

        std::cout << "[ui click]" << std::endl;

        mutex.lock();
        processor.process(command);
        mutex.unlock();

        std::cout << "\n\x1b[36m>\x1b[0m " << std::flush;
    });

    processor.process("help");

    while (true) {
        try {
            std::cout << "\x1b[36m>\x1b[0m " << std::flush;
            std::getline(std::cin, command);

            mutex.lock();
            processor.process(command);
            mutex.unlock();

            std::cout << std::endl;
        } catch (std::exception &ex) {
            std::cerr << "Unhandled exception!\n\t" << ex.what() << "\nRethrow (y/n)? ";

            std::string choice;
            std::getline(std::cin, choice);
            if (choice[0] == 'y') throw;
        }
    }
}

