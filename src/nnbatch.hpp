#pragma once

#include "board.hpp"
#include "nn.hpp"

template <unsigned int n>
class NNBatch {
public:
	NNBatch(unsigned int nncount);
	std::array<int, n> rankings();
private:
	NN nns[n];
	Board board;
};

