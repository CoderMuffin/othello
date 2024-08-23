#include "boardui.hpp"

BoardUI::BoardUI() {
    UI::on_click.bind([this](int x, int y) {
        on_click.fire(x / cell_size, y / cell_size);
    });
}

static void draw_circle(SDL_Renderer* renderer, int x, int y, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

void BoardUI::start() {
    UI::start(cell_size * 8, cell_size * 8, "Board");
}

void BoardUI::stop() {
    UI::stop();
}

void BoardUI::set_board(Board& board) {
    if (!running) return;

    mutex.lock();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw the grid
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            SDL_Rect cell = { x * cell_size, y * cell_size, cell_size, cell_size };
            if ((x + y) % 2) {
                SDL_SetRenderDrawColor(renderer, 100, 255, 100, SDL_ALPHA_OPAQUE);
            } else {
                SDL_SetRenderDrawColor(renderer, 50, 200, 50, SDL_ALPHA_OPAQUE);
            }
            SDL_RenderFillRect(renderer, &cell);

            if (BIT(board.occupied, XY(x, y))) {
                if (BIT(board.color, XY(x, y))) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                } else {
                    SDL_SetRenderDrawColor(renderer, 50, 50, 50, SDL_ALPHA_OPAQUE);
                }
                draw_circle(renderer, x * cell_size + cell_size / 2, y * cell_size + cell_size / 2, 15);
            }
        }
    }
    mutex.unlock();
}

void BoardUI::draw() {
    // Update the display
    mutex.lock();
    SDL_RenderPresent(renderer);
    mutex.unlock();
}
