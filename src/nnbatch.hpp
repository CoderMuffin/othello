#pragma once

#include "board.hpp"
#include "nn.hpp"

template <unsigned int N>
class NNBatch {
public:
	NNBatch(unsigned int nncount) {
		for (int i = 0; i < N; i++) {
			nns[i] = NN({ 64, 64 }, 1.0f);
		}
	}

	std::array<int, N> rankings();

	/*
	 * @brief plays the two neural networks against each other, returning the winner
	 * @returns the winning color (BLACK or WHITE)
	 */
	bool play_game(NN &black, NN &white);
private:
	NN nns[N];
	Board boards[N/2];
};

