#pragma once

#include <functional>
#include <vector>

template <typename... T>
class Event {
public:
    Event() {}
    void bind(std::function<void(T...)> fn) {
        bindings.push_back(fn);
    }
    void fire(T... args) {
        for (auto& fn : bindings) {
            fn(args...);
        }
    }
private:
    std::vector<std::function<void(T...)>> bindings;
};
