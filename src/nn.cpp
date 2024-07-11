#include "nn.hpp"

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

void NN::initialize_layers_random(std::initializer_list<int> shapes) {
    layers.clear();
    for (auto it = shapes.begin(); std::next(it) != shapes.end(); it++) {
        layers.push_back(Matrix::Random(*std::next(it), *it));
    }
}

