#pragma once
#include <lexer/lexer.hpp>
#include <pch.hpp>

class NCC {
private:
	Lexer lexer;

public:
	std::vector<std::string> sources;

	NCC();
};
