#pragma once

#include <cstdint>
#include <ostream>

#define BLACK (0)
#define WHITE (1)
#define OFFSET(b) (1ULL << (b))
#define XY(x, y) ((x) + (y)*8)
#define BIT(n, b) ((n) & OFFSET(b))

class Board {
public:
	Board();
	void move(ushort x, ushort y, bool color);

	uint64_t occupied;
	uint64_t color;
private:
	void move_flip_rows(ushort, ushort, bool);
};

std::ostream &operator<<(std::ostream &os, const Board &board);

