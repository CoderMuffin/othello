#include <iostream>
#include <ostream>
#include "nn.hpp"
#include "board.hpp"

int main() {
	Board b;
	std::cout << b << std::endl;
	b.move(5, 4, true);
	std::cout << b << std::endl;
	b.move(6, 4, false);
	std::cout << b << std::endl;
}

