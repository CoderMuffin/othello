#pragma once

#include <SDL.h>
#include "event.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

class UI {
public:
    void start(int width, int height, const char* title);
    void stop();
    virtual void draw();
    std::pair<int, int> wait_click();

    Event<int, int> on_click;

protected:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::thread ui_thread;

    enum Status {
        Starting,
        Idle,
        WantQuit
    };

    bool running = false;

    std::mutex mutex;
    std::condition_variable cv;

    Status status;
};
