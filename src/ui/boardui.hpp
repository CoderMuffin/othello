#pragma once

#include "ui.hpp"
#include "../board.hpp"

class BoardUI : private UI {
public:
    BoardUI();
    void start();
    void stop();
    void draw() override;
    void set_board(Board& board);

    Event<int, int> on_click;
    using UI::running;

private:
    std::mutex mutex;
    const int cell_size = 50;
};
