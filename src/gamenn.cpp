#include "gamenn.hpp"
#include "nn.hpp"

void GameNN::play(Vector &in, bool color) const {
    uint64_t moves = b.valid_moves(color);
    if (moves == 0) {
        return false;
    }

    nn.apply();

    return true;
}

