#pragma once
#include <backend/backend.hpp>
#include <lexer/lexer.hpp>
#include <parser/parser.hpp>
#include <pch.hpp>

class NCC {
public:
	Lexer lexer;
	Parser parser;
	Backend* backend;

	std::string assembly_output;
	std::vector<std::string> sources;

	// if name == "" then we are not in a function
	FunctionMetadata currentFunction;

	NCC();
	~NCC();

	void compile();
};
