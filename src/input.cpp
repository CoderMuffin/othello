#include <initializer_list>
#include <iostream>
#include "input.hpp"

CommandArm::CommandArm(std::initializer_list<CommandArm> p_arms) : arms(p_arms), variant(Root) {}

CommandArm::CommandArm(std::string p_requirement, std::initializer_list<CommandArm> p_arms) : requirement(p_requirement), arms(p_arms), variant(Arms) {}

CommandArm::CommandArm(std::string p_requirement, Handler p_handler) : requirement(p_requirement), handler(p_handler), variant(Case) {}

CommandArm::CommandArm(Handler p_handler) : handler(p_handler), variant(Default) {}

bool CommandArm::process(std::string input) {
    std::vector<std::string> command;

    size_t pos;
    std::string s = input;
    while ((pos = input.find(' ')) != std::string::npos) {
        s = input.substr(0, pos);
        if (s.length() == 0) continue;

        command.push_back(s);
        input.erase(0, pos + 1); // (' ').length is 1
    }
    command.push_back(input);

    return process(command, 0);
}

bool CommandArm::process(std::vector<std::string>& command, int index) {
    if (command.size() <= index) return false;

    if (variant == Arms || variant == Case) {
        if (requirement != command[index]) {
            return false;
        }
    }

    switch (variant) {
        case Arms:
            index += 1;
            if (command.size() <= index) return false;
        case Root:
            for (auto& arm : arms) {
                if (arm.process(command, index)) {
                    return true;
                }
            }
            std::cout << command[index] << " is not a valid command/subcommand (item " << index << ")" << std::endl;
            return false;
        case Case:
        case Default:
            handler(command);
            return true;
    }
}

