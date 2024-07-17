#pragma once

#include <initializer_list>
#include <Eigen/Dense>

typedef Eigen::MatrixXf Matrix;
typedef Eigen::VectorXf Vector;
class NN {
public:
    NN(const std::initializer_list<int> layer_shapes, float learning_rate) {
        initialize_layers_random(layer_shapes);
    }
    NN(const NN& other) = default;
    NN(NN &&other) = default;
    Vector apply(const Vector&) const;
    std::vector<Eigen::Index> shape() const;

    void mutate_add_layer();
    void mutate_add_node();
    void mutate_change_weight();
    void mutate_change_bias();

    friend void swap(NN &a, NN &b) {
        using std::swap;
        swap(a.weights, b.weights);
        swap(a.biases, b.biases);
    }
    
    friend std::ostream& operator<<(std::ostream& file, const NN& nn);
    friend std::istream& operator>>(std::istream& file, NN& nn);
private:
    void initialize_layers_random(const std::initializer_list<int>);
    std::vector<Matrix> weights;
    std::vector<Vector> biases;
};

