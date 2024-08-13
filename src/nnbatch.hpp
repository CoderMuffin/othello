#pragma once

#include "board.hpp"
#include "nn.hpp"

class NNBatch {
public:
	NNBatch(unsigned int nn_count, size_t p_mutate_count, size_t p_copy_count) : mutate_count(p_mutate_count), copy_count(p_copy_count) {
		nns.reserve(nn_count);
		boards.reserve(nn_count / 2);

		for (int i = 0; i < nn_count; i++) {
			nns.push_back(NN({ 64, 256, 256, 64 }, 1.0f));
		}
		for (int i = 0; i < nn_count/2; i++) {
			boards.push_back(Board());
		}
	}

	/*
	 * @brief plays the two neural networks against each other, returning the winner
	 * @returns the winning color (BLACK or WHITE)
	 */
	void mutate(NN& nn, int mutations);
	void play_generation(int mutations);
	static Vector vectorize(const Board& board, bool color);
	static unsigned int max_index(const Vector& result, uint64_t moves);

	static Board::WinState play_game(Board &board, const NN &black, const NN &white);
	std::vector<NN> nns;
private:
	std::vector<Board> boards;
	size_t copy_count; // number of winners to reinsert without mutation
	size_t mutate_count; // number of winners to mutate and reinsert
};

