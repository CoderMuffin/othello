#include "board.hpp"
#include "board.hpp"
#include "board.hpp"
#include "board.hpp"
#include <iostream>
#include "board.hpp"

Board::Board() {
	reset();
}

void Board::reset() {
	occupied = 0x0000'0018'1800'0000;
	color = 0x0000'0010'0800'0000;
}

#ifdef WIN32
#define TEXT_PIECE "()"
#else
#define TEXT_PIECE "\u2b24 "
#endif

std::ostream& operator<<(std::ostream& os, const Board& board) {
	for (int i = 0; i < 64; i++) {
		const char* square_color = ((i / 8 + i) % 2) ? "42" : "102";
		if (BIT(board.occupied, i)) {
			const char* piece_color = BIT(board.color, i) ? "1;37" : "0;30";
			os << "\x1b[" << piece_color << ";" << square_color  << "m" TEXT_PIECE;
		} else {
			os << "\x1b[" << square_color << "m  ";
		}

		if (i % 8 == 7) {
			os << "\x1b[0m" << std::endl;
		}
	}

	return os;
}

Board::GameState Board::state() const {
	if (occupied != MASK_ALL) {
		return Playing;
	}
}

// https://stackoverflow.com/questions/109023/count-the-number-of-set-bits-in-a-32-bit-integer HE HE HE HA
int Board::number_of_set_bits(uint32_t i) {
	i = i - ((i >> 1) & 0x55555555);        // add pairs of bits
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);  // quads
	i = (i + (i >> 4)) & 0x0F0F0F0F;        // groups of 8
	i *= 0x01010101;                        // horizontal sum of bytes
	return i >> 24;               // return just that top byte (after truncating to 32-bit even when int is wider than uint32_t)
}

bool Board::move(uint8_t new_x, uint8_t new_y, bool new_color) {
	this->occupied |= OFFSET(XY(new_x, new_y));
	if (new_color) {
		this->color |= OFFSET(XY(new_x, new_y));
	} else {
		this->color &= ~OFFSET(XY(new_x, new_y));
	}

	int x, y, i;
	uint64_t partial_flip = 0;
	uint64_t commit_flip = 0;

	// east/west (-)
	for (x = new_x + 1; x < 8; x++) {
		i = XY(x, new_y);

		if (!BIT(this->occupied, i)) {
			break;
		} else if (BIT(this->color, i) == new_color) {
			commit_flip |= partial_flip;
			break;
		} else {
			partial_flip |= OFFSET(i);
		}
	}
	partial_flip = 0;
	for (x = new_x - 1; x >= 0; x--) {
		i = XY(x, new_y);

		if (!BIT(this->occupied, i)) {
			break;
		} else if (BIT(this->color, i) == new_color) {
			commit_flip |= partial_flip;
			break;
		} else {
			partial_flip |= OFFSET(i);
		}
	}

	// north/south (|)
	partial_flip = 0;
	for (y = new_y + 1; y < 8; y++) {
		i = XY(new_x, y);

		if (!BIT(this->occupied, i)) {
			break;
		} else if (BIT(this->color, i) == new_color) {
			commit_flip |= partial_flip;
			break;
		} else {
			partial_flip |= OFFSET(i);
		}
	}
	partial_flip = 0;
	for (y = new_y - 1; y >= 0; y--) {
		i = XY(new_x, y);

		if (!BIT(this->occupied, i)) {
			break;
		} else if (BIT(this->color, i) == new_color) {
			commit_flip |= partial_flip;
			break;
		} else {
			partial_flip |= OFFSET(i);
		}
	}

	// ne/sw (/)
	partial_flip = 0;
	for (x = new_x - 1, y = new_y + 1; x >= 0 && y < 8; x--, y++) {
		i = XY(x, y);

		if (!BIT(this->occupied, i)) {
			break;
		} else if (BIT(this->color, i) == new_color) {
			commit_flip |= partial_flip;
			break;
		} else {
			partial_flip |= OFFSET(i);
		}
	}
	partial_flip = 0;
	for (x = new_x + 1, y = new_y - 1; x < 8 && y >= 0; x++, y--) {
		i = XY(x, y);

		if (!BIT(this->occupied, i)) {
			break;
		} else if (BIT(this->color, i) == new_color) {
			commit_flip |= partial_flip;
			break;
		} else {
			partial_flip |= OFFSET(i);
		}
	}

	// nw/se (\)
	partial_flip = 0;
	for (x = new_x + 1, y = new_y + 1; x < 8 && y < 8; x++, y++) {
		i = XY(x, y);

		if (!BIT(this->occupied, i)) {
			break;
		} else if (BIT(this->color, i) == new_color) {
			commit_flip |= partial_flip;
			break;
		} else {
			partial_flip |= OFFSET(i);
		}
	}
	partial_flip = 0;
	for (x = new_x - 1, y = new_y - 1; x >= 0 && y >= 0; x--, y--) {
		i = XY(x, y);

		if (!BIT(this->occupied, i)) {
			break;
		} else if (BIT(this->color, i) == new_color) {
			commit_flip |= partial_flip;
			break;
		} else {
			partial_flip |= OFFSET(i);
		}
	}

	this->color ^= commit_flip;

	return commit_flip != 0;
}

bool Board::move_if_valid(uint8_t x, uint8_t y, bool color) {
	bool flipped_pieces = move(x, y, color);
	if (!flipped_pieces) { // invalid, unset the move we just made
		this->occupied ^= OFFSET(XY(x, y));
	}
	return flipped_pieces;
}

void Board::from_dots(std::string dots) {
	occupied = 0;
	color = 0;
	for (int i = 0; i < 64; i++) {
		if (dots[i] == '.') {
			occupied |= OFFSET(i);
		} else if (dots[i] == '@') {
			occupied |= OFFSET(i);
			color |= OFFSET(i);
		}
	}
}

