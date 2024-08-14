#pragma once

#include <cstdint>
#include <ostream>
#include <utility>

constexpr auto BLACK = false;
constexpr auto WHITE = true;
constexpr uint64_t MASK_ALL = ~0ULL;
#define OFFSET(b) (1ULL << (b))
#define XY(x, y) ((x) + (y)*8)
#define BIT(n, b) (((n) & OFFSET(b)) != 0)

class Board {
public:
	enum WinState {
	   BlackWins,
	   WhiteWins,
	   Draw
	};

	Board();
	Board(const Board& other) {
		this->occupied = other.occupied;
		this->color = other.color;
	}

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
	void move(unsigned int x, unsigned int y, bool color);
	void move(unsigned int index, bool color);

	/**
	 * @brief place a piece on the board if the move is valid.
	 */
	uint64_t valid_moves(bool color) const;

	/**
	 * @brief load a board state from a string of dots
	 * @param dots - the string of dots to load from. '@' represents white tile, '.' represents black tile, ' ' represents a space
	 */
	void from_dots(std::string dots);

	std::string to_dots() const;

	/**
	 * @brief what state is the game in
	 * @returns WinState representing the current status of the game
	 */
	WinState win_state() const;

	inline std::pair<int, int> piece_count() const {
		int white = popcount64(occupied & color);
		int black = popcount64(occupied & ~color);
		return std::make_pair(black, white);
	}

	inline bool operator==(Board& other) {
		return this->occupied == other.occupied && this->color == other.color;
	}
	inline bool operator!=(Board& other) {
		return !operator==(other);
	}

	uint64_t occupied;
	uint64_t color;

private:
	// https://stackoverflow.com/questions/109023/count-the-number-of-set-bits-in-a-32-bit-integer HE HE HE HA
	static int popcount32(uint32_t i) {
		// see also popcnt
		i = i - ((i >> 1) & 0x55555555);        // add pairs of bits
		i = (i & 0x33333333) + ((i >> 2) & 0x33333333);  // quads
		i = (i + (i >> 4)) & 0x0F0F0F0F;        // groups of 8
		i *= 0x01010101;                        // horizontal sum of bytes
		return (int)(i >> 24);               // return just that top byte (after truncating to 32-bit even when int is wider than uint32_t)
	}

	static int popcount64(uint64_t i) {
		return popcount32((uint32_t)(i & 0xFF'FF'FF'FF)) + popcount32((uint32_t)((i >> 32) & 0xFF'FF'FF'FF));
	}
};

std::ostream& operator<<(std::ostream& os, const Board& board);

