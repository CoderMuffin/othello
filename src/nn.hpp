#pragma once

#include <initializer_list>

#include <Eigen/Dense>

typedef Eigen::MatrixXf Matrix;
typedef Eigen::VectorXf Vector;
class NN {
public:
    NN(const std::initializer_list<int> layer_shapes) {
        initialize_layers_random(layer_shapes);
    }
    NN(const NN& other) = default;
    NN& operator=(const NN& other) = default;
    NN(NN &&other) = default;
    Vector apply(const Vector&) const;
    std::vector<Eigen::Index> shape() const;

    void mutate_change_weight(float temperature);
    void mutate_change_bias(float temperature);

    friend void swap(NN &a, NN &b) {
        using std::swap;
        swap(a.weights, b.weights);
        swap(a.biases, b.biases);
    }
    
    friend std::ostream& operator<<(std::ostream& file, const NN& nn);
    friend std::istream& operator>>(std::istream& file, NN& nn);

    enum class Source {
        Mutate,
        WinnerClone,
        LoserClone,
        Random
    };

    Source source;

private:
    void initialize_layers_random(const std::initializer_list<int>);
    std::vector<Matrix> weights;
    std::vector<Vector> biases;
};

