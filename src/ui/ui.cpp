#include "ui.hpp"
#include <iostream>
#include <chrono>
#include <functional>
#include <thread>
#include <mutex>

void UI::draw() {
    SDL_SetRenderDrawColor(renderer, 255, 200, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void UI::start(int width, int height, const char* title) {
    if (running) {
        std::cerr << "already started ui" << std::endl;
        return;
    }

    running = true;
    status = Starting;

    ui_thread = std::thread([&, this]() {
        mutex.lock();

        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            throw std::runtime_error("Failed to init video: " + std::string(SDL_GetError()));
        }

        // Create the window
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
        if (!window) {
            throw std::runtime_error("Failed to create window: " + std::string(SDL_GetError()));
        }

        // Create the renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            SDL_DestroyWindow(window);
            throw std::runtime_error("Failed to create renderer: " + std::string(SDL_GetError()));
        }

        status = Idle;
        mutex.unlock();
        cv.notify_one();

        SDL_Event event;

        bool running = true;
        while (running) {
            draw();

            mutex.lock();
            if (status == WantQuit) {
                running = false;
            }
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    on_click.fire(event.button.x, event.button.y);
                }
            }
            mutex.unlock();
        }

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    });

    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this] { return status == Idle; });
}

void UI::stop() {
    if (!running) {
        std::cerr << "already stopped ui" << std::endl;
        return;
    }

    mutex.lock();
    status = WantQuit;
    mutex.unlock();

    ui_thread.join();
    running = false;
}
