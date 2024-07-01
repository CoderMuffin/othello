#pragma once

#include <Eigen/Eigen>

typedef Eigen::MatrixXf Matrix;
typedef Eigen::VectorXf Vector;
class NN {
public:
    NN(std::vector<int> &layer_shapes, float learning_rate) : learning_rate(learning_rate) {
        initialize_layers_random(layer_shapes);
    }

    Vector apply(Vector&) const;
    std::vector<Eigen::Index> shape() const;

private:
    void initialize_layers_random(std::vector<int>&);
    std::vector<Matrix> layers;
    float learning_rate;
};

