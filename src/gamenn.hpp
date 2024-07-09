#pragma once

#include "board.hpp"
#include "nn.hpp"

class GameNN {
public:
	GameNN() : nn({64, 64}, 1.0) {}
	void play(Board &b, bool color);
private:
	NN nn;
};

