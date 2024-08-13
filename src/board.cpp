#include "board.hpp"
#include <iostream>

Board::Board() {
    reset();
}

void Board::reset() {
    occupied = 0x0000'0018'1800'0000;
    color = 0x0000'0010'0800'0000;
}

#ifdef WIN32
#define TEXT_PIECE u8"\u2b24 "
#else
#define TEXT_PIECE "\u2b24 "
#endif

std::ostream& operator<<(std::ostream& os, const Board& board) {
    for (int i = 0; i < 64; i++) {
        const char* square_color = ((i / 8 + i) % 2) ? "42" : "102";
        if (BIT(board.occupied, i)) {
            const char* piece_color = BIT(board.color, i) ? "1;37" : "0;30";
            os << "\x1b[" << piece_color << ";" << square_color << "m" TEXT_PIECE;
        } else {
            os << "\x1b[" << square_color << "m  ";
        }

        if (i % 8 == 7) {
            os << "\x1b[0m" << std::endl;
        }
    }

    return os;
}

// e, s, se, sw, w, n, nw, ne
const static uint64_t direction_edge[] = { 0x7F7F7F7F7F7F7F7FU, 0x00FFFFFFFFFFFFFFU, 0x007F7F7F7F7F7F7FU, 0x00FEFEFEFEFEFEFEU, 0xFEFEFEFEFEFEFEFEU, 0xFFFFFFFFFFFFFF00U, 0xFEFEFEFEFEFEFE00U, 0x7F7F7F7F7F7F7F00U };
const static int direction_shift[] = { 1, 8, 9, 7 }; // can't bitshift by negative apparently

void Board::move(uint8_t new_x, uint8_t new_y, bool new_color) {
    move(XY(new_x, new_y), new_color);
}

void Board::move(uint8_t index, bool new_color) {
    uint64_t position = OFFSET(index);
    this->occupied |= position;
    if (new_color) {
        this->color |= position;
    } else {
        this->color &= ~position;
    }

    uint64_t mine = occupied & (new_color ? color : ~color);
    uint64_t theirs = occupied & (new_color ? ~color : color);

    uint64_t to_flip = 0;
    for (int i = 0; i < 4; i++) {
        uint64_t adjacent = (position & direction_edge[i]) << direction_shift[i];
        uint64_t potential_flip = 0;

        // TODO: optimisable?
        // hmmst how to
        // shift all, and with itself, and with edge, should highlight all ones on the end. take the one on the current row/col/whatever
        while ((adjacent & theirs) != 0) { // while neighboring 
            potential_flip |= adjacent & theirs;
            adjacent = (adjacent & direction_edge[i]) << direction_shift[i];
        }
        if ((adjacent & mine) != 0) {
            to_flip |= potential_flip;
        }

        adjacent = (position & direction_edge[i + 4]) >> direction_shift[i];
        potential_flip = 0;

        while ((adjacent & theirs) != 0) {
            potential_flip |= adjacent & theirs;
            adjacent = (adjacent & direction_edge[i + 4]) >> direction_shift[i];
        }
        if ((adjacent & mine) != 0) {
            to_flip |= potential_flip;
        }
    }

    color ^= to_flip;
}

uint64_t Board::valid_moves(bool new_color) const {
    uint64_t moves = 0;

    uint64_t mine = occupied & (new_color ? color : ~color);
    uint64_t theirs = occupied & (new_color ? ~color : color);

    uint64_t neighbors;
    for (int i = 0; i < 4; i++) {
        // from here: (explanations by yours truly) https://github.com/olinjohnson/Othello/blob/main/Assets/Scripts/Game.cs
        neighbors = ( // gets every piece that is next to ours in the given direction
            (mine & direction_edge[i]) // locations of every one of my pieces except on the edge
            << direction_shift[i]) // move everything once over toward that edge
            & theirs; // locations of every one of their pieces in that location

        while (neighbors) {
            uint64_t potential_moves = (neighbors & direction_edge[i]) << direction_shift[i]; // locations left of theirs that are left of ours
            moves |= potential_moves & ~occupied; // empty ones are valid
            neighbors = potential_moves & theirs; // things with theirs in are part of a longer chain, keep moving
        }

        // same for opposite direction
        neighbors = ((mine & direction_edge[i + 4]) >> direction_shift[i]) & theirs;

        while (neighbors) {
            uint64_t potential_moves = (neighbors & direction_edge[i + 4]) >> direction_shift[i];
            moves |= potential_moves & ~occupied;
            neighbors = potential_moves & theirs;
        }
    }

    return moves;
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

std::string Board::to_dots() const {
    char dots[64 + 8];

    int dot_i = 0;
    for (int i = 0; i < 64; i++) {
        if (!(occupied & OFFSET(i))) {
            dots[dot_i++] = ' ';
        } else if (color & OFFSET(i)) {
            dots[dot_i++] = '@';
        } else {
            dots[dot_i++] = '.';
        }
        if (i % 8 == 7) { // 7, 15, 23, 31, 39, 47, 55, 63
            dots[dot_i++] = '\n';
        }
    }
    
    dots[64 + 7] = '\0';

    return dots;
}

Board::WinState Board::win_state() const {
    auto [black, white] = piece_count();
    if (white == black) {
        return Draw;
    } else if (white > black) {
        return WhiteWins;
    } else {
        return BlackWins;
    }
}

