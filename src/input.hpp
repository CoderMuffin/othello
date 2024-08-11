#pragma once

#include <optional>
#include <string>
#include <functional>
#include <variant>

class CommandArm {
    friend class CommandProcessor;
    typedef std::function<void(std::vector<std::string>)> Callback;
public:
    CommandArm(std::string p_condition, Callback p_callback)
        : condition(p_condition), callback_arms(p_callback) {}
    CommandArm(std::string p_condition, std::initializer_list<CommandArm> p_arms)
        : condition(p_condition), callback_arms(p_arms) {}
    CommandArm(Callback p_callback)
        : condition(std::nullopt), callback_arms(p_callback) {}

private:
    CommandArm(std::initializer_list<CommandArm> p_arms)
        : condition(std::nullopt), callback_arms(p_arms);
    bool process_inner(std::vector<std::string>, int) const;
    std::optional<std::string> condition;
    std::variant<Callback, std::initializer_list<CommandArm>> callback_arms;
};

class CommandProcessor {
public:
    CommandProcessor(std::initializer_list<CommandArm> p_arms)
        : arms(p_arms) {}
    bool process(std::string) const;
private:
    std::initializer_list<CommandArm> arms;
};

