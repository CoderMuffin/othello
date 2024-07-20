#pragma once

#include <functional>
#include <initializer_list>
#include <string>

class CommandArm {
public:
	typedef std::function<void(std::vector<std::string>&)> Handler;

	CommandArm(std::initializer_list<CommandArm>);
	CommandArm(std::string, std::initializer_list<CommandArm>);
	CommandArm(std::string, Handler);
	CommandArm(Handler);

	bool process(std::string input);
	bool process(std::vector<std::string>& command, int index);
private:
	enum {
		Arms,
		Case,
		Default,
		Root
	} variant;
	std::string requirement;
	Handler handler;
	std::vector<CommandArm> arms;
};

