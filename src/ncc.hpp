#pragma once
#include <backend/backend.hpp>
#include <lexer/lexer.hpp>
#include <parser/parser.hpp>
#include <pch.hpp>

class NCC {
private:
	Lexer lexer;
	Parser parser;
	Backend* backend;

public:
	std::vector<std::string> sources;

	NCC();
	~NCC();
};
