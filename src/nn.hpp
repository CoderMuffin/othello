#pragma once

#include <Eigen/Eigen>

typedef Eigen::MatrixXf Matrix;
typedef Eigen::VectorXf Vector;
class NN {
public:
    NN(std::vector<int> layer_shapes, float learning_rate) : layer_shapes(layer_shapes), learning_rate(learning_rate) {}
    Vector apply(Vector& in) const;
    std::vector<int> shape() const;
private:
    std::vector<Matrix> layers;
    float learning_rate;
};

