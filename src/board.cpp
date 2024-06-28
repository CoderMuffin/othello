#include <iostream>
#include "board.hpp"

Board::Board() {
	occupied = 0x0000'0018'1800'0000;
	color = 0x0000'0010'0800'0000;
}

std::ostream &operator<<(std::ostream &os, const Board &board) {
	for (int i = 0; i < 64; i++) {
		const char *square_color = ((i/8 + i) % 2) ? "42" : "102";
		if (BIT(board.occupied, i)) {
			const char* piece_color = BIT(board.color, i) ? "37" : "30";
			os << "\x1b[" << square_color << ";" << piece_color << "m\u2b24 ";
		} else {
			os << "\x1b[" << square_color << "m  ";
		}

		if (i % 8 == 7) {
			os << "\x1b[0m" << std::endl;
		}
	}

	return os;
}

void Board::move_flip_rows(ushort new_x, ushort new_y, bool new_color) {
	int x, y, i;
	uint64_t partial_flip = 0;
	uint64_t commit_flip = 0;

	// east/west
	for (x = new_x + 1; x < 8; x++) {
		i = XY(x, new_y);
		bool color = BIT(this->color, i);
		
		if (!BIT(this->occupied, i)) {
			break;
		} else if (color == new_color) {
			commit_flip |= partial_flip;
			break;
		} else {
			partial_flip |= OFFSET(i);
		}
	}
	partial_flip = 0;
	for (x = new_x - 1; x >= 0; x--) {
		i = XY(x, new_y);
		bool color = BIT(this->color, i);
		
		if (!BIT(this->occupied, i)) {
			break;
		} else if (color == new_color) {
			commit_flip |= partial_flip;
			break;
		} else {
			partial_flip |= OFFSET(i);
		}
	}

	// north/south
	partial_flip = 0;
	for (y = new_x + 1; y < 8; y++) {
		i = XY(new_x, y);
		bool color = BIT(this->color, i);
		
		if (!BIT(this->occupied, i)) {
			break;
		} else if (color == new_color) {
			commit_flip |= partial_flip;
			break;
		} else {
			partial_flip |= OFFSET(i);
		}
	}
	partial_flip = 0;
	for (y = new_x - 1; y >= 0; y--) {
		i = XY(new_x, y);
		bool color = BIT(this->color, i);
		
		if (!BIT(this->occupied, i)) {
			break;
		} else if (color == new_color) {
			commit_flip |= partial_flip;
			break;
		} else {
			partial_flip |= OFFSET(i);
		}
	}

	this->color ^= commit_flip;
}

void Board::move(ushort new_x, ushort new_y, bool new_color) {
	this->occupied |= OFFSET(XY(new_x, new_y));
	if (new_color) {
		this->color |= OFFSET(XY(new_x, new_y));
	} else {
		this->color &= ~OFFSET(XY(new_x, new_y));
	}

	move_flip_rows(new_x, new_y, new_color);
}

