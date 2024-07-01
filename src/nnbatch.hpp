#pragma once

#include "nn.hpp"

template <unsigned int number> class NNBatch {
private:
    NN nns[number];
};