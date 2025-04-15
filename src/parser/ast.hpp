#pragma once
#include <parser/parser.hpp>
#include <pch.hpp>

class Parser;

class AstNode {
public:
	std::vector<AstNode*> children;

	virtual void print() = 0;

	virtual ~AstNode() = 0;
};

class AstRootNode : public AstNode {
public:
	void print();
};

class AstFuncDef : public AstNode {
public:
	std::string name;
	TypeMetadata return_type;

	static AstFuncDef* create(Parser* parser);
	void print();
};