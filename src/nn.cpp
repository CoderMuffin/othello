#include <numeric>
#include "nn.hpp"
#include "board.hpp"

Vector NN::apply(Vector& in) const {
    Vector out = in;
    for (Matrix layer : layers) {
        out = layer * out;
    }
    return out;
}

std::vector<int> NN::shape() const {
    std::vector<int> out = ;
    return out;
}