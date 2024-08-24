#pragma once

#include "board.hpp"
#include "nn.hpp"

unsigned int map8x8(unsigned int miniboard);

unsigned int map4x4(unsigned int board);

class NNBatch {
public:
	NNBatch(size_t nn_count, size_t p_mutate_count, size_t p_copy_count, size_t p_dummy_count) : mutate_count(p_mutate_count), copy_count(p_copy_count), dummy_count(p_dummy_count) {
		nns.reserve(nn_count);
		boards.reserve(nn_count / 2);

		for (size_t i = 0; i < nn_count; i++) {
			NN nn = make_nn();
			nn.source = NN::Source::WinnerClone;
			nns.push_back(nn);
		}
		for (size_t i = 0; i < nn_count/2; i++) {
			boards.push_back(Board());
		}
	}

	static inline NN make_nn() {
		NN nn{ 16, 256, 256, 16 };
		nn.source = NN::Source::Random;
		return nn;
	}

	/*
	 * @brief plays the two neural networks against each other, returning the winner
	 * @returns the winning color (BLACK or WHITE)
	 */
	void mutate(NN& nn, int mutations, float temperature);
	void play_generation(int mutations, float temperature);
	static Vector vectorize(const Board& board, bool color);
	static void move(Board& board, const NN& nn, bool to_move, uint64_t moves);
	static unsigned int max_index(const Vector& result, uint64_t moves);

	static Board::WinState play_game(Board &board, const NN &black, const NN &white);
	std::vector<NN> nns;
private:
	std::vector<Board> boards;
	size_t mutate_count; // number of winners to mutate and reinsert
	size_t copy_count; // number of winners to reinsert without mutation
	size_t dummy_count; // number of winners to reinsert without mutation
};

