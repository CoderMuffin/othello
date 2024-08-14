#include "nn.hpp"
#include <cassert>
#include <random>
#include <iostream>

std::ostream& operator<<(std::ostream& file, const NN& nn) {
    assert(nn.weights.size() == nn.biases.size());
    
    uint64_t size = nn.weights.size();
    file << size << '\n';
    
    for (const auto& element : nn.weights) {
        file << element.rows() << ' ' << element.cols() << '\n';
        file << element << '\n';
    }
    for (const auto& element : nn.biases) {
        file << element.size() << '\n';
        file << element << '\n';
    }

    return file;
}

std::istream& operator>>(std::istream& file, NN& nn) {
    nn.weights.clear();
    nn.biases.clear();
    
    uint64_t size;
    file >> size;
    
    nn.weights.reserve(size);
    nn.biases.reserve(size);

    for (uint64_t i = 0; i < size; i++) {
        Eigen::Index rows, cols;
        file >> rows >> cols;
        
        Matrix m(rows, cols);
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                file >> m(row, col);
            }
        }

        nn.weights.push_back(m);
    }

    for (uint64_t i = 0; i < size; i++) {
        Eigen::Index m_size;
        file >> m_size;

        Vector v(m_size);
        for (int j = 0; j < m_size; j++) {
            file >> v(j);
        }
        nn.biases.push_back(v);
    }

    return file;
}

Vector NN::apply(const Vector& in) const {
    Vector out = in;
    for (size_t i = 0; i < weights.size(); i++) {
        out = weights[i] * out + biases[i];
    }
    return out;
}

std::vector<Eigen::Index> NN::shape() const {
    // cols in, rows out
    std::vector<Eigen::Index> out;
    out.push_back(weights[0].cols());
    for (auto& layer : weights) {
        out.push_back(layer.rows());
    }
    return out;
}

void NN::initialize_layers_random(const std::initializer_list<int> shapes) {
    weights.clear();
    biases.clear();
    for (auto it = shapes.begin(); std::next(it) != shapes.end(); it++) {
        weights.push_back(Matrix::Random(*std::next(it), *it));
        biases.push_back(Vector::Random(*std::next(it)));
    }
}

std::mt19937 mutate_rng{std::random_device()()};
std::uniform_real_distribution<float> mutate_float_dist{-1, 1};

void NN::mutate_change_weight() {
    auto &weights_layer = weights[mutate_rng() % weights.size()];
    weights_layer(mutate_rng() % weights_layer.rows(), mutate_rng() % weights_layer.cols()) = mutate_float_dist(mutate_rng);
}

void NN::mutate_change_bias() {
    auto &biases_layer = biases[mutate_rng() % biases.size()];
    biases_layer(mutate_rng() % biases_layer.size()) = mutate_float_dist(mutate_rng);
}

void NN::mutate_add_layer() {
    size_t index = (mutate_rng() % (weights.size() - 1)) + 1; // if 3 layers, can only insert in middle 2 gaps
    // call to copy ctor
    biases.insert(biases.cbegin() + (ptrdiff_t)index, biases[index - 1]);
    // imagine position 0 (:!: :)
    //                     0 1 2
    // index                0 1
    // insert index 0, take same inputs as old one but new outputs
    // reform index 1, take same inputs (same size) and same outputs as old
    // hence no need to reform!
    auto before = Matrix::Random(weights[index - 1].cols(), weights[index - 1].cols()); // cols deliberate - square matrix produces same output
    weights.insert(weights.cbegin() + (ptrdiff_t)index, before);
}

void NN::mutate_add_node() {
    // broken!!!
    return;
/*
    int index = (mutate_rng() % (weights.size() - 1)) + 1; // if 3 layers, can only insert in middle 2 gaps
    // call to copy ctor
    biases[index].add.insert(biases.cbegin() + index, biases[index - 1]);
    auto before = Matrix::Random(weights[index - 1].cols(), weights[index - 1].cols()); // cols deliberate - square matrix produces same output
    weights.insert(weights.cbegin() + index, before);
*/
}
