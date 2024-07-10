#pragma once

#include "board.hpp"
#include "nn.hpp"

class GameNN {
public:
	GameNN() : nn({64, 64}, 1.0f) {}
	bool play(bool color) const;
private:
	NN nn;
	Board b;
};

