#include "muffintest.cpp"
#include "board.hpp"

std::string board_state(Board &b) {
	char out[65]{};
	for (int i = 0; i < 64; i++) {
		out[i] = (b.occupied & OFFSET(i)) ?
			(b.color & OFFSET(i)) ? '@' : '.'
			: ' ';
	}
	out[64] = 0;
	return std::string(out);
}

#define ASSERT_BOARD(state) {     \
	temp_board.from_dots(state);  \
	ASSERT_EQ(board, temp_board); \
}

TEST(assert_board_from_dots) {
	Board board;
	Board temp_board;

	ASSERT_BOARD(
		"        "
		"        "
		"        "
		"   @.   "
		"   .@   "
		"        "
		"        "
		"        ")

	board.move(0, 0, BLACK);
	board.move(7, 7, WHITE);

	ASSERT_BOARD(
		".       "
		"        "
		"        "
		"   @.   "
		"   .@   "
		"        "
		"        "
		"       @")

	ASSERT_EQ(board, temp_board);
}

TEST(flip_x) {
	Board board;
	Board temp_board;

	board.move(5, 4, WHITE);
	board.move(6, 4, WHITE);

	ASSERT_BOARD(
		"        "
		"        "
		"        "
		"   @.   "
		"   .@@@ "
		"        "
		"        "
		"        ");
	ASSERT_EQ(board, temp_board);

	board.move(7, 4, BLACK);

	ASSERT_BOARD(
		"        "
		"        "
		"        "
		"   @.   "
		"   ....."
		"        "
		"        "
		"        ");

	board.reset();
	board.move(1, 3, WHITE);
	board.move(2, 3, WHITE);

	ASSERT_BOARD(
		"        "
		"        "
		"        "
		" @@@.   "
		"   .@   "
		"        "
		"        "
		"        ");

	board.move(0, 3, BLACK);

	ASSERT_BOARD(
		"        "
		"        "
		"        "
		".....   "
		"   .@   "
		"        "
		"        "
		"        ");

	board.move(4, 2, BLACK);
	board.move(4, 2, BLACK);

	ASSERT_BOARD(
		"        "
		"        "
		"        "
		".....   "
		"   .@   "
		"        "
		"        "
		"        ");

	board.reset();
}

