#include "board.hpp"
#include "util.hpp" // this is not unused, it is used two lines below :P

#define MUFFINTEST_STARTUP bootstrap_win32_unicode();

#include "muffintest.cpp"

#define ASSERT_BOARD(state) {     \
	temp_board.from_dots(state);  \
	ASSERT_EQ(board, temp_board); \
}

#define ASSERT_BITBOARD(bits, dots) {  \
	temp_board.occupied = bits;  \
	temp_board.color = 0;         \
	temp_board_2.from_dots(dots);  \
	temp_board_2.color = 0;         \
	ASSERT_EQ(temp_board, temp_board_2); \
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
	board.move(1, 4, BLACK);
	board.move(2, 4, BLACK);

	ASSERT_BOARD(
		"        "
		"        "
		"        "
		"   @.   "
		" ...@   "
		"        "
		"        "
		"        ");

	board.move(0, 4, WHITE);

	ASSERT_BOARD(
		"        "
		"        "
		"        "
		"   @.   "
		"@@@@@   "
		"        "
		"        "
		"        ");

	board.from_dots(
		"        "
		"        "
		"        "
		"  @.@.  "
		"        "
		"        "
		"        "
		"        ");

	board.move(1, 3, BLACK);
	board.move(6, 3, WHITE);

	ASSERT_BOARD(
		"        "
		"        "
		"        "
		" ...@@@ "
		"        "
		"        "
		"        "
		"        ");
}

TEST(flip_y) {
	Board board;
	Board temp_board;

	board.move(3, 1, WHITE);
	board.move(3, 2, WHITE);

	ASSERT_BOARD(
		"        "
		"   @    "
		"   @    "
		"   @.   "
		"   .@   "
		"        "
		"        "
		"        ");
	ASSERT_EQ(board, temp_board);

	board.move(3, 0, BLACK);

	ASSERT_BOARD(
		"   .    "
		"   .    "
		"   .    "
		"   ..   "
		"   .@   "
		"        "
		"        "
		"        ");

	board.reset();
	board.move(3, 5, BLACK);
	board.move(3, 6, BLACK);

	ASSERT_BOARD(
		"        "
		"        "
		"        "
		"   @.   "
		"   .@   "
		"   .    "
		"   .    "
		"        ");

	board.move(3, 7, WHITE);

	ASSERT_BOARD(
		"        "
		"        "
		"        "
		"   @.   "
		"   @@   "
		"   @    "
		"   @    "
		"   @    ");

	board.from_dots(
		"        "
		"        "
		"   .    "
		"   @    "
		"   .    "
		"   @    "
		"        "
		"        ");

	board.move(3, 6, BLACK);
	board.move(3, 1, WHITE);

	ASSERT_BOARD(
		"        "
		"   @    "
		"   @    "
		"   @    "
		"   .    "
		"   .    "
		"   .    "
		"        ");
}

TEST(flip_se) {
	Board board;
	Board temp_board;

	board.move(2, 2, BLACK);
	board.move(5, 5, BLACK);

	ASSERT_BOARD(
		"        "
		"        "
		"  .     "
		"   ..   "
		"   ..   "
		"     .  "
		"        "
		"        ");

	board.reset();
	board.move(5, 5, BLACK);
	board.move(2, 2, BLACK);

	ASSERT_BOARD(
		"        "
		"        "
		"  .     "
		"   ..   "
		"   ..   "
		"     .  "
		"        "
		"        ");

	board.from_dots(
		"        "
		"        "
		"  .     "
		"   @    "
		"    .   "
		"     @  "
		"        "
		"        ");

	board.move(6, 6, BLACK);
	board.move(1, 1, WHITE);

	ASSERT_BOARD(
		"        "
		" @      "
		"  @     "
		"   @    "
		"    .   "
		"     .  "
		"      . "
		"        ");
}

