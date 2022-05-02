#pragma once
#include <string>

struct InputResult {
	std::string result;
	bool accepted = false;
};

InputResult BasicInput(const std::string& prompt);