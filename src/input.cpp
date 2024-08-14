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
    command.push_back(input);
    return arm.process_inner(command, 0);
}

bool CommandArm::process_inner(std::vector<std::string> command, unsigned int index) const {
    if (callback_arms.index() == 1) {
        bool end_of_command = index >= command.size();

        for (auto& arm : std::get<std::vector<CommandArm>>(callback_arms)) {
            if (!arm.condition) {
                return arm.process_inner(command, index);
            } else if (!end_of_command && *arm.condition == command[index]) {
                return arm.process_inner(command, index+1);
            }
        }

        // no more command left and default branch not called/present
        if (end_of_command) {
            std::cerr << "Unexpected end of command" << std::endl;
            return false;
        }

        bool whole_command = index == 0;

        if (whole_command) {
            std::cerr << "Invalid command";
        } else {
            std::cerr << "'" << command[index] << "' is not a valid subcommand (in \x1b[94m";
        }

        for (size_t i = 0; i < index; i++) {
            std::cerr << command[i];
            if (i != index-1) std::cerr << ' ';
        }

        std::cerr << " \x1b[91m" << command[index] << "\x1b[0m";
        if (!whole_command) std::cerr << ')';
        std::cerr << std::endl;

        return false;
    } else {
        command.erase(command.begin(), command.begin() + index);
        std::get<Callback>(callback_arms)(command);
        return true;
    }
}