TEST(flip_sw) {
	Board board;
	Board temp_board;

	board.move(2, 5, WHITE);
	board.move(5, 2, WHITE);

	ASSERT_BOARD(
		"        "
		"        "
		"     @  "
		"   @@   "
		"   @@   "
		"  @     "
		"        "
		"        ");

	board.reset();
	board.move(5, 2, WHITE);
	board.move(2, 5, WHITE);

	ASSERT_BOARD(
		"        "
		"        "
		"     @  "
		"   @@   "
		"   @@   "
		"  @     "
		"        "
		"        ");

	board.from_dots(
		"        "
		"        "
		"     .  "
		"    @   "
		"   .    "
		"  @     "
		"        "
		"        ");

	board.move(1, 6, BLACK);
	board.move(6, 1, WHITE);

	ASSERT_BOARD(
		"        "
		"      @ "
		"     @  "
		"    @   "
		"   .    "
		"  .     "
		" .      "
		"        ");
}

TEST(flip_double) {
	Board board;
	Board temp_board;

	board.from_dots(
		"        "
		"        "
		"        "
		"@.. ...@"
		"        "
		"        "
		"        "
		"        ");
	board.move(3, 3, WHITE);
	ASSERT_BOARD(
		"        "
		"        "
		"        "
		"@@@@@@@@"
		"        "
		"        "
		"        "
		"        ");

	board.from_dots(
		"   @    "
		"   .    "
		"   .    "
		"        "
		"   .    "
		"   .    "
		"   .    "
		"   @    ");
	board.move(3, 3, WHITE);
	ASSERT_BOARD(
		"   @    "
		"   @    "
		"   @    "
		"   @    "
		"   @    "
		"   @    "
		"   @    "
		"   @    ");

	board.from_dots(
		"@       "
		" .      "
		"  .     "
		"        "
		"    .   "
		"     .  "
		"      . "
		"       @");
	board.move(3, 3, WHITE);
	ASSERT_BOARD(
		"@       "
		" @      "
		"  @     "
		"   @    "
		"    @   "
		"     @  "
		"      @ "
		"       @");

	board.from_dots(
		"       @"
		"      . "
		"     .  "
		"    .   "
		"        "
		"  .     "
		" .      "
		"@       ");

	board.move(3, 4, WHITE);

	ASSERT_BOARD(
		"       @"
		"      @ "
		"     @  "
		"    @   "
		"   @    "
		"  @     "
		" @      "
		"@       ");
}

TEST(board_flip_all) {
	Board board;
	Board temp_board;

	board.from_dots(
		" @  @  @"
		"  . . . "
		"   ...  "
		"@... ..@"
		"   ...  "
		"  . . . "
		" .  .  @"
		"@   @   ");

	board.move(4, 3, WHITE);

	ASSERT_BOARD(
		" @  @  @"
		"  @ @ @ "
		"   @@@  "
		"@@@@@@@@"
		"   @@@  "
		"  @ @ @ "
		" @  @  @"
		"@   @   ");
}

TEST(move_gen) {
	Board board;
	Board temp_board;
	Board temp_board_2;

	board.from_dots(
		"        "
		"        "
		"  ...   "
		"  .@.   "
		"  ...   "
		"        "
		"        "
		"        ");
	ASSERT_BITBOARD(board.valid_moves(WHITE),
		"        "
		" @ @ @  "
		"        "
		" @   @  "
		"        "
		" @ @ @  "
		"        "
		"        ");
}

TEST(winner) {
	Board board;

	board.from_dots(
		"        "
		"        "
		"  .@.   "
		"  .@@   "
		"  .@.   "
		"        "
		"        "
		"        ");
	ASSERT_EQ(board.win_state(), Board::BlackWins);

	board.from_dots(
		"        "
		"        "
		"  .@.   "
		"  @@@   "
		"  .@.   "
		"        "
		"        "
		"        ");
	ASSERT_EQ(board.win_state(), Board::WhiteWins);

	board.from_dots(
		"        "
		"        "
		"  .@.   "
		"  @ @   "
		"  .@.   "
		"        "
		"        "
		"        ");
	ASSERT_EQ(board.win_state(), Board::Draw);

	board.from_dots(
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		"       .");
	ASSERT_EQ(board.win_state(), Board::BlackWins);
	
	board.from_dots(
		"@       "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        ");
	ASSERT_EQ(board.win_state(), Board::WhiteWins);

	board.from_dots(
		"@       "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		"       .");
	ASSERT_EQ(board.win_state(), Board::Draw);
}

