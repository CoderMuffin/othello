#include "board.hpp"

std::string board_state(Board &b) {
	char out[64];
	for (int i = 0; i < 64; i++) {
		out[i] = (b.occupied & OFFSET(i)) ?
			(b.color & OFFSET(i)) ? '@' : '.'
			: ' ';
	}
	return out;
}

int main() {

}

{
	Board b;

	b.move(5, 4, WHITE);
	REQUIRE(board_state(b) == "        "
	                          "        "
	                          "        "
	                          "   @.   "
	                          "   .@@  "
	                          "        "
	                          "        "
	                          "        ");
}

