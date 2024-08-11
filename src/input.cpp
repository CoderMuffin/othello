#include "input.hpp"

#include <iostream>

bool CommandProcessor::process(std::string input) const {
    const std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    std::vector<std::string> command;
    while ((pos = input.find(delimiter)) != std::string::npos) {
        command.push_back(input.substr(0, pos));
        input.erase(0, pos + delimiter.length());
    }
    return process_inner(command, 0);
}

bool CommandArm::process_inner(std::vector<std::string> command, int index) const {
    if (callback_arms.index() == 0) {
        for (auto& arm : std::get<std::initializer_list<CommandArm>>(callback_arms)) {
            if (!arm.condition || (index <= command.size() && *arm.condition == command[index])) {
                return arm.process_inner(command, index+1);
            }
        }

        std::cerr << "\x1b[91m;" << command[index] << "\x1b[0m is not a valid subcommand of \x1b[35m";
        for (int i = 0; i < index; i++) {
            std::cerr << command[i];
            if (i != index) std::cerr << ' ';
        }
        std::cerr << "\x1b[0m" << std::endl;
        return false;
    } else {
        command.erase(command.begin(), command.begin() + index + (condition ? 0 : -1));
        std::get<Callback>(callback_arms)(command);
        return true;
    }
}

