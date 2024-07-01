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

std::vector<Eigen::Index> NN::shape() const {
    // cols in, rows out
    std::vector<Eigen::Index> out;
    out.push_back(layers[0].cols());
    for (auto& layer : layers) {
        out.push_back(layer.rows());
    }
    return out;
}

void NN::initialize_layers_random(std::vector<int> &shapes) {
    layers.clear();
    for (int i = 0; i < shapes.size() - 1; i++) {
        layers.push_back(Matrix::Random(shapes[i + 1], shapes[i]));
    }
}