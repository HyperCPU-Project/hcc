#pragma once
#include <lexer/lexer.hpp>
#include <metadata.hpp>
#include <parser/parser.hpp>
#include <pch.hpp>
#include <result.hpp>

class Backend;

class NCC {
public:
	Lexer lexer;
	Parser parser;
	Backend* backend;
	bool printAst;

	std::string assembly_output;
	std::vector<std::string> sources;
	std::string output_filename;

	// if name == "" then we are not in a function
	FunctionMetadata currentFunction;

	NCC();
	~NCC();

	Result<NoSuccess, std::string> selectBackend(std::string backend_name);
	void compile();
	Result<NoSuccess, std::string> parseAndCompile();
};
