#include "gamenn.hpp"
#include "nn.hpp"

bool GameNN::play(Board& b, bool color) {
    uint64_t moves = b.valid_moves(color);
    if (moves == 0) {
        return false;
    }

    nn.apply(b).;

    return true;
}
