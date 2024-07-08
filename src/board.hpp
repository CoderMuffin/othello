#pragma once

#include <cstdint>
#include <ostream>

constexpr auto BLACK = 0;
constexpr auto WHITE = 1;
constexpr uint64_t MASK_ALL = ~0ULL;
#define OFFSET(b) (1ULL << (b))
#define XY(x, y) ((x) + (y)*8)
#define BIT(n, b) (((n) & OFFSET(b)) != 0)

class Board {
public:
	Board();

	/**
	 * @brief reset the board
	 */
	void reset();

	/**
	 * @brief place a piece on the board. overrides any piece previously in that position
	 * @param x - the column to place the piece
	 * @param y - the row to place the piece
	 * @param color - the color of the piece to be placed
	 */
	bool move(uint8_t x, uint8_t y, bool color);

	/**
	 * @brief place a piece on the board if the move is valid.
	 */
	uint64_t valid_moves(bool color) const;

	/**
	 * @brief load a board state from a string of dots
	 * @param dots - the string of dots to load from. '@' represents white tile, '.' represents black tile, ' ' represents a space
	 */
	void from_dots(std::string dots);

	enum GameState {
		Playing,
		BlackWins,
		WhiteWins,
		Draw
	};
	/**
	 * @brief what state is the game in
	 * @returns GameState representing the current status of the game
	 */
	GameState state() const;

	bool operator==(Board& other) { return this->occupied == other.occupied && this->color == other.color; }
	bool operator!=(Board& other) { return !operator==(other); }

	uint64_t occupied;
	uint64_t color;

private:
	int number_of_set_bits(uint32_t i);
};

std::ostream& operator<<(std::ostream& os, const Board& board);

